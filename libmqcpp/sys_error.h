//
// Created by deth on 19/12/2019.
//

#ifndef MSGQ_SYS_ERROR_H
#define MSGQ_SYS_ERROR_H

#include <system_error>
#include <errno.h>

inline std::system_error make_system_error(int error_code = errno, const char* what = nullptr)
{
    if (what != nullptr) {
        return std::system_error(error_code, std::system_category(), what);
    }
    return std::system_error(error_code, std::system_category());
}

[[noreturn]]
inline void throw_system_error(int error_code = errno, const char* what = nullptr)
{
    throw make_system_error(error_code, what);
}


#endif //MSGQ_SYS_ERROR_H
