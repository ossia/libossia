#pragma once

#include <exception>

namespace ossia {

class ossiaException : public std::exception
{
protected:
    const int m_lineno;
    const char* m_filename;
    const char* m_name;
    const char* m_details;
    ossiaException(int lineno, const char *filename, const char* details, const char* name);
public:
    ossiaException(int lineno, const char *filename, const char* details = "");
    const char* what () const noexcept;
};

class ossiaException_InvalidJSON : public ossiaException {
public:
    ossiaException_InvalidJSON(int line, const char* filename, const char* details = "");
};

class ossiaException_InvalidXML : public ossiaException {
public:
    ossiaException_InvalidXML(int line, const char* filename, const char* details = "");
};

class ossiaException_InvalidAddress : public ossiaException {
public:
    ossiaException_InvalidAddress(int line, const char *filename, const char* details = "");
};

}
