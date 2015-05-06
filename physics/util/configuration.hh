#ifndef PHYSICS_UTIL_CONFIGURATION_LOADED
#define PHYSICS_UTIL_CONFIGURATION_LOADED

#include <physics/util/exception.hh>
#include <physics/util/stringify.hh>

#include <string>
#include <map>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

namespace physics {

using ptree = boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

template <class T> using optional = boost::optional<T>;

template <class T> using translation_map = std::map<std::string, T>;

class configuration_error;
class configuration_path_error;
class configuration_key_error;
class configuration_value_error;
class configuration_translation_error;

// configuration handler
//
// The configuration constructor takes an identifier string
// as argument. This string should match the settings path
// in the associated settings ptree.
class configuration {
public:
  configuration(const std::string& identifier, const ptree& settings,
                const std::string& defaults_path = "defaults",
                const std::string& module_key = "module");

  // load the settings from a given ptree
  void load(const ptree& in_conf);

  // store the settings in the give ptree
  // The defaults are only exported if not yet present.
  void save(ptree& out_conf) const;

  // get the module info
  std::string module() const { return get<std::string>(module_key_); }

  // Three pairs of functions to get a setting by its key.
  //
  // In each pair, the translation_map version will lookup the
  // configuration value in the map, and throw a
  // configuration_translation_error if the lookup failed.
  //
  // 1. optional version
  template <class T> optional<T> get_optional(const std::string& key) const;
  template <class T>
  optional<T> get_optional(const std::string& key,
                           const translation_map<T>& tr) const;
  // 2. Throwing version
  template <class T> T get(const std::string& key) const;
  template <class T>
  T get(const std::string& key, const translation_map<T>& tr) const;
  // 3. default-value version
  //      if default_value is needed, it is automatically added to
  //      the default configurations for this board
  template <class T, class = typename std::enable_if<!is_map<T>::value>::type>
  T get(const std::string& key, const T& default_value);
  template <class T>
  T get(const std::string& key, const T& default_value,
        const translation_map<T>& tr);
  // same getters, but in vector version
  // 1. optional version
  template <class T>
  optional<std::vector<T>> get_optional_vector(const std::string& key) const;
  template <class T>
  optional<std::vector<T>>
  get_optional_vector(const std::string& key,
                      const translation_map<T>& tr) const;
  // 2. Throwing version
  template <class T> std::vector<T> get_vector(const std::string& key) const;
  template <class T>
  std::vector<T> get_vector(const std::string& key,
                            const translation_map<T>& tr) const;
  // special version to create bit pattern of a vector of
  // bit patterns
  // 1. optional versions
  template <class T>
  optional<T> get_optional_bitpattern(const std::string& key) const;
  template <class T>
  optional<T> get_optional_bitpattern(const std::string& key,
                                      const translation_map<T>& tr) const;
  // 2. throwing versions
  template <class T> T get_bitpattern(const std::string& key) const;
  template <class T>
  T get_bitpattern(const std::string& key, const translation_map<T>& tr) const;
  // special version to get a std::pair from a "range" vector
  // 1. optional versions
  template <class T>
  optional<std::pair<T, T>> get_optional_range(const std::string& key) const;
  template <class T>
  optional<std::pair<T, T>>
  get_optional_range(const std::string& key,
                     const translation_map<T>& tr) const;
  // 2. throwing versions
  template <class T> std::pair<T, T> get_range(const std::string& key) const;
  template <class T>
  std::pair<T, T> get_range(const std::string& key,
                            const translation_map<T>& tr) const;

  // Helper functions to construct exceptions
  configuration_path_error path_error(const std::string& path) const;
  configuration_key_error key_error(const std::string& key) const;
  configuration_value_error value_error(const std::string& key,
                                        const std::string& value) const;
  configuration_translation_error
  translation_error(const std::string& key, const std::string& value) const;
  template <class T>
  configuration_translation_error
  translation_error(const std::string& key, const std::string& value,
                    const translation_map<T>& tr) const;

private:
  template <class T>
  T translate(const std::string key, const std::string val,
              const translation_map<T>& tr) const;

