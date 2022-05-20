#include "input.h"

#include <assert.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SWMM_MAX_IO 5

int nIO = 1;

extern swmm6_io_module swmmSqliteIO;

swmm6_io_module* vIO[SWMM_MAX_IO] = {
  &swmmSqliteIO
};

swmm6_io_module* defaultIO = &swmmSqliteIO;

int swmm6_register_io_module(swmm6_io_module* pMod, int makeDefault)
{
  if(nIO >= SWMM_MAX_IO) {
    return SWMM_ABORT;
  }
  vIO[nIO] = pMod;
  nIO++;
  if(makeDefault) {
    defaultIO = pMod;
  }
  return SWMM_OK;
}

swmm6_io_module* swmm6_find_io_module(const char* sName)
{
  if(sName == NULL) {
    return defaultIO;
  }
  int index;
  for(index = 0 ; index < nIO ; index++) {
    assert(index < SWMM_MAX_IO);
    if(strcmp(vIO[index]->sName, sName) == 0) {
      return vIO[index];
    }
  }
  return NULL;
}

int inputOpen(const char* sInpName, const char* sModule, swmm6_input** outInput)
{
  swmm6_io_module* pIO = swmm6_find_io_module(sModule);
  swmm6_input* pInp;
  int rc = pIO->xOpenInput(sInpName, &pInp);
  if(rc) {
    *outInput = NULL;
    return rc;
  }
  pInp->io_methods = pIO;
  pInp->sName = strdup(sInpName);
  if(pInp->sName == NULL) {
    inputClose(pInp);
    return SWMM_NOMEM;
  }
  *outInput = pInp;
  return SWMM_OK;
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

int inputOpenCursor(swmm6_input* inp, const char* query, swmm6_provider* prv, swmm6_input_cursor** outCursor)
{
  VIRTUAL_CHECK(inp->io_methods, xOpenCursor)
  int rc = inp->io_methods->xOpenCursor(inp, query, prv, outCursor);
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
  free(inp->sName);
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
  sqlInp = malloc(sizeof(*sqlInp));
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
  static char* queries[] = {
    "SELECT uid, name, invert, rim FROM JUNCTION;"
  };
  static char* providers[] = {
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
  swmm6_provider* prv,
  swmm6_input_cursor** outCur)
{
  (void) prv;
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
  sqlCur = malloc(sizeof(*sqlCur));
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
  free(sqlCur);
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
  free(sqlInp);
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
