#ifndef __PUBLIC__
#define __PUBLIC__

#include <iostream>

// 错误日志
#define LOG(str) \
    std::cout << __FILE__ << ":" << __LINE__ << " " << \
    __TIMESTAMP__ << " : " << str << std::endl;

#endif