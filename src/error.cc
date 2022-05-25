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

ErrorCode Error::code() const
{
    return (ErrorCode) _code;
}

} // namespae swmm
