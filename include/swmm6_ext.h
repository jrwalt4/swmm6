#ifndef SWMM_EXT_H
#define SWMM_EXT_H
#include "swmm6.h"

#if __cplusplus
#define ENUM_DECL( NAME ) enum class NAME
#define ENUM_TYPEDEF( NAME )
#else
#define ENUM_DECL( NAME ) enum NAME
#define ENUM_TYPEDEF( NAME ) typedef enum NAME NAME;
#endif // __cplusplus

// forward declarations

ENUM_DECL(swmm6_object_type)
{
  NODE,
  LINK
};
ENUM_TYPEDEF(swmm6_object_type)

ENUM_DECL(swmm6_param_type)
{
  UID,
  INT,
  REAL,
  TEXT,
  UNIT
};
ENUM_TYPEDEF(swmm6_param_type)

typedef struct swmm6_ext_object swmm6_ext_object;
typedef struct swmm6_ext_node swmm6_ext_node;
typedef struct swmm6_ext_link swmm6_ext_link;
typedef struct swmm6_router swmm6_router;

typedef struct swmm6_io_module swmm6_io_module;
typedef struct swmm6_scenario_info swmm6_scenario_info;
typedef struct swmm6_input swmm6_input;
typedef struct swmm6_scenario_cursor swmm6_scenario_cursor;
typedef struct swmm6_input_cursor swmm6_input_cursor;
typedef struct swmm6_output swmm6_output;

typedef struct swmm6_param_def
{
  const char* sName;
  swmm6_param_type eType;
} swmm6_param_def;

typedef struct swmm6_param_pack swmm6_param_pack;

int swmm6_param_int(swmm6_param_pack*, int col);
double swmm6_param_real(swmm6_param_pack*, int col);
const char* swmm6_param_text(swmm6_param_pack*, int col);

typedef struct swmm6_ext_module
{
  const char* sName;
  int iVersion;
  swmm6_object_type eType;
  swmm6_ext_object* (*xCreateObject)(swmm6_uid uid, const char* name);
  int (*xDestroy)(swmm6_ext_object* obj);
  int (*xReadParams)(swmm6_ext_object* obj, swmm6_param_pack* vParams);
  int nParams;
  swmm6_param_def* vParams;
} swmm6_ext_module;


struct swmm6_ext_object
{
  swmm6_uid uid;
  char* name;
  swmm6_ext_module* mod;
};

typedef struct swmm6_node_module
{
  swmm6_ext_module xModule;
  double (*xGetDepth)(const swmm6_ext_node* node);
  double (*xGetInvert)(const swmm6_ext_node* node);
} swmm6_node_module;

struct swmm6_ext_node
{
  swmm6_ext_object object_base;
};

SWMM6_EXPORT
int swmm6_create_node_module(swmm6* prj, swmm6_node_module* mod);

SWMM6_EXPORT
double swmm6_node_getDepth(const swmm6_node* node);

SWMM6_EXPORT
double swmm6_node_getInvert(const swmm6_node* node);

typedef struct swmm6_link_module
{
  swmm6_ext_module xModule;
  double (*xGetLength)(const swmm6_ext_link* link);
} swmm6_link_module;

struct swmm6_ext_link
{
  swmm6_ext_object object_base;
};

SWMM6_EXPORT
int swmm6_create_link_module(swmm6* prj, swmm6_node_module* mod);

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

struct swmm6_io_module
{
  int iVersion;
  const char* sName;
  int (*xOpenInput)(const char* name, swmm6_input** outInp);
  int (*xOpenScenario)(const char* scenario, swmm6_input* inp,
      swmm6_scenario_cursor** info);
  int (*xScenarioNext)(swmm6_scenario_cursor* scn_cursor);
  int (*xOpenCursor)(swmm6_scenario_cursor* inp, swmm6_input_cursor** outCur);
  int (*xCursorNext)(swmm6_input_cursor* cur);

/* cursor reading methods */
  int (*xReadInt)(swmm6_input_cursor* cur, int col);
  double (*xReadDouble)(swmm6_input_cursor* cur, int col);
  const char* (*xReadText)(swmm6_input_cursor* cur, int col);

  int (*xCloseCursor)(swmm6_input_cursor* cur);
  int (*xCloseScenario)(swmm6_input* inp, swmm6_scenario_info* info);
  int (*xCloseInput)(swmm6_input* inp);
  int (*xOpenOutput)(const char* name, swmm6_output** ppOut);
  int (*xCloseOutput)(swmm6_output* pOut);
};

struct swmm6_input
{
  swmm6_io_module* io_methods;
  const char* sName;
  int nCursors;
};

struct swmm6_scenario_cursor
{
  swmm6_input* pInp;
};

struct swmm6_input_cursor
{
  swmm6_input* pInp;
  const char* sProvider;
};

struct swmm6_output
{
  swmm6_io_module* io_methods;
  char* sName;
};

SWMM6_EXPORT
int swmm6_open_with(const char* input, swmm6** pPrj, const swmm6_io_module* io);

typedef struct swmm6_api_routines
{
  const char* (*project_name)(swmm6* pPrj);
  int (*declare_module)(swmm6* pPrj, const char* sName);
} swmm6_api_routines;

typedef int (*swmm6_extension_init)(swmm6* prj, char** ppErrMsg, const swmm6_api_routines* api);

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
