#include "node.hh"

#include "input.hh"
#include "provider.hh"

using namespace std;

namespace swmm
{

class JunctionNode: public Node
{

  double _invert;
  double _rim;

public:
  using Node::Node;


  class Provider: public swmm::Provider
  {
    JunctionNode* read_cursor(InputObjectCursor& cursor) override
    {
      JunctionNode* jxn = new JunctionNode(cursor);
      jxn->_invert = cursor.readDouble(2);
      jxn->_rim = cursor.readDouble(3);
      return jxn;
    }
  };

  double get_depth() override
  {
    return _rim - _invert;
  }

  double get_invert() override
  {
    return _invert;
  }
};

int createBuiltinNodeProviders(swmm6* prj)
{
  return registerProvider(prj, new JunctionNode::Provider());
}

} // namespace swmm
