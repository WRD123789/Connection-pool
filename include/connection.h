#ifndef __CONNECTION__
#define __CONNECTION__

#include <string>
#include <mysql.h>
#include <ctime>

class Connection {
public: 
    Connection();
    ~Connection();

    bool connect(std::string ip, unsigned short port, std::string user, 
        std::string password, std::string dbName); // 连接数据库
    bool update(std::string sql);                  // 更新相关操作, insert、delete、update
    MYSQL_RES* query(std::string sql);             // 查询操作, select

    inline void refreshStartTime() { _startTime = clock(); }
    // 获取连接的空闲时间 (在连接队列中停留的时间)
    inline clock_t getIdleTime() { return clock() - _startTime; }

private:
    MYSQL *_connection; // 和 MySQL Server 的一条连接
    clock_t _startTime; // 记录连接进入连接队列的时间
};

#endif