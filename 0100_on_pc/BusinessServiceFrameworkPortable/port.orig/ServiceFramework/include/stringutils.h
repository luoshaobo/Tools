
#ifndef STRING_UTILS_H__
#define STRING_UTILS_H__

#include <string>
#include <vector>
#include "operatingsystem.h"

class Lib_API StringUtils
{
  public:

  static std::string trim(const std::string& s);
  static std::string trimLeft(const std::string& s);
  static std::string trimRight(const std::string& s);
  static std::string toUpper(std::string& s);
  static std::string toLower(std::string& s);
  static std::vector<std::string> tokenize(const std::string& s, const std::string& delimiters);
  static const std::string WHITESPACE;

  private:
  StringUtils();
};

#endif //STRING_UTILS_H__
