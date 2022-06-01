/**
 * @file link.hh
 * @author Reese Walton (jrwalt4@gmail.com)
 * @brief
 * @date 2022-05-29
 *
 */

#ifndef SWMM_LINK_H
#define SWMM_LINK_H

#include "object.hh"

namespace swmm
{

struct Link: public Object
{
public:
    virtual double get_length() = 0;
};

}

#endif // SWMM_LINK_H
