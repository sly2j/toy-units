#ifndef PHYSICS_UNIT_DETAIL_LOADED
#define PHYSICS_UNIT_DETAIL_LOADED

// misc utility functions

#include <ratio>

#include <physics/unit/prototype.hh>

// helper macro to define a scaled quantity with name NAME relative to the base
// unit BASE_UNIT. The new quantity will be in a unit scaled wrt BASE_UNIT with
// a factor of 10^SCALE.
#define PHYSICS_DEFINE_SCALED_QUANTITY(NAME, SCALE, BASE_UNIT)                 \
  using NAME = physics::quantity<physics::unit<                                \
      typename BASE_UNIT::system, typename BASE_UNIT::dimensions,              \
      std::ratio_add<typename BASE_UNIT::pow_10, std::ratio<SCALE>>,           \
      typename BASE_UNIT::pow_pi, typename BASE_UNIT::factor>>;

namespace physics {
namespace unit_impl {

// make a dimensionless unit_dimensions type
template <std::size_t NDim, class... Dims>
struct make_dimensionless
    : make_dimensionless<NDim - 1, std::ratio<0>, Dims...> {};
template <class... Dims> struct make_dimensionless<0, Dims...> {
  using type = unit_dimensions<Dims...>;
};
}
}

#endif
