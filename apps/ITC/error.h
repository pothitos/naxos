// Part of https://github.com/pothitos/naxos

#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>
#include <string>

class timetableException : public std::logic_error {
    public:
        timetableException(const std::string& error_message)
          : logic_error("Timetabling: " + error_message)
        {
        }
};

// Panic function for Timetabling
inline void assert_that(const bool test, const char* error_message)
//  Here we used the type 'char*' for 'error_message',
//  instead of 'string', plainly for time-performance reasons.
{
        if (!test)
                throw timetableException(error_message);
}

#endif // ERROR_H
