#include "input.hh"

#include "error.hh"

#include <assert.h>
#include <sqlite3.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unordered_map>

using namespace std;

namespace swmm
{

template<>
int InputCursor::read(int col)
{
  return readInt(col);
}

template<>
double InputCursor::read(int col)
{
  return readDouble(col);
}

template<>
const std::string InputCursor::read(int col)
{
  return readText(col);
}

class Swmm6InputCursor: public InputCursor
{
  sqlite3_stmt* _stmt;
public:
  Swmm6InputCursor(sqlite3_stmt* stmt): _stmt(stmt) {}

  bool next() override
  {
    int rc = sqlite3_step(_stmt);
    if(rc == SQLITE_ROW) {
      return true;
    }
    if(rc == SQLITE_DONE) {
      return false;
    }
    throw IoError(sqlite3_errmsg(sqlite3_db_handle(_stmt)));
  }

  int readInt(int col) override
  {
    return sqlite3_column_int(_stmt, col);
  }

  double readDouble(int col) override
  {
    return sqlite3_column_double(_stmt, col);
  }
  const std::string readText(int col) override
  {
    return (const char*) sqlite3_column_text(_stmt, col);
  }
};

class Swmm6Input: public Input
{
  sqlite3* _db;
public:
  Swmm6Input(const char* dbName)
  {
    sqlite3_open_v2(dbName, &_db, SQLITE_READONLY, NULL);
  }

  ~Swmm6Input()
  {
    sqlite3_close_v2(_db);
  }

  swmm6_scenario_info* describeScenario(const char* scenario) override
  {
    (void) scenario;
    static const char* queries[] = {
      "SELECT uid, name, invert, rim FROM JUNCTION;"
    };
    static const char* providers[] = {
      "JUNCTION"
    };
    static swmm6_scenario_info sqlInfo = {
      queries,
      providers,
      1
    };
    return &sqlInfo;
  }

  void releaseScenario(swmm6_scenario_info* info) override
  {
    (void) info;
  }

  Swmm6InputCursor* openCursor(const char* query, const swmm6_scenario_info& info) override
  {
    (void) info;
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(_db, query, -1, &stmt, NULL);
    if(rc) {
      throw IoError(sqlite3_errmsg(_db));
    }
    return new Swmm6InputCursor(stmt);
  }
};

class ExtensionInputCursor: public InputCursor
{
  swmm6_input_cursor* _cursor;
  swmm6_io_module* _methods;
public:
  ExtensionInputCursor(swmm6_input_cursor* cursor): _cursor(cursor), _methods(cursor->pInp->io_methods) {}
  ~ExtensionInputCursor()
  {
    _methods->xCloseCursor(_cursor);
  }
  bool next() override
  {
    return _methods->xNext(_cursor);
  }
  int readInt(int col) override
  {
    return _methods->xReadInt(_cursor, col);
  }
  double readDouble(int col) override
  {
    return _methods->xReadDouble(_cursor, col);
  }
  const std::string readText(int col) override
  {
    return _methods->xReadText(_cursor, col);
  }
};

class ExtensionInput: public Input
{
  swmm6_input* _input;
  swmm6_io_module* _methods;

public:
  ExtensionInput(swmm6_input* input): _input(input), _methods(input->io_methods) {}

  ~ExtensionInput()
  {
    _methods->xCloseInput(_input);
  }

  swmm6_scenario_info* describeScenario(const char* scenario) override
  {
    swmm6_scenario_info* info;
    int rc = _methods->xDescribeScenario(scenario, _input, &info);
    if(rc) {
      throw IoError("Error describing scenario", rc);
    }
    return info;
  }

  virtual ExtensionInputCursor* openCursor(const char* query, const swmm6_scenario_info& info) override
  {
    swmm6_input_cursor* cursor;
    int rc = _methods->xOpenCursor(_input, query, addressof(info), &cursor);
    if(rc) {
      throw IoError("Error opening cursor", rc);
    }
    return new ExtensionInputCursor(cursor);
  }

  void releaseScenario(swmm6_scenario_info* info) override
  {
    int rc = _methods->xReleaseScenario(_input, info);
    if(rc) {
      throw IoError("Cannot release scenario info", rc);
    }
  }
};

static unordered_map<string, swmm6_io_module*> extensionInputModules;

#define SWMM6_INPUT_MODULE "swmm6"
static string defaultInput = SWMM6_INPUT_MODULE;

} // namespace swmm

