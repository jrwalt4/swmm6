#include "input.hh"

#include "error.hh"

#include <assert.h>
#include <sqlite3.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unordered_map>
#include <vector>

using namespace std;

namespace swmm
{

class Swmm6InputObjectCursor: public InputObjectCursor
{
  sqlite3_stmt* _stmt;
public:
  Swmm6InputObjectCursor(sqlite3_stmt* stmt): _stmt(stmt) {}

  ~Swmm6InputObjectCursor()
  {
    sqlite3_finalize(_stmt);
  }

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

class Swmm6InputScenarioCursor: public InputScenarioCursor
{
  string _queries;
  const char* _next_query;
  sqlite3* _db;

public:
  Swmm6InputScenarioCursor(sqlite3* db, const char* scenario): _db(db)
  {
    (void) scenario;
    _queries = "SELECT * FROM JUNCTIONS; SELECT * FROM CONDUITS;";
    _next_query = _queries.c_str();
  }

  bool next() override
  {
    // check is `_next_query` pointer is at the end of `_queries`
    if((_next_query - _queries.c_str()) == _queries.length()) {
      return false;
    }
    return true;
  }

  Swmm6InputObjectCursor* openObjectCursor() override
  {
    sqlite3_stmt* stmt;
    // sqlite3 will set `_next_query` to beginning of next statement, or
    // end of `_queries` if there are no more.
    int rc = sqlite3_prepare_v2(_db, _next_query, -1, &stmt, &_next_query);
    if(rc) {
      throw IoError(sqlite3_errmsg(_db));
    }
    return new Swmm6InputObjectCursor(stmt);
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

  Swmm6InputScenarioCursor* openScenario(const char* scenario) override
  {
    return new Swmm6InputScenarioCursor(_db, scenario);
  }
};

class ExtensionInputObjectCursor: public InputObjectCursor
{
  swmm6_input_cursor* _cursor;
  swmm6_io_module* _methods;
public:
  ExtensionInputObjectCursor(swmm6_input_cursor* cursor): _cursor(cursor), _methods(cursor->pInp->io_methods) {}
  ~ExtensionInputObjectCursor()
  {
    _methods->xCloseCursor(_cursor);
  }
  bool next() override
  {
    return _methods->xCursorNext(_cursor);
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

class ExtensionInputScenarioCursor: public InputScenarioCursor
{
  swmm6_scenario_cursor* _scn_cursor;
  swmm6_io_module* _methods;

public:
  ExtensionInputScenarioCursor(swmm6_scenario_cursor* scn_cursor):
    _scn_cursor(scn_cursor),
    _methods(scn_cursor->pInp->io_methods) {}

  bool next() override
  {
    int rc = _methods->xScenarioNext(_scn_cursor);
    switch (rc) {
      case SWMM_NEXT: return true;
      case SWMM_DONE: return false;
      default: throw IoError("Scenario::next", rc);
    }
  }

  ExtensionInputObjectCursor* openObjectCursor() override
  {
    swmm6_input_cursor* cursor;
    int rc = _methods->xOpenCursor(_scn_cursor, &cursor);
    if(rc) {
      throw IoError("Error opening cursor", rc);
    }
    return new ExtensionInputObjectCursor(cursor);
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

  ExtensionInputScenarioCursor* openScenario(const char* scenario) override
  {
    swmm6_scenario_cursor* scn_cursor;
    int rc = _methods->xOpenScenario(scenario, _input, &scn_cursor);
    if(rc) {
      throw IoError("Error opening scneario: " + string{scenario}, rc);
    }
    return new ExtensionInputScenarioCursor(scn_cursor);
  }
};

Input* Input::open(const char* sInpName, const swmm6_io_module* input_module)
{
  if(input_module == NULL) {
    return new Swmm6Input(sInpName);
  }
  swmm6_input* pInp;
  int rc = input_module->xOpenInput(sInpName, &pInp);
  if(rc) {
    throw IoError("Unable to open", rc);
  }
  return new ExtensionInput(pInp);
}

swmm6_input_cursor* InputObjectCursor::as_extension()
{
  return reinterpret_cast<swmm6_input_cursor*>(this);
}

template<>
int InputObjectCursor::read(int col)
{
  return readInt(col);
}

template<>
double InputObjectCursor::read(int col)
{
  return readDouble(col);
}

template<>
const std::string InputObjectCursor::read(int col)
{
  return readText(col);
}

} // namespace swmm
