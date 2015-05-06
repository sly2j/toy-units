#ifndef PHYSICS_UTIL_ASSERT_LOADED
#define PHYSICS_UTIL_ASSERT_LOADED

#include <physics/util/logger.hh>
#include <physics/util/exception.hh>
#include <string>

// throwing assert
#define tassert(condition, msg)                                                \
  if (!(condition)) {                                                          \
    physics::tassert_impl(#condition, __FILE__, __LINE__, msg);               \
  }

namespace physics {
inline void tassert_impl(const std::string& condition,
                         const std::string& location, const int line,
                         const std::string& msg) {
  LOG_ERROR(location,
            "l" + std::to_string(line) + ": assert(" + condition + ") failed");
  throw exception{msg, "assert"};
}
}

#endif
