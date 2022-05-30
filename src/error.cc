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

} // namespae swmm
