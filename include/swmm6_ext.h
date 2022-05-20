#ifndef SWMM_EXT_H
#define SWMM_EXT_H
#include "swmm6.h"

// forward declarations

typedef enum swmm6_object_type
{
  NODE,
  LINK
} swmm6_object_type;

typedef struct swmm6_object swmm6_object;
typedef struct swmm6_node swmm6_node;
typedef struct swmm6_link swmm6_link;
typedef struct swmm6_router swmm6_router;

typedef struct swmm6_io_module swmm6_io_module;
typedef struct swmm6_scenario_info swmm6_scenario_info;
typedef struct swmm6_input swmm6_input;
typedef struct swmm6_input_cursor swmm6_input_cursor;
typedef struct swmm6_output swmm6_output;

/* Objects */
typedef struct swmm6_object_methods
{
  int (*xDestroy)(swmm6_object* obj);
} swmm6_object_methods;

struct swmm6_object
{
  swmm6_uid uid;
  char* name;
  swmm6_object_methods* object_methods;
};

SWMM6_EXPORT
int swmm6_object_destroy(swmm6_object* pObj);

typedef struct swmm6_node_methods
{
  swmm6_object_methods object_methods;
  double (*xGetDepth)(const swmm6_node* node);
  double (*xGetInvert)(const swmm6_node* node);
} swmm6_node_methods;

struct swmm6_node
{
  swmm6_object object_base;
  swmm6_node_methods* node_methods;
};

SWMM6_EXPORT
double swmm6_node_getDepth(const swmm6_node* node);

SWMM6_EXPORT
double swmm6_node_getInvert(const swmm6_node* node);

typedef struct swmm6_link_methods
{
  swmm6_object_methods object_methods;
  double (*xGetLength)(const swmm6_link* link);
} swmm6_link_methods;

struct swmm6_link
{
  swmm6_object object_base;
  swmm6_link_methods* link_methods;
};

SWMM6_EXPORT
double swmm6_link_getLength(const swmm6_link* node);

typedef struct swmm6_router_module swmm6_router_module;

struct swmm6_router
{
  const swmm6_router_module* pModule;
  int nRef;
  char* sErrMsg;
};

struct swmm6_router_module
{
  int (*xOpen)(swmm6* prj, void* pUserData, swmm6_router** ppRouter, int nArg, char* vArg[]);
  int (*xRoute)(swmm6* prj, swmm6_router* route, swmm6_link* pLink, swmm6_node aNode[2]);
  int (*xClose)(swmm6_router* pRouter);
};

SWMM6_EXPORT
int swmm6_create_router(swmm6* prj, const char* sName, swmm6_router_module* sMod, void* pUserData);

/* provider */
typedef struct swmm6_provider swmm6_provider;

#define SWMM_MAX_NAME 255
// include null terminator
#define SWMM_NAME_BUFFER_SIZE 256 

typedef struct swmm6_builder
{
  swmm6_uid uid;
  char  name[SWMM_NAME_BUFFER_SIZE];
  //swmm6_provider* provider;
} swmm6_builder;

typedef enum swmm6_param_type
{
  SWMM_INT,
  SWMM_REAL,
  SWMM_TEXT
} swmm6_param_type;

struct swmm6_provider
{
  int iVersion;
  swmm6_object_type eType;
  const char* sKind;
  swmm6_object_methods* methods;
  int nParams;
  char** aParams;
  swmm6_param_type* aParamTypes;
  int (*xGetBuilder)(swmm6_builder** outBldr);
  int (*xCreateObject)(swmm6_builder* bldr, swmm6_object** outObj);
  int (*xReleaseBuilder)(swmm6_builder* bldr);
  int (*xResetBuilder)(swmm6_builder* bldr);
  int (*xReadCursor)(swmm6_input_cursor* cur, swmm6_object** outObj);
};

SWMM6_EXPORT
int swmm6_create_provider(swmm6* prj, swmm6_provider* prv);

struct swmm6_io_module
{
  int iVersion;
  char* sName;
  int (*xOpenInput)(const char* name, swmm6_input** outInp);
  int (*xDescribeScenario)(const char* scenario, swmm6_input* inp,
      swmm6_scenario_info** info);
  int (*xOpenCursor)(swmm6_input* inp, const char* query, swmm6_provider* prv, swmm6_input_cursor** outCur);
  int (*xNext)(swmm6_input_cursor* cur);

/* cursor reading methods */
  int (*xReadInt)(swmm6_input_cursor* cur, int col);
  double (*xReadDouble)(swmm6_input_cursor* cur, int col);
  const char* (*xReadText)(swmm6_input_cursor* cur, int col);

  int (*xCloseCursor)(swmm6_input_cursor* cur);
  int (*xReleaseScenario)(swmm6_input* inp, swmm6_scenario_info* info);
  int (*xCloseInput)(swmm6_input* inp);
  int (*xOpenOutput)(const char* name, swmm6_output** ppOut);
  int (*xCloseOutput)(swmm6_output* pOut);
};

struct swmm6_input
{
  swmm6_io_module* io_methods;
  char* sName;
  int nCursors;
};

struct swmm6_scenario_info
{
  char** aQueries; // array of queries to build model
  char** aProviders;//array of providers associated with each query
  int nQueries; // length of query array
};

struct swmm6_input_cursor
{
  swmm6_input* pInp;
};

struct swmm6_output
{
  swmm6_io_module* io_methods;
  char* sName;
};

SWMM6_EXPORT
int swmm6_register_io_module(swmm6_io_module* mod, int makeDefault);

SWMM6_EXPORT
swmm6_io_module* swmm6_find_io_module(const char* sName);

SWMM6_EXPORT
int swmm6_describe_input(swmm6* prj, const char* schema);

/* builder structs for custom inputs to read core objects */

typedef struct swmm6_junction_builder
{
  swmm6_builder builder_base;
  double invert;
  double rim;
} swmm6_junction_builder;


typedef struct swmm6_api_routines
{
  const char* (*project_name)(swmm6* pPrj);
  int (*declare_module)(swmm6* pPrj, const char* sName);
} swmm6_api_routines;

SWMM6_EXPORT
int swmm6_extension_init(swmm6* prj, char** ppErrMsg, const swmm6_api_routines* api);

#ifndef SWMM_CORE
#define SWMM_EXT_INIT1 const swmm6_api_routines* swmm6api = 0;
#define SWMM_EXT_INIT2( api ) swmm6api = api;

#define swmm6_project_name     swmm6api->project_name
#define swmm6_declare_module   swmm6api->declare_module

#else
// no-ops otherwise
#define SWMM_EXT_DECL_API
#define SWMM_EXT_SET_API( api )


#endif // SWMM_CORE

#endif // SWMM_EXT_H
