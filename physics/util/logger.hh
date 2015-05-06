#ifndef UTIL_LOGGER_LOADED
#define UTIL_LOGGER_LOADED

#include <ctime>
#include <string>
#include <ostream>
#include <iostream>
#include <vector>
#include <mutex>

// let's move this out of the namespaces for convenience sake
enum log_level {
  LOG_LEVEL_NOTHING,
  LOG_LEVEL_CRITICAL,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_JUNK,
  LOG_LEVEL_JUNK2
};
const std::vector<std::string> LOG_LEVEL_NAMES{"nothing", "critical", "error",
                                               "warning", "info",     "debug",
                                               "junk",    "junk2"};

namespace physics {

// log handler class designed for global usage,
// threading secure
class log_handler {
private:
  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;

public:
  log_handler(const log_level level = LOG_LEVEL_INFO,
              std::ostream& sink = std::cout)
      : level_{level}, sink_(sink) {}

  inline log_level level() const {
    lock_t lock{mutex_};
    return level_;
  }
  void set_level(const int level);
  inline void operator()(const log_level mlevel, const std::string& mtitle,
                         const std::string& mtext) {
    lock_t lock{mutex_};
    if (mlevel > level_)
      return;
    time_t rt;
    time(&rt);
    sink_ << "[" << rt << ", " << mtitle << ", " << LOG_LEVEL_NAMES.at(mlevel)
          << "] " << mtext << std::endl;
  }

private:
  log_level level_;
  std::ostream& sink_;
  mutable mutex_t mutex_;
};
namespace global {
extern log_handler logger;
}

template <log_level level, class Logger = log_handler>
inline void log(const std::string& mtitle, const std::string& mtext,
                Logger& logger = global::logger) {
  logger(level, mtitle, mtext);
}
}

// use preprocessor here to delay the evaluation of mtitle and mtext
// to after the log level check, this is significantly faster than calling
// log<LEVEL>(mtitle, mtext) directly in the code, because the compiler isn't
// allowed to this for us. So, don't use any assignemts in the arguments, they
// won't always work ;-)

#define LOG_CRITICAL(mtitle, mtext)                                            \
  if (physics::global::logger.level() >= LOG_LEVEL_CRITICAL) {                 \
    physics::log<LOG_LEVEL_CRITICAL>((mtitle), (mtext));                       \
  }
#define LOG_ERROR(mtitle, mtext)                                               \
  if (physics::global::logger.level() >= LOG_LEVEL_ERROR) {                    \
    physics::log<LOG_LEVEL_ERROR>((mtitle), (mtext));                          \
  }
#define LOG_WARNING(mtitle, mtext)                                             \
  if (physics::global::logger.level() >= LOG_LEVEL_WARNING) {                  \
    physics::log<LOG_LEVEL_WARNING>((mtitle), (mtext));                        \
  }
#define LOG_INFO(mtitle, mtext)                                                \
  if (physics::global::logger.level() >= LOG_LEVEL_INFO) {                     \
    physics::log<LOG_LEVEL_INFO>((mtitle), (mtext));                           \
  }
#define LOG_DEBUG(mtitle, mtext)                                               \
  if (physics::global::logger.level() >= LOG_LEVEL_DEBUG) {                    \
    physics::log<LOG_LEVEL_DEBUG>((mtitle), (mtext));                          \
  }
#define LOG_JUNK(mtitle, mtext)                                                \
  if (physics::global::logger.level() >= LOG_LEVEL_JUNK) {                     \
    physics::log<LOG_LEVEL_JUNK>((mtitle), (mtext));                           \
  }
#define LOG_JUNK2(mtitle, mtext)                                               \
  if (physics::global::logger.level() >= LOG_LEVEL_JUNK2) {                    \
    physics::log<LOG_LEVEL_JUNK2>((mtitle), (mtext));                          \
  }

#endif
