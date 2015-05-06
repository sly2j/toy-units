#include "configuration.hh"

#include <physics/util/logger.hh>

using namespace physics;

using boost::property_tree::ptree_bad_path;
using boost::property_tree::ptree_error;

////////////////////////////////////////////////////////////////////////////////
// class configuration
////////////////////////////////////////////////////////////////////////////////
configuration::configuration(const std::string& identifier,
                             const ptree& settings,
                             const std::string& defaults_root,
                             const std::string& module_key)
    : settings_path_{identifier}
    , defaults_path_{defaults_root}
    , module_key_{module_key} {
  load(settings);
}
// load
void configuration::load(const ptree& in_conf) {
  try {

    settings_ = in_conf.get_child(settings_path_);
    LOG_INFO(settings_path_, "Loading settings");

    auto module = settings_.get_optional<std::string>(module_key_);
    if (!module) {
      throw configuration_error("module descriptor '" + module_key_ +
                                "' has to be set in " + settings_path_);
    }
    defaults_path_ += "." + *module;
    auto def = in_conf.get_child_optional(defaults_path_);
    if (def) {
      LOG_INFO(settings_path_, *module + " defaults found.");
      defaults_ = *def;
    } else {
      LOG_INFO(settings_path_, "No default settings provided for this board.");
    }
  } catch (ptree_bad_path& e) {
    throw path_error(e.path<std::string>());
  } catch (ptree_error& e) {
    // shouldn't happen
    throw configuration_error("Processing error", e.what());
  }
}
// save
void configuration::save(ptree& out_conf) const {
  out_conf.put_child(settings_path_, settings_);
  LOG_INFO(settings_path_, "Settings saved.");
  if (!out_conf.get_child_optional(defaults_path_)) {
    out_conf.put_child(defaults_path_, defaults_);
    LOG_INFO(defaults_path_, "Settings saved.");
  }
}

configuration_path_error
configuration::path_error(const std::string& path) const {
  return {path};
}
configuration_key_error configuration::key_error(const std::string& key) const {
  return {key, settings_path_, defaults_path_};
}
configuration_value_error
configuration::value_error(const std::string& key,
                           const std::string& value) const {
  return {key, value, settings_path_, defaults_path_};
}
configuration_translation_error
configuration::translation_error(const std::string& key,
                                 const std::string& value) const {
  return {key, value, settings_path_, defaults_path_};
}

////////////////////////////////////////////////////////////////////////////////
// exceptions
////////////////////////////////////////////////////////////////////////////////
// path error
configuration_path_error::configuration_path_error(const std::string& path)
    : configuration_error{"Invalid configuration path '" + path + "'",
                          "configuration_path_error"} {}
// key error
configuration_key_error::configuration_key_error(
    const std::string& key, const std::string& settings_path,
    const std::string& defaults_path)
    : configuration_error{"Cannot find '" + key + "' (in '" + settings_path +
                              "' or '" + defaults_path + "')",
                          "configuration_key_error"} {}
// value error
configuration_value_error::configuration_value_error(
    const std::string& key, const std::string& value,
    const std::string& settings_path, const std::string& defaults_path)
    : configuration_error{"Invalid value '" + value + "' for key '" + key +
                              "' (in '" + settings_path + "' or '" +
                              defaults_path + "')",
                          "configuration_value_error"} {}

// translation error
configuration_translation_error::configuration_translation_error(
    const std::string& key, const std::string& value,
    const std::string& settings_path, const std::string& defaults_path)
    : configuration_error{"Unable to translate value '" + value +
                              "' for key '" + key + "' (in '" + settings_path +
                              "' or '" + defaults_path + "')",
                          "configuration_translation_error"} {}
