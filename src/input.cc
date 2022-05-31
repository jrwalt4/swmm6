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

class Swmm6InputObjectReader: public InputObjectReader
{
  sqlite3_stmt* _stmt;
  ParamDefPack& _params;
public:
  Swmm6InputObjectReader(sqlite3_stmt* stmt, ParamDefPack& params):
    _stmt(stmt), _params(params) {}

  ~Swmm6InputObjectReader()
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

  int readParams(ParamPack& values) override
  {
    bool success;
    int ncols = sqlite3_column_count(_stmt);
    assert(values.length() == ncols);
    for(int i = 0 ; i < ncols ; i++) {
      switch(_params[i].param_type) {
        case swmm6_param_type::INT:
          success = values.set_int(sqlite3_column_int(_stmt, i), i);
          break;
        case swmm6_param_type::REAL:
          success = values.set_real(sqlite3_column_double(_stmt, i), i);
          break;
        case swmm6_param_type::TEXT:
          success = values.set_text((const char*)sqlite3_column_text(_stmt, i), i);
          break;
        case swmm6_param_type::UID:
          success = values.set_int(sqlite3_column_int(_stmt, i), i);
          break;
      }
      if (!success) {
        return SWMM_ERROR;
      }
    }
    return SWMM_OK;
  }
};

class Swmm6InputCursor: public InputCursor
{
  sqlite3_stmt* _stmt;

public:
  Swmm6InputCursor(sqlite3* db, swmm6_object_type obj_type, const char* scenario)
  {
    (void) scenario;
    const char* _query;
    switch(obj_type) {
      case swmm6_object_type::NODE:
        _query = "SELECT uid, name, kind FROM JUNCTIONS;";
      case swmm6_object_type::LINK:
        _query = "SELECT uid, name, kind FROM CONDUITS";
    }
    int rc = sqlite3_prepare_v2(db, _query, -1, &_stmt, NULL);
    if(rc) {
      throw IoError("Error opening db", rc);
    }
  }

  std::pair<bool, InputObjectConstructorProps> next() override
  {
    InputObjectConstructorProps props;
    int rc = sqlite3_step(_stmt);
    if(rc == SQLITE_DONE) {
      props.uid = sqlite3_column_int(_stmt, 0);
      props.name = (const char*)sqlite3_column_text(_stmt, 1);
      props.kind = (const char*)sqlite3_column_text(_stmt, 2);
      return make_pair(true, std::move(props));
    }
    return make_pair(false, std::move(props));
  }

  ~Swmm6InputCursor()
  {
    sqlite3_finalize(_stmt);
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

  Swmm6InputCursor* openNodeCursor(const char* scenario) override
  {
    return new Swmm6InputCursor(_db, swmm6_object_type::NODE, scenario);
  }


  Swmm6InputObjectReader* openReader(const char* kind, ParamDefPack& params) override
  {
    sqlite3_stmt* stmt;
    string query = "SELECT uid ";
    for(auto& param : params) {
      query += ", " + param.param_name;
    }
    query += " FROM " + string{kind};
    int rc = sqlite3_prepare_v2(_db, query.c_str(), -1, &stmt, NULL);
    if(rc) {
      throw IoError(sqlite3_errmsg(_db));
    }
    return new Swmm6InputObjectReader(stmt, params);
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
  throw NotImplementedError("ExtensionInput");
  //return new ExtensionInput(pInp);
}

} // namespace swmm
