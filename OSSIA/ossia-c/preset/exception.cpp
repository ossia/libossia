#include "exception.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <cstring>

using namespace ossia;

ossiaException::ossiaException(int lineno, const char* filename, const char* details, const char* name) : m_lineno(lineno), m_filename(filename), m_name(name), m_details(details){}
ossiaException::ossiaException(int lineno, const char* filename, const char* details) : ossiaException(lineno, filename, details, "Preset Exception"){}

const char* ossiaException::what() const noexcept{
    std::stringstream wh;
    wh << m_filename << " l." << m_lineno << ": Preset Exception: " << m_name;
    if (strlen(m_details) > 0) {
        wh << " (" << m_details << ")";
    }
    char* cstr = new char[wh.str().size()];
    std::sprintf(cstr, "%s", wh.str().c_str());
    return cstr;
}

ossiaException_InvalidJSON::ossiaException_InvalidJSON(int line, const char* filename, const char* details) : ossiaException::ossiaException(line, filename, details, "Invalid JSON"){}
ossiaException_InvalidXML::ossiaException_InvalidXML(int line, const char* filename, const char* details) : ossiaException::ossiaException(line, filename, details, "Invalid XML"){}
ossiaException_InvalidAddress::ossiaException_InvalidAddress(int line, const char* filename, const char* details) : ossiaException::ossiaException(line, filename, details, "Invalid address"){}
