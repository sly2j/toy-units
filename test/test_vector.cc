#include <iostream>

#define BOOST_TEST_MODULE test_vector
#include <boost/test/unit_test.hpp>

#include "physics/unit.hh"

#include <cmath>

constexpr const char* mm_name{"mm"};
constexpr const char* ns_name{"ns"};
constexpr const char* MeV_name{"MeV"};
constexpr const char* rad_name{"rad"};

namespace quantities {
using sys_type = physics::unit_system<mm_name, ns_name, MeV_name, rad_name>;
// angles
using angle_type = physics::unit_dimensions<std::ratio<0>, std::ratio<0>,
                                            std::ratio<0>, std::ratio<1>>;
using radian_type = physics::unit<sys_type, angle_type>;
using angle_radian_type = physics::quantity<radian_type>;
// velocities
using velocity_type = physics::unit_dimensions<std::ratio<1>, std::ratio<-1>,
                                               std::ratio<0>, std::ratio<0>>;
using mm_per_ns_type = physics::unit<sys_type, velocity_type>;
using m_per_ns_type = physics::unit<sys_type, velocity_type, std::ratio<3>>;
using velocity_mm_per_ns_type = physics::quantity<mm_per_ns_type>;
using velocity_m_per_ns_type = physics::quantity<m_per_ns_type>;
// energies
using energy_type = physics::unit_dimensions<std::ratio<0>, std::ratio<0>,
                                             std::ratio<1>, std::ratio<0>>;
using MeV_type = physics::unit<sys_type, energy_type>;
using energy_MeV_type = physics::quantity<MeV_type>;
}
double cos(quantities::angle_radian_type phi) { return std::cos(phi.value()); }
double sin(quantities::angle_radian_type phi) { return std::sin(phi.value()); }

#include "physics/vector.hh"
namespace quantities {
using velocity_vector =
    physics::vector<velocity_mm_per_ns_type, angle_radian_type>;
using velocity2_vector =
    physics::vector<velocity_m_per_ns_type, angle_radian_type>;
using energy_vector = physics::vector<energy_MeV_type, angle_radian_type>;
using velocity_lorentzvector =
    physics::lorentzvector<velocity_mm_per_ns_type, angle_radian_type>;
using velocity2_lorentzvector =
    physics::lorentzvector<velocity_m_per_ns_type, angle_radian_type>;
using energy_lorentzvector =
    physics::lorentzvector<energy_MeV_type, angle_radian_type>;
}

