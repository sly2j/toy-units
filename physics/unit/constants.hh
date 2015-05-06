#ifndef PHYSICS_UNIT_CONSTANTS_LOADED
#define PHYSICS_UNIT_CONSTANTS_LOADED

#include <physics/unit/standard.hh>

namespace physics {
namespace standard_units {

// Physical constants
namespace constants {

// pi
constexpr double pi = physics::unit_impl::pi;

// speed of light
constexpr auto c = distance::m{299792458.} / time::s{1};
constexpr auto c2 = c * c;

// Avogadro's constant
constexpr auto NA = 6.02214179e+23 / amount::mol{1};

// Planck's constant
constexpr auto h = energy::eV{4.135667516e-15} * time::s{1};
constexpr auto hbar = energy::eV{6.58211928e-16} * time::s{1};
constexpr auto hc = energy::eV{1.23984193} * distance::um{1};
constexpr auto hbarc = energy::eV{0.19732697} * distance::um{1};

// elementary masses and resting energy
constexpr auto electron_mass_c2 = energy::MeV{0.510998910};
constexpr auto electron_mass = (electron_mass_c2 / c2);
constexpr auto proton_mass_c2 = energy::MeV{938.272013};
constexpr auto proton_mass = proton_mass_c2 / c2;
constexpr auto neutron_mass_c2 = energy::MeV{939.56536};
constexpr auto neutron_mass = neutron_mass_c2 / c2;
// amu is the atomic equivalent mass unit (u)
constexpr auto amu_c2 = energy::MeV{931.494028};
constexpr auto amu = amu_c2 / c2;

// elementary charge
constexpr auto electron_charge = charge::elementary{-1.};
constexpr auto e2 = electron_charge.pow<2>();

// permeability and permittivity of free space
constexpr auto mu0 = energy::MeV{2.01334e-16} *
                     (time::ns{1} * charge::elementary{1}).pow<2>() /
                     distance::mm{1};
constexpr auto epsilon0 = charge::elementary{5.52636e+10}.pow<2>() /
                          (energy::MeV{1} * distance::mm{1});

// fine structure constant
constexpr auto alpha = e2 / (4. * pi * epsilon0 * hbarc);

// Boltzmann
constexpr auto kB = energy::MeV{8.617343e-11} * temperature::K{1};

// TODO TODO complete from
// http://www.apc.univ-paris7.fr/~franco/g4doxy4.10/html/_physical_constants_8h_source.html
}
} // namespace standard_units
} // namespace physics

#endif
