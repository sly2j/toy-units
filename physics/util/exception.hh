#ifndef PHYSICS_UTIL_EXCEPTION_LOADED
#define PHYSICS_UTIL_EXCEPTION_LOADED

#include <exception>
#include <string>

namespace physics {

class exception : public std::exception {
public:
  exception(const std::string& msg, const std::string& type = "exception")
      : msg_{msg}, type_{type} {}

  virtual const char* what() const throw() { return msg_.c_str(); }
  virtual const char* type() const throw() { return type_.c_str(); }
  virtual ~exception() throw() {}

private:
  std::string msg_;
  std::string type_;
};
}

#endif
