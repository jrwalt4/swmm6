/**
 * @file object.hh
 * @author Reese Walton (jrwalt4@gmail.com)
 * @brief
 * @date 2022-05-29
 *
 */

#ifndef SWMM_OBJECT_H
#define SWMM_OBJECT_H

#include "swmm6_int.hh"

#include "input.hh"

#include <string>
#include <utility>

namespace swmm
{

struct Object
{
    swmm6_uid uid;
    std::string name;

    Object(): uid(-1){};
    Object(swmm6_uid obj_id, const char* obj_name):
        uid(obj_id), name(std::string{obj_name}) {}

    virtual ~Object() = default;
};

struct ExtensionObject: public virtual Object
{
  swmm6_ext_object* _obj;
  swmm6_ext_module* _module;
  ExtensionObject(swmm6_ext_object* obj): _obj(obj), _module(obj->mod) {}

  ~ExtensionObject();
};

}

#endif // SWMM_OBJECT_H
