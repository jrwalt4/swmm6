/**
 * @file error.hh
 * @author Reese Walton (jrwalt4@gmail.com)
 * @brief Errors
 * @date 2022-05-22
 *
 */

#ifndef SWMM_ERROR_H
#define SWMM_ERROR_H

#include "swmm6_int.hh"

#include <exception>
#include <string>

namespace swmm
{

enum class ErrorCode
{
    NOID = -1,
    OK = 0,
    ERROR,
    NOMEM,
    ABORT,
    NOTFOUND,
    NOIMPL,

    ERROR_MAX
};

enum class ResultCode
{
    ROW = 256,
    DONE,

    RESULT_MAX
};

class Error: public std::exception
{
    int _code;
    std::string _msg;
public:
    Error(int code = SWMM_ERROR, std::string msg = ""): _code(code), _msg(std::move(msg)) {}
    Error(std::string msg): Error(SWMM_ERROR, msg) {}

    ErrorCode errcode() const;
    int code() const;
    const char* what() const noexcept override;
};

class IoError: public Error
{
public:
    IoError(std::string msg, int code = SWMM_ERROR): Error(code, msg) {}
};

class NotImplementedError: public Error
{
public:
    NotImplementedError(std::string method): Error(SWMM_NOIMPL, method + " not Implemented") {}
};

class NoProviderError: public Error
{
public:
    NoProviderError(std::string prv): Error(SWMM_NOPRV, "No Provider: "+prv) {}
};

}
#endif // SWMM_ERROR_H
