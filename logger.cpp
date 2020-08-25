//
// Created by jiaming pan on 8/10/20.
//

#include "logger.h"
#include "iostream"
#include <thread>
#include <iostream>
logger::level  g_logLevel = logger::level::CLOSED;
logger::~logger() {
  std::cout << m_os.str() << std::endl;
}
logger::logger(const std::string &file, int line, logger::level, const char *func)
{

    auto tid = std::this_thread::get_id();
    m_os <<"threadId:" << tid <<" "<< file <<" line:" << line << " " <<  "TRACE" <<" ";
}

logger::level logger::logLevel() {
    return g_logLevel;
}

