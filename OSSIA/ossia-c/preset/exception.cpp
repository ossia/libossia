#include "exception.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <cstring>

using namespace ossia;

ossiaException::ossiaException(int lineno, const std::string& filename, const std::string& details, const std::string& name) :
  m_message([&] {
  std::stringstream wh;
  wh << filename << " l." << lineno << ": Preset Exception: " << name;
  if (details.size() > 0) {
      wh << " (" << details << ")";
  }
  return wh.str();
}())
{

}

ossiaException::ossiaException(
    int lineno,
    const std::string& filename,
    const std::string& details) :
  ossiaException(lineno, filename, details, "Preset Exception"){}

const char* ossiaException::what() const noexcept
{
  return m_message.c_str();
}

ossiaException_InvalidJSON::ossiaException_InvalidJSON(int line, const std::string& filename, const std::string& details) :
  ossiaException::ossiaException(line, filename, details, "Invalid JSON"){}
ossiaException_InvalidXML::ossiaException_InvalidXML(int line, const std::string& filename, const std::string& details) :
  ossiaException::ossiaException(line, filename, details, "Invalid XML"){}
ossiaException_InvalidAddress::ossiaException_InvalidAddress(int line, const std::string& filename, const std::string& details) :
  ossiaException::ossiaException(line, filename, details, "Invalid address"){}
