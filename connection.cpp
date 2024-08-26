#include "connection.h"
#include "public.h"

using namespace std;

Connection::Connection()
{
    // 初始化数据库连接
    _connection = mysql_init(nullptr);
}

Connection::~Connection()
{
    if (_connection != nullptr)
        mysql_close(_connection);
}

bool Connection::connect(string ip, unsigned short port, string user, 
        string password, string dbName)
{
    MYSQL *p = mysql_real_connect(_connection, ip.c_str(), user.c_str(), 
        password.c_str(), dbName.c_str(), port, nullptr, 0);
    if (p == nullptr)
        return false;
    return true;
}

bool Connection::update(string sql)
{
    if (mysql_query(_connection, sql.c_str())) {
        LOG("更新失败: " + sql);
        return false;
    }
    return true;
}

MYSQL_RES* Connection::query(string sql)
{
    if (mysql_query(_connection, sql.c_str())) {
        LOG("查询失败: " + sql);
        return nullptr;
    }
    return mysql_use_result(_connection);
}