int swmm6_register_io_module(swmm6_io_module* pMod, int makeDefault)
{
  using namespace swmm;
  auto [ iter, success ] = extensionInputModules.insert({pMod->sName, pMod});
  if(!success) {
    return SWMM_ERROR;
  }
  if(makeDefault) {
    defaultInput = iter->first;
  }
  return SWMM_OK;
}


swmm::Input* inputOpen(const char* sInpName, const char* sModule)
{
  using namespace swmm;
  string moduleName = sModule == NULL ? defaultInput : sModule;
  swmm6_io_module* ioModule = extensionInputModules.at(sModule);
  if(ioModule == NULL) {
    if(moduleName == SWMM6_INPUT_MODULE) {
      // using builtin module
      return new Swmm6Input(sInpName);
    }
    throw IoError("No IO Module: " + string(sModule));
  }
  swmm6_input* pInp;
  int rc = ioModule->xOpenInput(sInpName, &pInp);
  if(rc) {
    throw IoError("Unable to open", rc);
  }
  return new ExtensionInput(pInp);
}

int inputDescribeScenario(const char* scenario, swmm6_input* inp, swmm6_scenario_info** outInfo)
{
  if(inp->io_methods->xDescribeScenario == NULL) {
    return SWMM_NOIMPL;
  }
  return inp->io_methods->xDescribeScenario(scenario, inp, outInfo);
}

int inputReleaseScenario(swmm6_input* inp, swmm6_scenario_info* info)
{
  VIRTUAL_CALL(inp->io_methods,xReleaseScenario, inp, info);
}

int inputOpenCursor(swmm6_input* inp, const char* query, const swmm6_scenario_info* info, swmm6_input_cursor** outCursor)
{
  VIRTUAL_CHECK(inp->io_methods, xOpenCursor)
  int rc = inp->io_methods->xOpenCursor(inp, query, info, outCursor);
  if(rc) {
    return rc;
  }
  (*outCursor)->pInp = inp;
  return SWMM_OK;
}


int inputNext(swmm6_input_cursor* cur)
{
  VIRTUAL_CALL(cur->pInp->io_methods, xNext, cur)
}

int inputReadInt(swmm6_input_cursor* cur, int col)
{
  return cur->pInp->io_methods->xReadInt(cur, col);
}

double inputReadDouble(swmm6_input_cursor* cur, int col)
{
  return cur->pInp->io_methods->xReadDouble(cur, col);
}

const char* inputReadText(swmm6_input_cursor* cur, int col)
{
  return cur->pInp->io_methods->xReadText(cur, col);
}

int inputCloseCursor(swmm6_input_cursor* cursor)
{
  VIRTUAL_CALL(cursor->pInp->io_methods,xCloseCursor,cursor);
}

int inputClose(swmm6_input* inp)
{
  free((void*)inp->sName);
  VIRTUAL_CALL(inp->io_methods,xCloseInput, inp)
}

/********************************
 * Default SQLite3 Input Module *
 ********************************/

typedef struct SqliteInput
{
  swmm6_input base; // base class
  sqlite3* db;
} SqliteInput;

typedef struct SqliteInputCursor
{
  swmm6_input_cursor base;
  sqlite3_stmt* stmt;
} SqliteInputCursor;

int swmmSqliteOpen(const char* name, swmm6_input** outInp)
{
  SqliteInput* sqlInp;
  int rc;
  sqlite3* db;
  rc = sqlite3_open_v2(name, &db, SQLITE_OPEN_READONLY, NULL);
  if(rc) {
    puts(sqlite3_errmsg(db));
    return SWMM_ERROR;
  }
  sqlInp = new SqliteInput();
  if(sqlInp == NULL) {
    sqlite3_close(db);
    return SWMM_ERROR;
  }
  sqlInp->db = db;
  *outInp = &(sqlInp->base);
  return SWMM_OK;
}

int swmmSqliteDescribeScenario(const char* scenario, swmm6_input* inp, swmm6_scenario_info** info)
{
  (void) scenario;
  (void) inp;
  static const char* queries[] = {
    "SELECT uid, name, invert, rim FROM JUNCTION;"
  };
  static const char* providers[] = {
    "JUNCTION"
  };
  static swmm6_scenario_info sqlInfo = {
    queries,
    providers,
    1
  };
  *info = &sqlInfo;
  return SWMM_OK;
}

int swmmSqliteReleaseScenario(swmm6_input* inp, swmm6_scenario_info* info)
{
  // no-op
  (void) inp;
  (void) info;
  return SWMM_OK;
}

