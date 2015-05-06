#ifndef PHYSICS_UTIL_STRINGIFY_LOADED
#define PHYSICS_UTIL_STRINGIFY_LOADED

#include <physics/util/type_traits.hh>

#include <boost/lexical_cast.hpp>

#include <string>
#include <utility>
#include <type_traits>


// =============================================================================
// stringify returns a stringified version of a range. 
//
// The stringified range has its elements separated by <delimiter>. The elements
// are accesed by <acc> prior to string conversion. The default accessor is a
// NO-OP with specializations for std::pair and range types, allowing for
// stringification of complicated nested containers. Custom accessors can be
// used to stringify containers of more complicated types.
//  
// std::string physics::stringify(const Range& r,
//                                const std::string& delimiter = ", ",
//                                ElementAccessor acc);
// =============================================================================

namespace physics {
namespace stringify_impl {
// standard accessor returns el untouched
template <class Element,
          class = typename std::enable_if<!is_container<Element>::value>::type>
Element element_accessor(const Element& el);
// container accessor returns stringified version of container
template <class Container,
          class = typename std::enable_if<is_container<Container>::value>::type,
          class = void>
std::string element_accessor(const Container& c);
// pair element accessor returns a "<first>: <second>" string
template <class U, class V>
std::string element_accessor(const std::pair<U, V>& p);
}

// return stringified version of a range, with the elements
// separated by <delimiter>. The elements are accessed by <acc>
// prior to string conversion.
// Default accessor just returns the string, with specializations for
// STL containers and std::pair, allowing for nesting.
template <class Range,
          class ElementAccessor = decltype(
              stringify_impl::element_accessor<typename Range::value_type>)>
std::string
stringify(const Range& r, const std::string& delimiter = ", ",
          ElementAccessor acc =
              stringify_impl::element_accessor<typename Range::value_type>);
}

// =============================================================================
// Implementation
// =============================================================================
namespace physics {
namespace stringify_impl {
template <class Element, class> auto element_accessor(const Element& el) {
  return el;
}
template <class Container, class, class>
std::string element_accessor(const Container& c) {
  return "(" + stringify(c) + ")";
}
template <class U, class V>
std::string element_accessor(const std::pair<U, V>& p) {
  return (boost::lexical_cast<std::string>(element_accessor(p.first)) + ": " +
          boost::lexical_cast<std::string>(element_accessor(p.second)));
}
}

template <class Range, class ElementAccessor>
std::string stringify(const Range& r, const std::string& delimiter,
                      ElementAccessor acc) {
  std::string str{};
  for (auto& el : r) {
    if (!str.empty()) {
      str += delimiter;
    }
    str += boost::lexical_cast<std::string>(acc(el));
  }
  return str;
}
}

#endif
