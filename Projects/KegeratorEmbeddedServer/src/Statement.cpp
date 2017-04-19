#include "Statement.h"
  
template<>
void Statement::bind< size_t >(int iLocation, const size_t &iValue)
{
  sqlite3_bind_int(mStatement, iLocation, iValue);
}

template<>
void Statement::bind< std::string >(int iLocation, const std::string &iValue)
{
  sqlite3_bind_text(mStatement, iLocation, iValue.c_str(), iValue.size(), SQLITE_TRANSIENT);
}
