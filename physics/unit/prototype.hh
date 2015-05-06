#ifndef PHYSICS_UNIT_PROTOTYPE_LOADED
#define PHYSICS_UNIT_PROTOTYPE_LOADED

// prototypes for unit.hh

namespace physics {
template <const char* const&... Names> struct unit_system;
template <class... Dimensions> struct unit_dimensions;
template <class System, class Dimensions, class Pow10, class PowPi,
          class Factor> struct unit;
template <class Unit> class quantity;
}

#endif
