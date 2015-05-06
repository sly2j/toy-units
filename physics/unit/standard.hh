#ifndef PHYSICS_QUANTITY_STANDARD_LOADED
#define PHYSICS_QUANTITY_STANDARD_LOADED

// Geant4 HEP-units inspired unit system

#include <physics/unit.hh>
#include <physics/unit/prefix.hh>

namespace physics {
namespace standard_units {

// base unit names
constexpr const char* mm_name{"mm"};      // 1. distance
constexpr const char* ns_name{"ns"};      // 2. time
constexpr const char* MeV_name{"MeV"};    // 3. energy
constexpr const char* eplus_name{"e+"};   // 4. charge
constexpr const char* kelvin_name{"K"};   // 5. temperature
constexpr const char* mole_name{"mole"};  // 6. amount of substance
constexpr const char* candela_name{"cd"}; // 7. luminous intensity
constexpr const char* radian_name{"rad"}; // 8. angle
constexpr const char* sr_name{"sr"};      // 9. solid angle

// unit system
using standard_system =
    unit_system<mm_name, ns_name, MeV_name, eplus_name, kelvin_name, mole_name,
                candela_name, radian_name, sr_name>;

// =============================================================================
// Distance: [y-Y]m, inch, foot
// =============================================================================
namespace distance {
using dimensions = physics::make_unit_dimensions<1, 0, 0, 0, 0, 0, 0, 0, 0>;
using meter_unit = physics::unit<standard_system, dimensions, std::ratio<3>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(m, meter_unit);
using inch =
    physics::quantity<physics::unit<standard_system, dimensions, std::ratio<0>,
                                    std::ratio<0>, std::ratio<254, 10>>>;
using foot =
    physics::quantity<physics::unit<standard_system, dimensions, std::ratio<0>,
                                    std::ratio<0>, std::ratio<3048, 10>>>;
}

// =============================================================================
// Time: [y-Y]s
// =============================================================================
namespace time {
using dimensions = physics::make_unit_dimensions<0, 1, 0, 0, 0, 0, 0, 0, 0>;
using second_unit = physics::unit<standard_system, dimensions, std::ratio<9>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(s, second_unit);
}

// =============================================================================
// Energy: [y-Y]eV, [y-Y]J
// =============================================================================
namespace energy {
using dimensions = physics::make_unit_dimensions<0, 0, 1, 0, 0, 0, 0, 0, 0>;
using electronvolt_unit = physics::unit<standard_system, dimensions, std::ratio<-6>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(eV, electronvolt_unit);
using joule_unit = physics::unit<standard_system, dimensions, std::ratio<12>,
                                 std::ratio<0>, std::ratio<624150, 100000>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(J, joule_unit);
}

// =============================================================================
// Charge: elementary, [y-Y]C
// =============================================================================
namespace charge {
using dimensions = physics::make_unit_dimensions<0, 0, 0, 1, 0, 0, 0, 0, 0>;
using elementary = physics::quantity<physics::unit<standard_system, dimensions>>;
using coulomb_unit = physics::unit<standard_system, dimensions, std::ratio<18>,
                                   std::ratio<0>, std::ratio<62415 / 10000>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(C, coulomb_unit);
}
namespace temperature {
using dimensions = physics::make_unit_dimensions<0, 0, 0, 0, 1, 0, 0, 0, 0>;
using kelvin_unit = physics::unit<standard_system, dimensions, std::ratio<0>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(K, kelvin_unit);
}

// =============================================================================
// Amount: [y-Y]mol
// =============================================================================
namespace amount {
using dimensions = physics::make_unit_dimensions<0, 0, 0, 0, 0, 1, 0, 0, 0>;
using mole_unit = physics::unit<standard_system, dimensions, std::ratio<0>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(mol, mole_unit);
}

// =============================================================================
// Luminous Intensity: [y-Y]Cd
// =============================================================================
namespace luminous_intensity {
using dimensions = physics::make_unit_dimensions<0, 0, 0, 0, 0, 0, 1, 0, 0>;
using candela_unit = physics::unit<standard_system, dimensions, std::ratio<0>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(cd, candela_unit);
}

// =============================================================================
// Angle: [y-Y]rad, degree
// =============================================================================
namespace angle {
using dimensions = physics::make_unit_dimensions<0, 0, 0, 0, 0, 0, 0, 1, 0>;
using radian_unit = physics::unit<standard_system, dimensions, std::ratio<0>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(rad, radian_unit);
using degree_unit = physics::unit<standard_system, dimensions, std::ratio<0>,
                                  std::ratio<1>, std::ratio<1, 180>>;
using degree = physics::quantity<degree_unit>;
}

// =============================================================================
// Solid Angle: [y-Y]sr
// =============================================================================
namespace solid_angle {
using dimensions = physics::make_unit_dimensions<0, 0, 0, 0, 0, 0, 0, 0, 1>;
using steradian_unit = physics::unit<standard_system, dimensions, std::ratio<0>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(sr, steradian_unit);
}
// =============================================================================
// DERIVED QUANTITIES
// =============================================================================
// Cross section: [y-Y]barn
// =============================================================================
namespace cross_section {
using dimensions = physics::make_unit_dimensions<2, 0, 0, 0, 0, 0, 0, 0, 0>;
using barn_unit = physics::unit<standard_system, dimensions, std::ratio<-22>>;
PHYSICS_DEFINE_PREFIX_QUANTITIES(barn, barn_unit);
}
// =============================================================================
// Mass: [y-Y]g
// =============================================================================
namespace mass {
using gram_unit =
    decltype(energy::J{} * time::s{}.pow<2>() / distance::m{}.pow<2>())::unit;
PHYSICS_DEFINE_PREFIX_QUANTITIES(g, gram_unit);
}
// =============================================================================
// Power: [y-Y]W
// =============================================================================
namespace power {
using watt_unit = decltype(energy::J{} / time::s{})::unit; // 6.24150e+3 MeV/ns
PHYSICS_DEFINE_PREFIX_QUANTITIES(W, watt_unit);
}
// =============================================================================
// Force: [y-Y]N
// =============================================================================
namespace force {
using newton_unit =
    decltype(energy::J{} / distance::m{})::unit; // 6.24150e+9 MeV/mm
PHYSICS_DEFINE_PREFIX_QUANTITIES(N, newton_unit);
}
// =============================================================================
// Pressure: [y-Y]Pa, [y-Y]bar, [y-Y]atm
// =============================================================================
namespace pressure {
using pascal_unit = decltype(force::N{} / distance::m{}.pow<2>())::unit;
PHYSICS_DEFINE_PREFIX_QUANTITIES(Pa, pascal_unit); // 6.24150e+3 MeV/mm3
using bar_unit =
    physics::unit<standard_system, pascal_unit::dimensions,
                  std::ratio_add<pascal_unit::pow_10, std::ratio<5>>,
                  pascal_unit::pow_pi,
                  pascal_unit::factor>; // 6.24150e+8 MeV/mm3
PHYSICS_DEFINE_PREFIX_QUANTITIES(bar, bar_unit);
using atmosphere_unit =
    physics::unit<standard_system, pascal_unit::dimensions, std::ratio<8>,
                  std::ratio<0>,
                  std::ratio<632420, 100000>>; // 6.24150e+8 MeV/mm3
PHYSICS_DEFINE_PREFIX_QUANTITIES(atm, atmosphere_unit);
}
// TODO TODO complete 
// from http://geant4.web.cern.ch/geant4/reports/gallery/electromagnetic/units/SystemOfUnits.h.html
} // namespace standard_units
} // namespace physics

PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::distance, m);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::time, s);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::energy, eV);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::energy, J);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::charge, C);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::temperature, K);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::amount, mol);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::luminous_intensity, cd);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::angle, rad);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::solid_angle, sr);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::cross_section, barn);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::mass, g);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::power, W);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::force, N);
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(physics::standard_units::pressure, Pa);

// define geometrical functions of angles


#endif
