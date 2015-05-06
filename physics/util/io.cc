#include "io.hh"
#include <boost/filesystem.hpp>

#include <physics/util/logger.hh>

namespace physics {
//////////////////////////////////////////////////////////////////////////////////////////
// make_filename
//////////////////////////////////////////////////////////////////////////////////////////
std::string make_filename(const std::string& dir, const std::string& base,
                          const std::string& extra) {
  std::string ret{dir};
  if (!dir.empty() && dir.back() != '/') {
    ret += "/";
  }
  ret += base;
  if (!extra.empty()) {
    ret += "." + extra;
  }
  return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////////////////////////
io_error::io_error(const std::string& msg, const std::string& type)
    : exception{msg, type} {}

io_read_error::io_read_error(const std::string& msg, const std::string& type)
    : io_error{msg, type} {}

io_write_error::io_write_error(const std::string& msg, const std::string& type)
    : io_error{msg, type} {}

//////////////////////////////////////////////////////////////////////////////////////////
// directory proxies
//////////////////////////////////////////////////////////////////////////////////////////
output_directory::output_directory(const std::string& path_name, bool force_new)
    : path{path_name} {
  if (!boost::filesystem::exists(path)) {
    LOG_INFO("I/O", "Creating output directory '" + path + "'");
    boost::filesystem::create_directories(path);
  } else if (force_new) {
    throw io_write_error{"Failed to create directory '" + path + "'."};
  } else if (!boost::filesystem::is_directory(path)) {
    throw io_write_error{"Not a directory: '" + path + "'."};
  }
}
output_directory::output_directory(const configuration& conf,
                                   const std::string& key, bool force_new)
    : output_directory(conf.get<std::string>(key), force_new) {}

input_directory::input_directory(const std::string& path_name)
    : path{path_name} {
  if (!boost::filesystem::exists(path)) {
    throw io_read_error{"No such file or directory: '" + path + "'."};
  } else if (!boost::filesystem::is_directory(path)) {
    throw io_read_error{"Not a directory: '" + path + "'."};
  }
}
input_directory::input_directory(const configuration& conf,
                                 const std::string& key)
    : input_directory(conf.get<std::string>(key)) {}

} // namespace physics