// 3-vector of raw doubles
BOOST_AUTO_TEST_CASE(vector_double) {
  using vector_type = physics::vector<double>;

  // construction and assignment
  {
    // vector::vector()
    constexpr vector_type v1;
    BOOST_CHECK((v1.x1 == 0 && v1.x2 == 0 && v1.x3 == 0));
    // vector::vector(double)
    constexpr vector_type v2{3.14};
    BOOST_CHECK((v2.x1 == 3.14 && v2.x2 == 3.14 && v2.x3 == 3.14));
    // vector::vector(double, double, double)
    constexpr vector_type v3{1., 2., 3.};
    BOOST_CHECK((v3.x1 == 1 && v3.x2 == 2 && v3.x3 == 3));
    // vector::vector(vector)
    constexpr vector_type v4{v3};
    BOOST_CHECK((v4.x1 == 1 && v4.x2 == 2 && v4.x3 == 3));
    // vector& vector::operator=(vector)
    {
      vector_type v5;
      v5 = v4;
      BOOST_CHECK((v5.x1 == v4.x1 && v5.x2 == v4.x2 && v5.x3 == v4.x3));
      // operator= returns reference
      (v5 = v3) = v2;
      BOOST_CHECK((v5.x1 == v2.x1 && v5.x2 == v2.x2 && v5.x3 == v2.x3));
    }
  }
  // comparison
  {
    constexpr vector_type v1{1., 0, 0};
    constexpr vector_type v2{1, 1, 2};
    BOOST_CHECK((v1 == v1));
    BOOST_CHECK((v1 != v2));
  }
  // vector arithmetic
  {
    constexpr vector_type v1{1., 2., 3.};
    constexpr vector_type v2{2.1, 3.2, 4.3};
    // vector::operator+(vector)
    // vector::operator-(vector)
    {
      constexpr vector_type va{v1 + v2};
      constexpr vector_type vs{v1 - v2};
      BOOST_CHECK((va.x1 == v1.x1 + v2.x1 && va.x2 == v1.x2 + v2.x2 &&
                   va.x3 == v1.x3 + v2.x3));
      BOOST_CHECK((vs.x1 == v1.x1 - v2.x1 && vs.x2 == v1.x2 - v2.x2 &&
                   vs.x3 == v1.x3 - v2.x3));
    }
    // vector::operator*(double)
    // vector::operator/(double)
    // operator*(double, vector)
    // vector::operator*(vector<double>) (scalar)
    // vector::operator^(vector<double>) (cross)
    {
      constexpr double d{8.1};
      constexpr vector_type vm{v1 * d};
      constexpr vector_type vd{v1 / d};
      BOOST_CHECK(
          (vm.x1 == v1.x1 * d && vm.x2 == v1.x2 * d && vm.x3 == v1.x3 * d));
      BOOST_CHECK(
          (vd.x1 == v1.x1 / d && vd.x2 == v1.x2 / d && vd.x3 == v1.x3 / d));
      constexpr vector_type vm2{d * vm};
      BOOST_CHECK(
          (vm2.x1 == vm.x1 * d && vm2.x2 == vm.x2 * d && vm2.x3 == vm.x3 * d));
      constexpr double scalar = v1 * v2;
      BOOST_CHECK((scalar == v1.x1 * v2.x1 + v1.x2 * v2.x2 + v1.x3 * v2.x3));
      constexpr vector_type vc{v1 ^ v2};
      BOOST_CHECK((vc.x1 == v1.x2 * v2.x3 - v1.x3 * v2.x2 &&
                   vc.x2 == v1.x3 * v2.x1 - v1.x1 * v2.x3 &&
                   vc.x3 == v1.x1 * v2.x2 - v1.x2 * v2.x1));
    }
    // a 3-vector of raw doubles can be multiplied/divided by a dimensionful
    // quantity
    // vector::operator*(Quantity)
    // vector::operator/(Quantity)
    // operator*(Quantity, vector)
    // vector::operator*(vector<Quantity) (scalar)
    {
      using sys_type = physics::unit_system<mm_name, ns_name, MeV_name>;
      using distance_type =
          physics::unit_dimensions<std::ratio<1>, std::ratio<0>, std::ratio<0>>;
      using mm_type = physics::unit<sys_type, distance_type, std::ratio<0>,
                                    std::ratio<0>, std::ratio<1>>;
      using distance_mm_type = physics::quantity<mm_type>;
      using distance_vector = physics::vector<distance_mm_type>;
      constexpr distance_mm_type distance{42.};
      constexpr distance_vector vdistance{distance};
      constexpr auto vm = v1 * distance;
      constexpr auto vd = v1 / distance;
      BOOST_CHECK((physics::unit_string(vm) == " mm" &&
                   physics::unit_string(vd) == " mm^-1"));
      BOOST_CHECK((vm.x1.value() == v1.x1 * distance.value() &&
                   vm.x2.value() == v1.x2 * distance.value() &&
                   vm.x3.value() == v1.x3 * distance.value()));
      BOOST_CHECK((vd.x1.value() == v1.x1 / distance.value() &&
                   vd.x2.value() == v1.x2 / distance.value() &&
                   vd.x3.value() == v1.x3 / distance.value()));
      constexpr auto vm2 = distance * v2;
      BOOST_CHECK((physics::unit_string(vm2) == " mm"));
      BOOST_CHECK((vm2.x1.value() == v2.x1 * distance.value() &&
                   vm2.x2.value() == v2.x2 * distance.value() &&
                   vm2.x3.value() == v2.x3 * distance.value()));
      constexpr auto scalar = v1 * vdistance;
      BOOST_CHECK((scalar.value() ==
                   v1.x1 * vdistance.x1.value() + v1.x2 * vdistance.x2.value() +
                       v1.x3 * vdistance.x3.value()));
      BOOST_CHECK(
          (scalar ==
           v1.x1 * vdistance.x1 + v1.x2 * vdistance.x2 + v1.x3 * vdistance.x3));
      BOOST_CHECK((unit_string(scalar) == " mm"));
      constexpr auto vc = v1 ^ vdistance;
      BOOST_CHECK((vc.x1 == v1.x2 * vdistance.x3 - v1.x3 * vdistance.x2 &&
                   vc.x2 == v1.x3 * vdistance.x1 - v1.x1 * vdistance.x3 &&
                   vc.x3 == v1.x1 * vdistance.x2 - v1.x2 * vdistance.x1));
      BOOST_CHECK((unit_string(vc) == " mm"));
    }
  }
  // arithmetic assigment
  {
    constexpr vector_type v1{1., 2., 3.};
    constexpr vector_type v2{2.1, 3.2, 4.3};
    // vector::operator+=(vector)
    // vector::operator-=(vector)
    {
      vector_type va{v1};
      vector_type vs{v1};
      va += v2;
      vs -= v2;
      BOOST_CHECK((va.x1 == v1.x1 + v2.x1 && va.x2 == v1.x2 + v2.x2 &&
                   va.x3 == v1.x3 + v2.x3));
      BOOST_CHECK((vs.x1 == v1.x1 - v2.x1 && vs.x2 == v1.x2 - v2.x2 &&
                   vs.x3 == v1.x3 - v2.x3));
    }
    // vector::operator*=(double)
    // vector::operator/=(double)
    {
      constexpr double d{7.1};
      vector_type vm{v1};
      vector_type vd{v2};
      vm *= d;
      vd /= d;
      BOOST_CHECK(
          (vm.x1 == v1.x1 * d && vm.x2 == v1.x2 * d && vm.x3 == v1.x3 * d));
      BOOST_CHECK(
          (vd.x1 == v2.x1 / d && vd.x2 == v2.x2 / d && vd.x3 == v2.x3 / d));
    }
  }
  // magnitude
  {
    constexpr vector_type v1{1., 0, 0};
    constexpr vector_type v2{1, 1, 2};
    BOOST_CHECK((v1.mag2() == 1 && v1.mag() == 1));
    BOOST_CHECK(
        (v2.mag2() == 6 &&
         v2.mag() == sqrt(v2.x1 * v2.x1 + v2.x2 * v2.x2 + v2.x3 * v2.x3)));
  }
}
// 3-vector of quantities
BOOST_AUTO_TEST_CASE(vector_quantity) {
  using namespace quantities;

  // construction and assignment
  {
    constexpr velocity_mm_per_ns_type q1{0.};
    constexpr velocity_mm_per_ns_type q2{1.23};
    constexpr velocity_mm_per_ns_type q3{76.2812};
    constexpr velocity_m_per_ns_type q2m{q2};
    // vector::vector()
    constexpr velocity_vector v1;
    BOOST_CHECK((v1.x1 == q1 && v1.x2 == q1 && v1.x3 == q1));
    BOOST_CHECK((physics::unit_string(v1) == " mm ns^-1"));
    // vector::vector(quantity)
    constexpr velocity_vector v2{q2};
    BOOST_CHECK((v2.x1 == q2 && v2.x2 == q2 && v2.x3 == q2));
    constexpr velocity_vector vm2{q2m};
    BOOST_CHECK((vm2.x1 == q2 && vm2.x2 == q2 && vm2.x3 == q2));
    // vector::vector(q, q, q)
    constexpr velocity_vector v3{q1, q2, q3};
    BOOST_CHECK((v3.x1 == q1 && v3.x2 == q2 && v3.x3 == q3));
    constexpr velocity_vector v3m{q1, q2m, q3};
    BOOST_CHECK((v3m.x1 == q1 && v3m.x2 == q2 && v3m.x3 == q3));
    // vector::vector(q, theta, phi) (spherical)
    // vector::vector(q, theta, x3) (cilindrical)
    {
      constexpr angle_radian_type theta4{0.5};
      constexpr angle_radian_type phi4{1.7};
      const velocity_vector vs{q2, theta4, phi4};
      BOOST_CHECK((vs.x1 == q2 * sin(theta4.value()) * cos(phi4.value()) &&
                   vs.x2 == q2 * sin(theta4.value()) * sin(phi4.value()) &&
                   vs.x3 == q2 * cos(theta4.value())));
      const velocity_vector vc{q2, theta4, q1};
      BOOST_CHECK((vc.x1 == q2 * cos(theta4.value()) &&
                   vc.x2 == q2 * sin(theta4.value()) && vc.x3 == q1));
    }
    // vector::vector(vector)
    constexpr velocity_vector v4{v3};
    BOOST_CHECK((v4.x1 == v3.x1 && v4.x2 == v3.x2 && v4.x3 == v3.x3));
    constexpr velocity2_vector vm4{v4};
    constexpr velocity_vector v5{vm4};
    BOOST_CHECK((v5.x1 == v3.x1 && v5.x2 == v3.x2 && v5.x3 == v3.x3));
    // vector& vector::operator=(vector)
    {
      velocity_vector v;
      v = v4;
      BOOST_CHECK((v.x1 == v4.x1 && v.x2 == v4.x2 && v.x3 == v4.x3));
      // operator= returns reference
      (v = v3) = v1;
      BOOST_CHECK((v.x1 == v1.x1 && v.x2 == v1.x2 && v.x3 == v1.x3));
    }
  }
  // comparison
  {
    constexpr velocity_mm_per_ns_type q1{0.};
    constexpr velocity_mm_per_ns_type q2{1.23};
    constexpr velocity_mm_per_ns_type q3{76.2812};
    constexpr velocity_vector v1{q1, q2, q3};
    constexpr velocity_vector v2{q1, q1, q3};
    BOOST_CHECK((v1 == v1));
    BOOST_CHECK((v1 != v2));
  }
  // vector arithmetic
  {
    constexpr velocity_mm_per_ns_type v1{0.};
    constexpr velocity_mm_per_ns_type v2{1.23};
    constexpr velocity_mm_per_ns_type v3{76.2812};
    constexpr velocity_vector v{v1, v2, v3};
    constexpr velocity_m_per_ns_type vm1{0.32};
    constexpr velocity_m_per_ns_type vm2{0.8};
    // not a typo, vm.x3 is initialized from compatible quantity
    constexpr velocity2_vector vm{vm1, vm2, v3};
    constexpr energy_MeV_type p1{3.};
    constexpr energy_MeV_type p2{139.12};
    constexpr energy_MeV_type p3{12.35};
    constexpr energy_vector p{p1, p2, p3};
    // vector::operator+(vector)
    // vector::operator-(vector)
    {
      constexpr velocity_vector va{v + vm};
      constexpr velocity_vector vs{v - vm};
      BOOST_CHECK((va.x1 == v.x1 + vm.x1 && va.x2 == v.x2 + vm.x2 &&
                   va.x3 == v.x3 + vm.x3));
      BOOST_CHECK((va.x1.value() != v.x1.value() + vm.x1.value()));
      BOOST_CHECK((vs.x1 == v.x1 - vm.x1 && vs.x2 == v.x2 - vm.x2 &&
                   vs.x3 == v.x3 - vm.x3));
      BOOST_CHECK((vs.x1.value() != v.x1.value() - vm.x1.value()));
    }
    // vector::operator*(double)
    // vector::operator/(double)
    // operator*(double, vector)
    // vector::operator*(vector<double>) (scalar)
    // vector::operator^(vector<double>) (cross)
    {
      using double_vector = physics::vector<double>;
      constexpr double_vector vd{2., 5., -234.e4};
      constexpr double d{4.291e+121};
      constexpr velocity_vector mv = v * d;
      constexpr velocity_vector md = v / d;
      BOOST_CHECK(
          (mv.x1 == v.x1 * d && mv.x2 == v.x2 * d && mv.x3 == v.x3 * d));
      BOOST_CHECK(
          (md.x1 == v.x1 / d && md.x2 == v.x2 / d && md.x3 == v.x3 / d));
      constexpr velocity_vector mv2 = d * v;
      BOOST_CHECK(
          (mv2.x1 == v.x1 * d && mv2.x2 == v.x2 * d && mv2.x3 == v.x3 * d));
      constexpr auto scalar = v * vd;
      BOOST_CHECK((scalar == v.x1 * vd.x1 + v.x2 * vd.x2 + v.x3 * vd.x3));
      BOOST_CHECK(
          (scalar.value() ==
           v.x1.value() * vd.x1 + v.x2.value() * vd.x2 + v.x3.value() * vd.x3));
      BOOST_CHECK((physics::unit_string(scalar) == " mm ns^-1"));
      constexpr auto vc = v ^ vd;
      BOOST_CHECK((vc.x1 == v.x2 * vd.x3 - v.x3 * vd.x2 &&
                   vc.x2 == v.x3 * vd.x1 - v.x1 * vd.x3 &&
                   vc.x3 == v.x1 * vd.x2 - v.x2 * vd.x1));
      BOOST_CHECK((physics::unit_string(vc) == " mm ns^-1"));
    }
    // vector::operator*(quantity)
    // vector::operator/(quantity)
    // operator*(quantity, vector)
    // vector::operator*(vector<quantity>) (scalar)
    // vector::operator^(vector<quantity>) (cross)
    {
      constexpr auto mv = v * p1;
      constexpr auto md = v / p1;
      BOOST_CHECK(
          (mv.x1 == v.x1 * p1 && mv.x2 == v.x2 * p1 && mv.x3 == v.x3 * p1));
      BOOST_CHECK((physics::unit_string(mv) == " mm ns^-1 MeV"));
      BOOST_CHECK(
          (md.x1 == v.x1 / p1 && md.x2 == v.x2 / p1 && md.x3 == v.x3 / p1));
      BOOST_CHECK((physics::unit_string(md) == " mm ns^-1 MeV^-1"));
      constexpr auto mv2 = p2 * v;
      BOOST_CHECK(
          (mv2.x1 == v.x1 * p2 && mv2.x2 == v.x2 * p2 && mv2.x3 == v.x3 * p2));
      BOOST_CHECK((physics::unit_string(mv2) == " mm ns^-1 MeV"));
      constexpr auto scalar = v * p;
      BOOST_CHECK((scalar == v.x1 * p.x1 + v.x2 * p.x2 + v.x3 * p.x3));
      BOOST_CHECK((physics::unit_string(scalar) == " mm ns^-1 MeV"));
      constexpr auto vc = v ^ p;
      BOOST_CHECK((vc.x1 == v.x2 * p.x3 - v.x3 * p.x2 &&
                   vc.x2 == v.x3 * p.x1 - v.x1 * p.x3 &&
                   vc.x3 == v.x1 * p.x2 - v.x2 * p.x1));
      BOOST_CHECK((physics::unit_string(vc) == " mm ns^-1 MeV"));
    }
  }
  // arithmetic assignment
  {
    constexpr velocity_mm_per_ns_type v1{0.};
    constexpr velocity_mm_per_ns_type v2{1.23};
    constexpr velocity_mm_per_ns_type v3{76.2812};
    constexpr velocity_vector v{v1, v2, v3};
    // vector::operator+=(vector)
    // vector::operator-=(vector)
    {
      velocity_vector va{v1};
      velocity_vector vs{v3};
      va += 2 * v;
      vs -= 2 * v;
      BOOST_CHECK((va.x1 == v1 + 2 * v.x1 && va.x2 == v1 + 2 * v.x2 &&
                   va.x3 == v1 + 2 * v.x3));
      BOOST_CHECK((vs.x1 == v3 - 2 * v.x1 && vs.x2 == v3 - 2 * v.x2 &&
                   vs.x3 == v3 - 2 * v.x3));
    }
    // vector::operator*=(double)
    // vector::operator/=(double)
    {
      constexpr double d{7.1};
      velocity_vector vm{v};
      velocity_vector vd{v};
      vm *= d;
      vd /= d;
      BOOST_CHECK(
          (vm.x1 == v.x1 * d && vm.x2 == v.x2 * d && vm.x3 == v.x3 * d));
      BOOST_CHECK(
          (vd.x1 == v.x1 / d && vd.x2 == v.x2 / d && vd.x3 == v.x3 / d));
    }
  }
  // magnitude
  {
    constexpr velocity_mm_per_ns_type v1{0.};
    constexpr velocity_mm_per_ns_type v2{1.23};
    constexpr velocity_mm_per_ns_type v3{76.2812};
    constexpr velocity_vector v{v1, v2, v3};
    BOOST_CHECK((v.mag2() == v1 * v1 + v2 * v2 + v3 * v3));
    BOOST_CHECK((unit_string(v.mag2()) == " mm^2 ns^-2"));
    BOOST_CHECK((v.mag() == sqrt(v1 * v1 + v2 * v2 + v3 * v3)));
    BOOST_CHECK((unit_string(v.mag()) == " mm ns^-1"));
  }
}

