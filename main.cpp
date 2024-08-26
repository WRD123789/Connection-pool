#include <iostream>

#include "connection.h"
#include "connectionpool.h"

using namespace std;

int main()
{
    clock_t begin = clock();

    ConnectionPool *connectionPool = ConnectionPool::getConnectionPool();

    thread thread0([&connectionPool](){
        for (int i = 0; i < 2500; i += 1) {
            shared_ptr<Connection> connection = connectionPool->getConnection();
            string sql = "INSERT INTO user(name, age) \
                        VALUES('Test', 20)";
            if (connection->update(sql))
                std::cout << "数据插入成功.." << std::endl;
            else
                std::cout << "数据插入失败.." << std::endl;
        }
    });

    thread thread1([&connectionPool](){
        for (int i = 0; i < 2500; i += 1) {
            shared_ptr<Connection> connection = connectionPool->getConnection();
            string sql = "INSERT INTO user(name, age) \
                        VALUES('Test', 20)";
            if (connection->update(sql))
                std::cout << "数据插入成功.." << std::endl;
            else
                std::cout << "数据插入失败.." << std::endl;
        }
    });

    thread thread2([&connectionPool](){
        for (int i = 0; i < 2500; i += 1) {
            shared_ptr<Connection> connection = connectionPool->getConnection();
            string sql = "INSERT INTO user(name, age) \
                        VALUES('Test', 20)";
            if (connection->update(sql))
                std::cout << "数据插入成功.." << std::endl;
            else
                std::cout << "数据插入失败.." << std::endl;
        }
    });

    thread thread3([&connectionPool](){
        for (int i = 0; i < 2500; i += 1) {
            shared_ptr<Connection> connection = connectionPool->getConnection();
            string sql = "INSERT INTO user(name, age) \
                        VALUES('Test', 20)";
            if (connection->update(sql))
                std::cout << "数据插入成功.." << std::endl;
            else
                std::cout << "数据插入失败.." << std::endl;
        }
    });

    clock_t end = clock();
    cout << end - begin << " ms" << endl;

    return 0;
}