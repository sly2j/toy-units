#ifndef PHYSICS_UNIT_TYPE_TRAITS_LOADED
#define PHYSICS_UNIT_TYPE_TRAITS_LOADED

#include <ratio>
#include <type_traits>

#include <physics/unit/prototype.hh>

// type traits to help with compile-time type checking when defining
// units/quantities

namespace physics {
namespace unit_impl {

// type checking
template <class T> struct is_unit_system;
template <class T> struct is_unit_dimensions;
template <class T> struct is_unit;
template <class T> struct is_quantity;

// test unit compatibility (same system and dimensions)
template <class U1, class U2> struct are_compatible;
template <class U1, class U2> struct have_same_system;

// test for valid std::ratios
template <class... R> struct are_valid_ratios;

} // namespace unit_impl
} // namespace physics

////////////////////////////////////////////////////////////////////////////////
// implementation: type checking
////////////////////////////////////////////////////////////////////////////////
namespace physics {
namespace unit_impl {
// is_unit_system
template <class T> struct is_unit_system : std::false_type {};
template <const char* const&... Names>
struct is_unit_system<unit_system<Names...>> : std::true_type {};

// is_unit_dimensions
template <class T> struct is_unit_dimensions : std::false_type {};
template <class... Dimensions>
struct is_unit_dimensions<unit_dimensions<Dimensions...>> : std::true_type {};

// is_unit
template <class T> struct is_unit : std::false_type {};
template <class System, class Dimensions, class Pow10, class PowPi,
          class Factor>
struct is_unit<unit<System, Dimensions, Pow10, PowPi, Factor>>
    : std::true_type {};

// is_quantity
template <class T> struct is_quantity : std::false_type {};
template <class Unit> struct is_quantity<quantity<Unit>> : std::true_type {};

} // namespace unit_impl
} // namespace physics
////////////////////////////////////////////////////////////////////////////////
// implementation: unit compatibility
////////////////////////////////////////////////////////////////////////////////
namespace physics {
namespace unit_impl {
  // are_compatible
  template <class U1, class U2> struct are_compatible : std::false_type {};
  template <class System, class Dimensions, class U1Pow10, class U2Pow10,
            class U1PowPi, class U2PowPi, class U1Factor, class U2Factor>
  struct are_compatible<unit<System, Dimensions, U1Pow10, U1PowPi, U1Factor>,
                        unit<System, Dimensions, U2Pow10, U2PowPi, U2Factor>>
      : std::true_type {};

  // have_same_system
  template <class U1, class U2> struct have_same_system : std::false_type {};
  template <class System, class U1Dimensions, class U2Dimensions, class U1Pow10,
            class U2Pow10, class U1PowPi, class U2PowPi, class U1Factor,
            class U2Factor>
  struct have_same_system<
      unit<System, U1Dimensions, U1Pow10, U1PowPi, U1Factor>,
      unit<System, U2Dimensions, U2Pow10, U2PowPi, U2Factor>> : std::true_type {
  };
} // namespace unit_impl
} // namespace physics
////////////////////////////////////////////////////////////////////////////////
// implementation: are_valid_ratios
////////////////////////////////////////////////////////////////////////////////
namespace physics {
namespace unit_impl {
template <class... T> constexpr std::false_type are_valid_ratios_impl(T...) {
  return {};
}
template <std::intmax_t... N, std::intmax_t... D>
constexpr std::true_type are_valid_ratios_impl(std::ratio<N, D>...) {
  return {};
}
template <class... T> struct are_valid_ratios 
  : decltype(are_valid_ratios_impl(T{}...)) {};

} // namespace unit_impl
} // namespace physics

#endif