BOOST_AUTO_TEST_CASE(lorentzvector_double) {
  using vector_type = physics::lorentzvector<double>;

  // construction and assignment
  {
    // lorentzvector::lorentvector
    constexpr vector_type v1;
    BOOST_CHECK((v1.x0 == 0 && v1.x.x1 == 0 && v1.x.x2 == 0 && v1.x.x3 == 0));
    // lorentzvector::lorentzvector(double)
    constexpr vector_type v2{3.14};
    BOOST_CHECK((v2.x0 == 3.14 && v2.x.x1 == 3.14 && v2.x.x2 == 3.14 &&
                 v2.x.x3 == 3.14));
    // lorentzvector::lorentzvector(double, vector)
    constexpr vector_type v3{5., {1., 2., 3.}};
    BOOST_CHECK((v3.x0 == 5. && v3.x.x1 == 1 && v3.x.x2 == 2 && v3.x.x3 == 3));
    // lorentzvector::lorentzvector(lorentzvector)
    constexpr vector_type v4{v3};
    BOOST_CHECK((v4.x0 == 5. && v4.x.x1 == 1 && v4.x.x2 == 2 && v4.x.x3 == 3));
    // lorentzvector& lorentzvector::operator=(lorentzvector)
    {
      vector_type v5;
      v5 = v4;
      BOOST_CHECK((v5.x0 == v4.x0 && v5.x.x1 == v4.x.x1 && v5.x.x2 == v4.x.x2 &&
                   v5.x.x3 == v4.x.x3));
    }
  }
  // comparison
  {
    constexpr vector_type v1{4, {1., 0, 0}};
    constexpr vector_type v2{4, {1, 1, 2}};
    BOOST_CHECK((v1 == v1));
    BOOST_CHECK((v1 != v2));
  }
  // vector arithmetic
  {
    constexpr vector_type v1{1., {2., 3., 4.}};
    constexpr vector_type v2{4.1, {2.4, 9.2, -9.4}};
    // lorentzvector operator+(lorentzvector)
    // lorentzvector operator-(lorentzvector)
    {
      constexpr vector_type va = v1 + v2;
      constexpr vector_type vs = v1 - v2;
      BOOST_CHECK((va.x0 == v1.x0 + v2.x0 && va.x == v1.x + v2.x));
      BOOST_CHECK((vs.x0 == v1.x0 - v2.x0 && vs.x == v1.x - v2.x));
    }
    // lorentzvector::operator*(double)
    // lorentzvector::operator/(double)
    // operator*(double, lorentzvector)
    // lorentzvector::operator*(lorentzvector<double>) (scalar)
    {
      constexpr double d{8.1};
      constexpr vector_type vm{v1 * d};
      constexpr vector_type vd{v1 / d};
      BOOST_CHECK((vm.x0 == v1.x0 * d && vm.x == v1.x * d));
      BOOST_CHECK((vd.x0 == v1.x0 / d && vd.x == v1.x / d));
      constexpr vector_type vm2{d * vm};
      BOOST_CHECK((vm2.x0 == vm.x0 * d && vm2.x == vm.x * d));
      constexpr double scalar = v1 * v2;
      BOOST_CHECK((scalar == v1.x0 * v2.x0 - v1.x * v2.x));
    }
    // a 4-vector of raw doubles can be multiplied/divided by a dimensionful
    // quantity
    // lorentzvector::operator*(Quantity)
    // lorentzvector::operator/(Quantity)
    // operator*(Quantity, lorentzvector)
    // lorentzvector::operator*(lorentzvector<Quantity) (scalar)
    {
      using sys_type = physics::unit_system<mm_name, ns_name, MeV_name>;
      using distance_type =
          physics::unit_dimensions<std::ratio<1>, std::ratio<0>, std::ratio<0>>;
      using mm_type = physics::unit<sys_type, distance_type, std::ratio<0>,
                                    std::ratio<0>, std::ratio<1>>;
      using distance_mm_type = physics::quantity<mm_type>;
      using distance_vector = physics::lorentzvector<distance_mm_type>;
      constexpr distance_mm_type distance{42.};
      constexpr distance_vector vdistance{distance};
      constexpr auto vm = v1 * distance;
      constexpr auto vd = v1 / distance;
      BOOST_CHECK((physics::unit_string(vm) == " mm" &&
                   physics::unit_string(vd) == " mm^-1"));
      BOOST_CHECK((vm.x0 == v1.x0 * distance && vm.x == v1.x * distance));
      BOOST_CHECK((vd.x0 == v1.x0 / distance && vd.x == v1.x / distance));
      constexpr auto vm2 = distance * v2;
      BOOST_CHECK((physics::unit_string(vm2) == " mm"));
      BOOST_CHECK((vm2.x0 == v2.x0 * distance && vm2.x == v2.x * distance));
      constexpr auto scalar = v1 * vdistance;
      BOOST_CHECK((scalar == v1.x0 * vdistance.x0 - v1.x * vdistance.x));
      BOOST_CHECK((unit_string(scalar) == " mm"));
    }
  }
  // arithmetic assignment
  {
    constexpr vector_type v1{9, {1., 2., 3.}};
    constexpr vector_type v2{7.2, {2.1, 3.2, 4.3}};
    // vector::operator+=(vector)
    // vector::operator-=(vector)
    {
      vector_type va{v1};
      vector_type vs{v1};
      va += v2;
      vs -= v2;
      BOOST_CHECK((va.x0 == v1.x0 + v2.x0 && va.x == v1.x + v2.x));
      BOOST_CHECK((vs.x0 == v1.x0 - v2.x0 && vs.x == v1.x - v2.x));
    }
    // vector::operator*=(double)
    // vector::operator/=(double)
    {
      constexpr double d{7.1};
      vector_type vm{v1};
      vector_type vd{v2};
      vm *= d;
      vd /= d;
      BOOST_CHECK((vm.x0 == v1.x0 * d && vm.x == v1.x * d));
      BOOST_CHECK((vd.x0 == v2.x0 / d && vd.x == v2.x / d));
    }
  }
  // magnitude
  {
    constexpr vector_type v1{1., {1, 0, 0}};
    constexpr vector_type v2{83, {1, 1, 2}};
    constexpr vector_type v3{0, {1, 1, 2}};
    BOOST_CHECK((v1.mag2() == 0 && v1.mag() == 0));
    BOOST_CHECK((v2.mag2() == v2.x0 * v2.x0 - v2.x.mag2() &&
                 v2.mag() == sqrt(v2.x0 * v2.x0 - v2.x * v2.x)));
    BOOST_CHECK(
        (v2.mag2() > 0 && v2.mag() > 0 && v3.mag2() < 0 && v3.mag() < 0));
  }
  // boost
  {
    // lorentzvector::beta()
    constexpr vector_type v1{4, {3, 2, 1}};
    BOOST_CHECK((v1.beta() == v1.x/v1.x0));
    // boost to COM frame
    auto v2 = v1.boost(v1.beta());
    BOOST_CHECK((fabs(v2.mag2() - v1.mag2()) < 1e-10));
    BOOST_CHECK((v2.x == vector_type::vector_type{0, 0, 0}));
  }
}

