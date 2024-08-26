#include "connectionpool.h"
#include "public.h"

using namespace std;

ConnectionPool* ConnectionPool::getConnectionPool()
{
    // 静态局部变量是线程安全的
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::loadConfigFile()
{
    FILE *config = fopen("/Users/wangrundong/Documents/Projects/ConnectionPool/mysql.cnf", "r");
    if (config == nullptr) {
        LOG("mysql.cnf 文件不存在!");
        return false;
    }

    while (!feof(config)) {
        char line[1024] = {0};
        fgets(line, 1024, config);

        string str = line;
        int begin = str.find('=', 0);
        if (begin == -1)
            continue;
        
        int end = str.find('\n', begin);
        string key = str.substr(0, begin - 1);
        string value = str.substr(begin + 2, end - begin - 2);

        if (key == "ip")
            _ip = value;
        else if (key == "port")
            _port = atoi(value.c_str());
        else if (key == "username")
            _username = value;
        else if (key == "password")
            _password = value;
        else if (key == "dbName")
            _dbName = value;
        else if (key == "initSize")
            _initSize = atoi(value.c_str());
        else if (key == "maxSize")
            _maxSize = atoi(value.c_str());
        else if (key == "maxIdleTime")
            _maxIdleTime = atoi(value.c_str());
        else if (key == "connectionTimeOut")
            _connectionTimeOut = atoi(value.c_str());
    }

    return true;
}

ConnectionPool::ConnectionPool()
{
    if (!loadConfigFile())
        return;
    
    // 创建初始数量的连接
    for (int i = 0; i < _initSize; i += 1) {
        Connection *connection = new Connection();
        connection->connect(_ip, _port, _username, _password, _dbName);
        connection->refreshStartTime();

        _connectionQueue.push(connection);
        _connectionCnt += 1;
    }

    // 启动一个新的线程, 作为连接的生产者
    thread produceThread(std::bind(&ConnectionPool::produceConnection, this));
    produceThread.detach(); // 分离线程和 `std::thread` 对象, 避免对象销毁时终止线程

    // 启动一个新的线程, 回收连接队列中空闲时间超过 `_maxIdleTime` 的连接
    thread recycleThread(std::bind(&ConnectionPool::recycleIdleConnection, this));
    recycleThread.detach();
}

void ConnectionPool::produceConnection()
{
    for (;;) {
        // 智能锁, 构建时获取锁, 析构时释放锁
        unique_lock<mutex> lock(_queueMutex);
        while (!_connectionQueue.empty())
            cv.wait(lock); // 队列不空时, 生产线程进入等待状态
        
        // 连接数量没有到达上限, 继续生产新的连接
        if (_connectionCnt < _maxSize) {
            Connection *connection = new Connection();
            connection->connect(_ip, _port, _username, _password, _dbName);
            connection->refreshStartTime();

            _connectionQueue.push(connection);
            _connectionCnt += 1;
        }

        cv.notify_all();
    }
}

shared_ptr<Connection> ConnectionPool::getConnection()
{
    unique_lock<mutex> lock(_queueMutex);
    while (_connectionQueue.empty()) {
        cv_status status = cv.wait_for(lock, chrono::milliseconds(_connectionTimeOut));
        if (status == cv_status::timeout) {
            if (_connectionQueue.empty()) {
                LOG("获取连接超时..");
                return nullptr;
            }
        }
    }

    // 自定义智能指针释放资源的方式, 在指针销毁时把连接归还给连接队列
    shared_ptr<Connection> sp(_connectionQueue.front(), 
        [this](Connection *connection) {
            unique_lock<mutex> lock(_queueMutex);
            connection->refreshStartTime();
            _connectionQueue.push(connection);
        });
    _connectionQueue.pop();

    cv.notify_all();
    
    return sp;
}

void ConnectionPool::recycleIdleConnection()
{
    for (;;) {
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));

        unique_lock<mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize) {
            Connection *connection = _connectionQueue.front();
            if (connection->getIdleTime() >= (_maxIdleTime * 1000)) {
                _connectionQueue.pop();
                _connectionCnt -= 1;
                delete connection;
            } else {
                // 根据队列先进先出的特性, 队列中连接的空闲时间从头到尾逐渐递减
                break;
            }
        }
    }
}