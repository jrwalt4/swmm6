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
public:
    Error(int code = SWMM_ERROR): _code(code) {}

    ErrorCode errcode() const;
    int code() const;
    const char* what() const noexcept override;
};

class IoError: public Error
{
    std::string _msg;
public:
    IoError(std::string msg, int code = SWMM_ERROR): Error(code), _msg(std::move(msg)) {}
    const char* what() const noexcept override;
};

class NotImplementedError: public Error
{
    std::string _method;
public:
    NotImplementedError(std::string method): Error(SWMM_NOIMPL), _method(std::move(method)) {}
    const char* what() const noexcept override;
};

class NoProviderError: public Error
{
    std::string _provider;
public:
    NoProviderError(std::string prv): Error(SWMM_NOPRV), _provider(std::move(prv)) {}
    const char* what() const noexcept override;
};

}
#endif // SWMM_ERROR_H
