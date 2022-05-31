/**
 * @file junction.hh
 * @author Reese Walton (jrwalt4@gmail.com)
 * @brief
 * @date 2022-05-31
 *
 */
#ifndef SWMM_NODES_JUNCTION_H
#define SWMM_NODES_JUNCTION_H

#include "node.hh"

#include "provider.hh"

namespace swmm
{

class JunctionNode: public Node
{

  double _invert;
  double _rim;

public:
  using Node::Node;

  struct Provider: public ProviderBase
  {
    Provider();

    JunctionNode* create_object(swmm6_uid uid, const char* name) override;
    void read_params(Object& obj, ParamPack& values) override;
  };

  //friend Provider;

  double get_depth() override;

  double get_invert() override;
};

} // namespace swmm

#endif // SWMM_NODES_JUNCTION_H
