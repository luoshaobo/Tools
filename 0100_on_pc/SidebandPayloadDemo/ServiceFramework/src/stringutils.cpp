#include "stringutils.h"
#include <algorithm>
#include <sstream>

const std::string StringUtils::WHITESPACE = " \n\r\t";

std::string StringUtils::trim(const std::string& s)
{
  return trimRight(trimLeft(s));
}

std::string StringUtils::trimLeft(const std::string& s)
{
  std::string ret;
  size_t startpos = s.find_first_not_of(StringUtils::WHITESPACE);

  if (startpos != std::string::npos)
  {
    ret = s.substr(startpos);
  }
  return ret;
}

std::string StringUtils::trimRight(const std::string& s)
{
  std::string ret;
  size_t endpos = s.find_last_not_of(StringUtils::WHITESPACE);

  if (endpos != std::string::npos)
  {
    ret = s.substr(0, endpos + 1);
  }
  return ret;
}
std::string StringUtils::toUpper(std::string& s)
{
  transform(s.begin(), s.end(), s.begin(), toupper);
  return s;
}
std::string StringUtils::toLower(std::string& s)
{
  transform(s.begin(), s.end(), s.begin(), tolower);
  return s;
}

std::vector<std::string> StringUtils::tokenize(const std::string& s, const std::string& delimiters)
{
  std::string input = s;
  std::vector<std::string> res;

  while (input.empty() == false)
  {
    //find delimiter
    std::string::size_type  dPos = input.find_first_of(delimiters);

    //check if it starts with the delimiter
    if (dPos == 0)
    {
      //remove delimiter and add the empty entry
      input = input.substr(1);
      res.push_back("");
    }
    else
    {
      //cut the entry and add it
      std::string entry = input.substr(0, dPos);
      res.push_back(entry);

      //check dPos for end
      if (dPos == std::string::npos)
      {
        input.clear();
      }
      else
      {
        //remove delimiter
        input = input.substr(dPos + 1);
      }
    }
  }

  return res;
}
