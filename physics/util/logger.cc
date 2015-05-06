#include "logger.hh"

namespace physics {

void log_handler::set_level(const int level) {
  // not thread secure per default, see header for explanation
  lock_t lock{mutex_};
  if (level < 0) {
    level_ = LOG_LEVEL_NOTHING;
  } else if (level < LOG_LEVEL_NAMES.size()) {
    level_ = log_level(level);
  } else {
    level_ = log_level(LOG_LEVEL_NAMES.size() - 1);
  }
}

namespace global {
log_handler logger{};
}
}
