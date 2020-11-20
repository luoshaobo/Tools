#ifndef TYPEINFO_H
#define TYPEINFO_H

#include <typeinfo>
#include "operatingsystem.h"

using namespace std;

class Lib_API TypeInfo
{
public:
  //The class type_info holds implementation-specific information about a type,
  //including the name of the type and means to compare two types for equality or collating order.
  //This is the class returned by the typeid operator.
  const type_info& m_typeInfo;

public:
  explicit TypeInfo(const type_info& info) : m_typeInfo(info) {};

  //implement a compare operator to be able comparing types easily and in readable manner
  bool operator < (const TypeInfo& rhs) const
  {
    return (m_typeInfo.before(rhs.m_typeInfo) != 0);
  }

  const char* getName() const
  {
    return m_typeInfo.name();
  }


};

#endif
