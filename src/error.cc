/**
 * @file error.cc
 * @author Reese Walton (jrwalt4@gmail.com)
 * @brief
 * @date 2022-05-22
 *
 */

#include "error.hh"

namespace swmm
{

ErrorCode Error::errcode() const
{
    return (ErrorCode) _code;
}

int Error::code() const
{
    return _code;
}

const char* Error::what() const noexcept
{
    return "Error";
}

const char* IoError::what() const noexcept
{
    return ("IoError: " + _msg).c_str();
}

const char* NotImplementedError::what() const noexcept
{
    return ("Method `" + _method + "` not implemented").c_str();
}

const char* NoProviderError::what() const noexcept
{
    return ("Provider `" + _provider + "` not found").c_str();
}

} // namespae swmm
