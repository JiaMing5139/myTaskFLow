//
// Created by jiaming pan on 8/10/20.
//

#ifndef MYTASKFLOW_LOGGER_H
#define MYTASKFLOW_LOGGER_H

#include <ostream>
#include <string>
#include <sstream>

class logger{
public:
    enum level{
        CLOSED,
        TRACE
    };
    logger(const std::string &file, int line, level lev, const char* func);
    ~logger();
    std::ostream & stream(){
        return this->m_os;
    }
#define LOG_TRACE  if(logger::TRACE <= logger::logLevel()) \
           logger(__FILE__, __LINE__, logger::TRACE, __func__).stream() \

    static level logLevel();

    logger(const logger & logger1) = delete;
    logger & operator = (const logger & logger1) = delete ;

private:
    std::ostringstream m_os;
    level  m_level;
    int line;
};

#endif //MYTASKFLOW_LOGGER_H
