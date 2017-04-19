#pragma once

#include "sqlite/sqlite3.h"

#include <string>
#include <thread>

class Statement
{
public:
  Statement(sqlite3 *iSqlite, const char *iStatementString)
    : mStatement(nullptr)
  {
    auto wResult = sqlite3_prepare_v2(iSqlite, iStatementString, -1, &mStatement, nullptr);
    if (wResult != SQLITE_OK)
    {
      throw std::runtime_error(std::string("Cannot prepare statement: ").append(sqlite3_errstr(wResult)).c_str());
    }
  }
  Statement(Statement &&iOther)
    : mStatement(iOther.mStatement)
  {
    iOther.mStatement = nullptr;
  }
  ~Statement()
  {
    sqlite3_finalize(mStatement);
  }

  int runOnce()
  {
    int wResult = 0;
    while ((wResult = sqlite3_step(mStatement)) == SQLITE_BUSY)
    {
      std::this_thread::yield();
    }
    if (wResult == SQLITE_ERROR)
    {
      throw std::runtime_error(std::string("Statement runtime error: ").append(sqlite3_errstr(wResult)).c_str());
    }
    return wResult;
  }

  void clear()
  {
    sqlite3_reset(mStatement);
    sqlite3_clear_bindings(mStatement);
  }

  template< typename T >
  void bind(int iLocation, const T &iValue)
  {
    //static_assert(false, "Statement: cannot bind this type");
  }

  template< typename Lambda >
  void evaluate(const Lambda &iLambda)
  {
    iLambda(mStatement);
  }

private:
  sqlite3_stmt *mStatement;
};
  
template<>
void Statement::bind< size_t >(int iLocation, const size_t &iValue);

template<>
void Statement::bind< std::string >(int iLocation, const std::string &iValue);