int swmmSqliteOpenCursor(
  swmm6_input* inp,
  const char* query,
  const swmm6_scenario_info* info,
  swmm6_input_cursor** outCur)
{
  (void) info;
  SqliteInput* sqlInp = (SqliteInput*) inp;
  SqliteInputCursor* sqlCur;
  sqlite3_stmt* stmt;
  int rc;
  /*
  const char* sql;
  if(strcmp(kind, "JUNCTION") == 0) {
    sql = "SELECT uid, name, invert, rim FROM JUNCTION";
  } else {
    return SWMM_ABORT;
  }
  */
  rc = sqlite3_prepare_v2(sqlInp->db, query, -1, &stmt, NULL);
  if(rc) {
    puts(sqlite3_errmsg(sqlInp->db));
    return SWMM_ERROR;
  }
  sqlCur = new SqliteInputCursor();
  sqlCur->stmt = stmt;
  *outCur = (swmm6_input_cursor*) sqlCur;
  return SWMM_OK;
}

int swmmSqliteNext(swmm6_input_cursor* inpCur)
{
  SqliteInputCursor* sqlCur = (SqliteInputCursor*) inpCur;
  int rc = sqlite3_step(sqlCur->stmt);
  if(rc == SQLITE_ROW) {
    /*
    if(strcmp(kind, "JUNCTION") == 0) {
      rc = swmmSqliteReadJunction(sqlCur, (swmm6_junction_builder*) bldr);
      if (rc) {
        return rc;
      }
      return SWMM_ROW;
    }
    */
    return SWMM_ROW;
  }
  if(rc != SQLITE_DONE) {
    return SWMM_ERROR;
  }
  return SWMM_DONE;
}


int swmmSqliteReadInt(swmm6_input_cursor* cur, int col)
{
  SqliteInputCursor* sqlCur = (SqliteInputCursor*) cur;
  sqlite3_stmt* stmt = sqlCur->stmt;
  return sqlite3_column_int(stmt, col);
  /*
  const char* name = (const char*) sqlite3_column_text(stmt, 1);
  char* result = strncpy(jxnBldr->builder_base.name, name, SWMM_MAX_NAME);
  // capture result in case we want to error check?
  (void) result;
  jxnBldr->invert = sqlite3_column_double(stmt, 2);
  jxnBldr->rim = sqlite3_column_double(stmt, 3);
  return SWMM_OK;
  */
}

double swmmSqliteReadDouble(swmm6_input_cursor* cur, int col) {
  SqliteInputCursor* sqlCur = (SqliteInputCursor*) cur;
  return sqlite3_column_double(sqlCur->stmt, col);
}

const char* swmmSqliteReadText(swmm6_input_cursor* cur, int col) {
  SqliteInputCursor* sqlCur = (SqliteInputCursor*) cur;
  return (const char*) sqlite3_column_text(sqlCur->stmt, col);
}

int swmmSqliteCloseCursor(swmm6_input_cursor* cur)
{
  SqliteInputCursor* sqlCur = (SqliteInputCursor*) cur;
  int rc = sqlite3_finalize(sqlCur->stmt);
  if(rc) {
    return SWMM_ERROR;
  }
  delete sqlCur;
  return SWMM_OK;
}

int swmmSqliteCloseInput(swmm6_input* inp)
{
  SqliteInput* sqlInp = (SqliteInput*) inp;
  int rc = sqlite3_close(sqlInp->db);
  if(rc) {
    puts(sqlite3_errmsg(sqlInp->db));
    return SWMM_ERROR;
  }
  delete sqlInp;
  return SWMM_OK;
}

int swmmSqliteOpenOutput(const char* name, swmm6_output** ppOut)
{
  (void) name;
  *ppOut = NULL;
  return SWMM_ABORT;
}

int swmmSqliteCloseOutput(swmm6_output* pOut)
{
  (void) pOut;
  return SWMM_OK;
}

swmm6_io_module swmmSqliteIO = {
  1,
  "swmm-sqlite",
  swmmSqliteOpen,       // xOpen
  swmmSqliteDescribeScenario, // xDescribeScenario
  swmmSqliteOpenCursor, // xOpenCursor
  swmmSqliteNext,       // xNext
  swmmSqliteReadInt,
  swmmSqliteReadDouble,
  swmmSqliteReadText,
  swmmSqliteCloseCursor,// xCloseCursor
  swmmSqliteReleaseScenario, // xReleaseScenario
  swmmSqliteCloseInput, // xCloseInput
  swmmSqliteOpenOutput, // xOpenOutput
  swmmSqliteCloseOutput // xCloseOutput
};