  // settings
  const std::string settings_path_;
  ptree settings_;
  // defaults
  std::string defaults_path_;
  ptree defaults_;
  const std::string module_key_;
};
}

//////////////////////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////////////////////
namespace physics {
// exceptions
class configuration_error : public physics::exception {
public:
  configuration_error(const std::string& msg,
                      const std::string& type = "configuration_error")
      : physics::exception{msg, type} {}
};

class configuration_path_error : public configuration_error {
public:
  configuration_path_error(const std::string& path);
};
class configuration_key_error : public configuration_error {
public:
  configuration_key_error(const std::string& key,
                          const std::string& settings_path,
                          const std::string& defaults_path);
};

class configuration_value_error : public configuration_error {
public:
  configuration_value_error(const std::string& key, const std::string& value,
                            const std::string& settings_path,
                            const std::string& defaults_path);
};
class configuration_translation_error : public configuration_error {
public:
  configuration_translation_error(const std::string& key,
                                  const std::string& value,
                                  const std::string& settings_path,
                                  const std::string& defaults_path);
  template <class T>
  configuration_translation_error(const std::string& key,
                                  const std::string& value,
                                  const translation_map<T>& tr,
                                  const std::string& settings_path,
                                  const std::string& defaults_path);
};

// configuration getters
template <class T>
optional<T> configuration::get_optional(const std::string& key) const {
  try {
    auto s = settings_.get_optional<T>(key);
    if (!s) {
      s = defaults_.get_optional<T>(key);
    }
    return s;
  } catch (boost::property_tree::ptree_bad_data& e) {
    throw translation_error(key, e.data<std::string>());
  }
}
template <class T>
optional<T> configuration::get_optional(const std::string& key,
                                        const translation_map<T>& tr) const {
  auto s = get_optional<std::string>(key);
  if (!s) {
    return {};
  }
  return {translate(key, *s, tr)};
}
template <class T> T configuration::get(const std::string& key) const {
  auto s = get_optional<T>(key);
  if (!s) {
    throw key_error(key);
  }
  return *s;
}
template <class T>
T configuration::get(const std::string& key,
                     const translation_map<T>& tr) const {
  std::string val{get<std::string>(key)};
  return translate(key, val, tr);
}
template <class T, class>
T configuration::get(const std::string& key, const T& default_value) {
  auto s = get_optional<T>(key);
  if (!s) {
    defaults_.put(key, default_value);
    return default_value;
  }
  return *s;
}
template <class T>
T configuration::get(const std::string& key, const T& default_value,
                     const translation_map<T>& tr) {
  std::string val{get(key, default_value)};
  return translate(key, val, tr);
}
// and vector versions
template <class T>
optional<std::vector<T>>
configuration::get_optional_vector(const std::string& key) const {
  optional<std::vector<T>> vec;
  auto node = settings_.get_child_optional(key);
  if (!node) {
    node = defaults_.get_child_optional(key);
  }
  if (node) {
    vec.reset(std::vector<T>());
    for (const auto& child : *node) {
      auto val = child.second.get_value_optional<T>();
      if (val) {
        vec->push_back(*val);
      }
    }
  }
  return vec;
}
template <class T>
optional<std::vector<T>>
configuration::get_optional_vector(const std::string& key,
                                   const translation_map<T>& tr) const {
  optional<std::vector<T>> vec;
  auto vec_str = get_optional_vector<std::string>(key);
  if (vec_str) {
    vec.reset(std::vector<T>());
    for (const auto& el : *vec_str) {
      vec->push_back(translate(key, el, tr));
    }
  }
  return vec;
}
template <class T>
std::vector<T> configuration::get_vector(const std::string& key) const {
  auto s = get_optional_vector<T>(key);
  if (!s) {
    throw key_error(key);
  }
  return *s;
}
template <class T>
std::vector<T> configuration::get_vector(const std::string& key,
                                         const translation_map<T>& tr) const {
  auto s = get_optional_vector<T>(key, tr);
  if (!s) {
    throw key_error(key);
  }
  return *s;
}
// and bitpattern versiosn
template <class T>
optional<T>
configuration::get_optional_bitpattern(const std::string& key) const {
  optional<std::vector<T>> vec{get_optional_vector<T>(key)};
  optional<T> pattern;
  if (vec) {
    pattern.reset(static_cast<T>(0));
    for (const auto& val : *vec) {
      *pattern = static_cast<T>(*pattern | val);
    }
  }
  return pattern;
}
template <class T>
optional<T>
configuration::get_optional_bitpattern(const std::string& key,
                                       const translation_map<T>& tr) const {
  optional<std::vector<T>> vec{get_optional_vector(key, tr)};
  optional<T> pattern;
  if (vec) {
    pattern.reset(static_cast<T>(0));
    for (const auto& val : *vec) {
      *pattern = static_cast<T>(*pattern | val);
    }
  }
  return pattern;
}
template <class T>
T configuration::get_bitpattern(const std::string& key) const {
  auto s = get_optional_bitpattern<T>(key);
  if (!s) {
    throw key_error(key);
  }
  return *s;
}
template <class T>
T configuration::get_bitpattern(const std::string& key,
                                const translation_map<T>& tr) const {
  auto s = get_optional_bitpattern<T>(key, tr);
  if (!s) {
    throw key_error(key);
  }
  return *s;
}
// and "range" (pair) version
template <class T>
optional<std::pair<T, T>>
configuration::get_optional_range(const std::string& key) const
{
  auto range = get_optional_vector<T>(key);
  if (range) {
    if(range->size() != 2) {
      throw translation_error(key, stringify(*range));
    }
    return {{(*range)[0], (*range)[1]}};
  }
  return {};
}
template <class T>
optional<std::pair<T, T>>
configuration::get_optional_range(const std::string& key,
                                  const translation_map<T>& tr) const {
  auto range = get_optional_vector(key, tr);
  if (range) {
    if (range->size() != 2) {
      throw translation_error(key, stringify(*range));
    }
    return {{(*range)[0], (*range)[1]}};
  }
  return {};
}
template <class T>
std::pair<T, T> configuration::get_range(const std::string& key) const {
  auto range = get_optional_range<T>(key);
  if (!range) {
    throw key_error(key);
  }
  return *range;
}
template <class T>
std::pair<T, T> configuration::get_range(const std::string& key,
                                         const translation_map<T>& tr) const {
  auto range = get_optional_range(key, tr);
  if (!range) {
    throw key_error(key);
  }
  return *range;
}

    // configuration_translation_error<T> impl
template <class T>
configuration_translation_error
configuration::translation_error(const std::string& key,
                                 const std::string& value,
                                 const translation_map<T>& tr) const {
  return {key, value, tr, settings_path_, defaults_path_};
}

// "manual" translation (private)
template <class T>
T configuration::translate(const std::string key, const std::string val,
                           const translation_map<T>& tr) const {
  try {
    return tr.at(val);
  } catch (std::out_of_range) {
    throw translation_error(key, val, tr);
  }
}

// further configuration_translation_error implementation
template <class T>
configuration_translation_error::configuration_translation_error(
    const std::string& key, const std::string& value,
    const translation_map<T>& tr, const std::string& settings_path,
    const std::string& defaults_path)
    : configuration_error(
          "Unable to translate value '" + value + "' for key '" + key +
              "' (in '" + settings_path + "' or '" + defaults_path +
              "' -- allowed values: '" +
              stringify(tr, "', '",
                        [](const typename translation_map<T>::value_type& el) {
                return el.first;
              }) +
              "')",
          "configuration_translation_error") {}
}

#endif
