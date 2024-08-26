cmake_minimum_required(VERSION 3.5)

project(connection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main main.cpp connection.cpp connectionpool.cpp)

target_link_libraries(main PRIVATE
    /opt/homebrew/opt/mysql/lib/libmysqlclient.dylib
)

target_include_directories(main PRIVATE
    /opt/homebrew/opt/mysql/include/mysql
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

link_directories("/opt/homebrew/opt/mysql/lib")