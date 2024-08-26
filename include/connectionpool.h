#ifndef __CONNECTION_POOL__
#define __CONNECTION_POOL__

#include "connection.h"

#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <functional>
#include <condition_variable>

class ConnectionPool {
public:
    static ConnectionPool* getConnectionPool();

    std::shared_ptr<Connection> getConnection(); // 从连接池中获取一个空闲的连接

private:
    ConnectionPool();

    // 从配置文件中加载配置项
    bool loadConfigFile();

    // 运行在独立的线程中, 负责生产新的连接
    void produceConnection();

    // 运行在独立的线程中, 负责回收连接队列中空闲时间超过 `_maxIdleTime` 的连接
    void recycleIdleConnection();

    // 数据库的 IP 地址、端口号、登录用户名、密码和数据库名称
    std::string _ip;
    unsigned short _port;
    std::string _username;
    std::string _password;
    std::string _dbName;

    int _initSize;          // 连接池初始连接量
    int _maxSize;           // 连接池最大连接量
    int _maxIdleTime;       // 连接池中连接的最大空闲时间
    int _connectionTimeOut; // 连接池获取连接的超时时间

    std::queue<Connection*> _connectionQueue; // 空闲连接队列
    std::mutex _queueMutex;                        // 维护连接队列的锁

    std::atomic_int _connectionCnt; // 原子整数类型, 记录创建的连接的总数量
    std::condition_variable cv;     // 设置条件变量, 用于生产者线程和消费者线程的通信
};

#endif