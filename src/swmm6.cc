#include "swmm6_int.hh"

#include "error.hh"
#include "input.hh"
#include "provider.hh"
#include "simulation.hh"

#include <assert.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>

using namespace std;
using namespace swmm;

struct swmm6
{
private:
  unique_ptr<Input> _input;

  unordered_map<string, ProviderBase&> _providers;

  unordered_map<string, unique_ptr<Simulation>> _simulations;

public:
  explicit swmm6(Input* inp): _input(inp) {}

  Input& get_input() const noexcept
  {
    return *_input;
  }

  bool register_provider(ProviderBase& prv)
  {
    return std::get<1>(_providers.insert( { prv.name, prv }));
  }

  ProviderBase& find_provider(string& prv_name)
  {
    return (*_providers.find(prv_name)).second;
  }

};

namespace swmm
{

bool registerProvider(swmm6& prj, ProviderBase& prv)
{
  return prj.register_provider(prv);
}

} // namespace swmm

int swmm6_open(const char* input, swmm6** pPrj)
{
  return swmm6_open_with(input, pPrj, NULL);
}

int swmm6_open_with(const char* inpName, swmm6** pPrj, const swmm6_io_module* io)
{
  Input* inp;
  try {
    inp = Input::open(inpName, io);
    if(inp == nullptr) {
      return SWMM_ERROR;
    }
  } catch (IoError& err) {
    return err.code();
  }
  swmm6* prj = new swmm6(inp);
  if(prj == NULL) {
    return SWMM_ERROR;
  }

  int rc = createBuiltinNodeProviders(*prj);
  if(rc) {
    delete prj;
    return rc;
  }
  *pPrj = prj;
  return SWMM_OK;
}

int swmm6_open_simulation(const char* scenario, swmm6* prj, swmm6_simulation** pSim, char** zErr)
{
  (void) zErr;
  Input& inp = prj->get_input();
  Simulation* sim = new Simulation(*prj, scenario);

  if(sim == nullptr) {
    return SWMM_NOMEM;
  }

  auto nodeCursor = unique_ptr<InputCursor>(inp.openNodeCursor(scenario));
  auto result = nodeCursor->next();
  while(result.first) {
    auto props = result.second;
    ProviderBase& prv = prj->find_provider(props.name);
    Node* node = dynamic_cast<Node*>(prv.create_object(props.uid, props.name.c_str()));
    bool success = sim->add_node(node);
    if(success) {
      result = nodeCursor->next();
    } else {
      delete sim;
      return SWMM_ERROR;
    }
  }
  *pSim = (swmm6_simulation*) sim;
  return SWMM_OK;
}

int swmm6_close(swmm6* prj)
{
  int rc = SWMM_OK;
  puts("Closing simulations");
  delete prj;

  return rc;
}
