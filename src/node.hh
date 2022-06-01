/**
 * @file node.hh
 * @author Reese Walton (jrwalt4@gmail.com)
 * @brief
 * @date 2022-05-29
 *
 */

#ifndef SWMM_NODE_H
#define SWMM_NODE_H

#include "swmm6_int.hh"

#include "object.hh"

namespace swmm
{

struct Node: public Object
{
    using Object::Object;

    virtual double get_depth() = 0;
    virtual double get_invert() = 0;
};

/**
 * Called during prj initialization to load builtin nodes
 * (JUNCTION, STORAGE, OUTFALL, etc.)
 */
int createBuiltinNodeProviders(swmm6& prj);

}

#endif // SWMM_NODE_H
