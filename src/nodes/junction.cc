#include "junction.hh"

namespace swmm
{

double JunctionNode::get_depth()
{
    return _rim - _invert;
}

double JunctionNode::get_invert()
{
    return _invert;
}

JunctionNode::Provider::Provider(): ProviderBase(
    "JUNCTION",
    {
        {"INVERT", swmm6_param_type::REAL},
        {"RIM", swmm6_param_type::REAL}
    }
) {}

JunctionNode* JunctionNode::Provider::create_object(swmm6_uid uid, const char* name)
{
    return new JunctionNode(uid, name);
}

void JunctionNode::Provider::read_params(Object& obj, ParamPack& values)
{
    JunctionNode& jxn = dynamic_cast<JunctionNode&>(obj);
    jxn._invert = values.get_real(0);
    jxn._rim = values.get_real(1);
}

} // namespace swmm