BOOST_AUTO_TEST_CASE(lorentzvector_quantity) {
  using namespace quantities;

  // construction and assignment
  {
    constexpr velocity_mm_per_ns_type q1{0.};
    constexpr velocity_mm_per_ns_type q2{1.23};
    constexpr velocity_mm_per_ns_type q3{76.2812};
    constexpr velocity_m_per_ns_type q2m{q2};
    // lorentzvector::lorentzvector()
    constexpr velocity_lorentzvector v1;
    BOOST_CHECK((v1.x0 == q1 && v1.x == velocity_vector{q1}));
    BOOST_CHECK((physics::unit_string(v1) == " mm ns^-1"));
    // lorentzvector::lorentzvector(quantity)
    constexpr velocity_lorentzvector v2{q2};
    BOOST_CHECK((v2.x0 == q2 && v2.x == velocity_vector{q2}));
    constexpr velocity_lorentzvector vm2{q2m};
    BOOST_CHECK((vm2.x0 == q2 && vm2.x == velocity_vector{q2}));
    // lorentzvector::lorentzvector(q, vector)
    constexpr velocity_lorentzvector v3{q2, {q1, q2, q3}};
    BOOST_CHECK((v3.x0 == q2 && v3.x == velocity_vector{q1, q2, q3}));
    constexpr velocity_lorentzvector v3m{q2m, {q1, q2m, q3}};
    BOOST_CHECK((v3m.x0 == q2 && v3m.x == velocity_vector{q1, q2, q3}));
    // lorentzvector::lorentzvector(lorentzvector)
    constexpr velocity_lorentzvector v4{v3};
    BOOST_CHECK((v4.x0 == v3.x0 && v4.x == v3.x));
    // lorentzvector& lorentzvector::operator=(lorentzvector)
    {
      velocity_lorentzvector v;
      v = v4;
      BOOST_CHECK((v.x0 == v4.x0 && v.x == v4.x));
    }
  }
  // comparison
  {
    constexpr velocity_mm_per_ns_type q1{0.};
    constexpr velocity_mm_per_ns_type q2{1.23};
    constexpr velocity_mm_per_ns_type q3{76.2812};
    constexpr velocity_lorentzvector v1{q2, {q1, q2, q3}};
    constexpr velocity_lorentzvector v2{q1, {q1, q2, q3}};
    BOOST_CHECK((v1 == v1));
    BOOST_CHECK((v1 != v2));
  }
  // lorentzvector arithmetic
  {
    constexpr velocity_mm_per_ns_type v1{0.};
    constexpr velocity_mm_per_ns_type v2{1.23};
    constexpr velocity_mm_per_ns_type v3{76.2812};
    constexpr velocity_lorentzvector v{v2, {v1, v2, v3}};
    constexpr velocity_m_per_ns_type vm1{0.32};
    constexpr velocity_m_per_ns_type vm2{0.8};
    // not a typo, vm.x3 is initialized from compatible quantity
    constexpr velocity2_lorentzvector vm{vm2, {vm1, vm2, v3}};
    constexpr energy_MeV_type p1{3.};
    constexpr energy_MeV_type p2{139.12};
    constexpr energy_MeV_type p3{12.35};
    constexpr energy_lorentzvector p{p1, {p1, p2, p3}};
    // lorentzvector::operator+(lorentzvector)
    // lorentzvector::operator-(lorentzvector)
    {
      constexpr velocity_lorentzvector va{v + vm};
      constexpr velocity_lorentzvector vs{v - vm};
      BOOST_CHECK((va.x0 == v.x0 + vm.x0 && va.x == v.x + vm.x));
      BOOST_CHECK((vs.x0 == v.x0 - vm.x0 && vs.x == v.x - vm.x));
    }
    // lorentzvector::operator*(double)
    // lorentzvector::operator/(double)
    // operator*(double, lorentzvector)
    // lorentzvector::operator*(lorentzvector<double>) (scalar)
    // lorentzvector::operator^(lorentzvector<double>) (cross)
    {
      using double_lorentzvector = physics::lorentzvector<double>;
      constexpr double_lorentzvector dd{87, {2., 5., -234.e4}};
      constexpr double d{4.291e+121};
      constexpr velocity_lorentzvector vm = v * d;
      constexpr velocity_lorentzvector vd = v / d;
      BOOST_CHECK((vm.x0 == v.x0 * d && vm.x == v.x * d));
      BOOST_CHECK((vd.x0 == v.x0 / d && vd.x == v.x / d));
      constexpr velocity_lorentzvector vm2 = d * v;
      BOOST_CHECK((vm2.x0 == v.x0 * d && vm2.x == v.x * d));
      constexpr auto scalar = v * dd;
      BOOST_CHECK((scalar == v.x0 * dd.x0 - v.x * dd.x));
      BOOST_CHECK((physics::unit_string(scalar) == " mm ns^-1"));
    }
    // lorentzvector::operator*(quantity)
    // lorentzvector::operator/(quantity)
    // operator*(quantity, lorentzvector)
    // lorentzvector::operator*(lorentzvector<quantity>) (scalar)
    // lorentzvector::operator^(lorentzvector<quantity>) (cross)
    {
      constexpr auto vm = v * p1;
      constexpr auto vd = v / p1;
      BOOST_CHECK((physics::unit_string(vm) == " mm ns^-1 MeV" &&
                   physics::unit_string(vd) == " mm ns^-1 MeV^-1"));
      BOOST_CHECK((vm.x0 == v.x0 * p1 && vm.x == v.x * p1));
      BOOST_CHECK((vd.x0 == v.x0 / p1 && vd.x == v.x / p1));
      constexpr auto vm2 = p2 * v;
      BOOST_CHECK((physics::unit_string(vm2) == " mm ns^-1 MeV"));
      BOOST_CHECK((vm2.x0 == v.x0 * p2 && vm2.x == v.x * p2));
      constexpr auto scalar = v * p;
      BOOST_CHECK((scalar == v.x0 * p.x0 - v.x * p.x));
      BOOST_CHECK((physics::unit_string(scalar) == " mm ns^-1 MeV"));
    }
  }
  // arithmetic assignment
  {
    constexpr velocity_mm_per_ns_type v1{0.};
    constexpr velocity_mm_per_ns_type v2{1.23};
    constexpr velocity_mm_per_ns_type v3{76.2812};
    constexpr velocity_lorentzvector v{v2, {v1, v2, v3}};
    // lorentzvector::operator+=(lorentzvector)
    // lorentzvector::operator-=(lorentzvector)
    {
      velocity_lorentzvector va{v1};
      velocity_lorentzvector vs{v3};
      va += 2 * v;
      vs -= 2 * v;
      BOOST_CHECK(
          (va.x0 == v1 + 2 * v.x0 && va.x == velocity_vector{v1} + 2 * v.x));
      BOOST_CHECK(
          (vs.x0 == v3 - 2 * v.x0 && vs.x == velocity_vector{v3} - 2 * v.x));
    }
    // lorentzvector::operator*=(double)
    // lorentzvector::operator/=(double)
    {
      constexpr double d{7.1};
      velocity_lorentzvector vm{v};
      velocity_lorentzvector vd{v};
      vm *= d;
      vd /= d;
      BOOST_CHECK((vm.x0 == v.x0 * d && vm.x == v.x * d));
      BOOST_CHECK((vd.x0 == v.x0 / d && vd.x == v.x / d));
    }
  }
  // magnitude
  {
    constexpr velocity_mm_per_ns_type v1{0.};
    constexpr velocity_mm_per_ns_type v2{1.23};
    constexpr velocity_mm_per_ns_type v3{76.2812};
    constexpr velocity_lorentzvector v{v3, {v1, v2, v3}};
    BOOST_CHECK((v.mag2() == v3 * v3 - v1 * v1 - v2 * v2 - v3 * v3));
    BOOST_CHECK((unit_string(v.mag2()) == " mm^2 ns^-2"));
    BOOST_CHECK((v.mag() == -sqrt(-v3 * v3 + v1 * v1 + v2 * v2 + v3 * v3)));
    BOOST_CHECK((unit_string(v.mag()) == " mm ns^-1"));
  }
  // boost
  {
    constexpr velocity_mm_per_ns_type v0{9.83e3};
    constexpr velocity_mm_per_ns_type v1{0.};
    constexpr velocity_mm_per_ns_type v2{1.23};
    constexpr velocity_mm_per_ns_type v3{76.2812};
    constexpr velocity_lorentzvector v{v0, {v1, v2, v3}};
    BOOST_CHECK((v.beta() == v.x/v.x0 && unit_string(v.beta()) == ""));
    // boost to COM frame
    velocity_lorentzvector vcm = v.boost(v.beta());
    BOOST_CHECK((fabs(vcm.mag2() - physics::mag2(v)).value() < 1e-6));
    BOOST_CHECK((vcm.x == velocity_vector{v1, v1, v1}));
    velocity_lorentzvector vcm_too = physics::boost(v, v.beta());
  }
}
