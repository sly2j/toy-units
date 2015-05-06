#include <iostream>
#include <array>

#define BOOST_TEST_MODULE test_unit
#include <boost/test/unit_test.hpp>

#include "physics/unit.hh"
#include "physics/unit/prefix.hh"
#include "physics/unit/standard.hh"
#include "physics/unit/constants.hh"

constexpr const char* mm_name{"mm"};
constexpr const char* ns_name{"ns"};
constexpr const char* MeV_name{"MeV"};
constexpr const char* radian_name{"rad"};

using std::fabs;

BOOST_AUTO_TEST_CASE(test_unit_system) {
  using sys1_type = physics::unit_system<mm_name, ns_name, MeV_name>;
  using sys2_type =
      physics::unit_system<mm_name, ns_name, MeV_name, radian_name>;
  BOOST_CHECK(physics::unit_impl::is_unit_system<sys1_type>::value == true);
  BOOST_CHECK(physics::unit_impl::is_unit_system<sys2_type>::value == true);
  BOOST_CHECK(physics::unit_impl::is_unit_system<double>::value == false);
}

BOOST_AUTO_TEST_CASE(test_unit_dimensions) {
  using sys1_type = physics::unit_system<mm_name, ns_name, MeV_name>;
  using dim1_type =
      physics::unit_dimensions<std::ratio<1>, std::ratio<0>, std::ratio<0>>;
  BOOST_CHECK(physics::unit_impl::is_unit_dimensions<dim1_type>::value == true);
  BOOST_CHECK(physics::unit_impl::is_unit_dimensions<sys1_type>::value ==
              false);

  // dimension math
}

BOOST_AUTO_TEST_CASE(test_unit) {
  using sys1_type = physics::unit_system<mm_name, ns_name, MeV_name>;
  using sys3_type = physics::unit_system<ns_name, MeV_name>;
  using dim1_type =
      physics::unit_dimensions<std::ratio<1>, std::ratio<0>, std::ratio<0>>;
  using dim2_type =
      physics::unit_dimensions<std::ratio<1>, std::ratio<2>, std::ratio<0>>;
  using dim3_type = physics::unit_dimensions<std::ratio<2>, std::ratio<0>>;

  using u1_type = physics::unit<sys1_type, dim1_type, std::ratio<1>,
                                std::ratio<1>, std::ratio<1, 2>>;
  using u2_type = physics::unit<sys1_type, dim1_type, std::ratio<0>,
                                std::ratio<0>, std::ratio<1>>;
  using u3_type = physics::unit<sys1_type, dim2_type, std::ratio<1>,
                                std::ratio<1>, std::ratio<4, 3>>;
  using u4_type =
      physics::unit<sys3_type, dim3_type, std::ratio<3>, std::ratio<1>>;

  // units
  BOOST_CHECK(physics::unit_impl::is_unit<u1_type>::value == true);
  BOOST_CHECK(physics::unit_impl::is_unit<u2_type>::value == true);
  BOOST_CHECK(physics::unit_impl::is_unit<u3_type>::value == true);
  BOOST_CHECK(physics::unit_impl::is_unit<u4_type>::value == true);
  // not units
  BOOST_CHECK(physics::unit_impl::is_unit<dim1_type>::value == false);
  BOOST_CHECK(physics::unit_impl::is_unit<sys1_type>::value == false);
  BOOST_CHECK(physics::unit_impl::is_unit<double>::value == false);

  // u1 and u2 are compatible
  BOOST_CHECK(
      (physics::unit_impl::are_compatible<u1_type, u2_type>::value == true));
  BOOST_CHECK(
      (physics::unit_impl::have_same_system<u1_type, u2_type>::value == true));
  // u1 and u3 are NOT compatible (different dimensions), but have same system
  BOOST_CHECK(
      (physics::unit_impl::are_compatible<u1_type, u3_type>::value == false));
  BOOST_CHECK(
      (physics::unit_impl::have_same_system<u1_type, u3_type>::value == true));
  // u1 and u4 are not compatible (different dimensions, different system)
  BOOST_CHECK(
      (physics::unit_impl::are_compatible<u1_type, u4_type>::value == false));
  BOOST_CHECK(
      (physics::unit_impl::have_same_system<u1_type, u4_type>::value == false));

  // unit multiply
  using u1xu1_type = physics::unit_multiply<u1_type, u1_type>;
  BOOST_CHECK((u1xu1_type::pow_10::num == 2));
  BOOST_CHECK((u1xu1_type::pow_pi::num == 2));
  BOOST_CHECK((u1xu1_type::factor::num == 1 && u1xu1_type::factor::den == 4));

  using u1xu2_type = physics::unit_multiply<u1_type, u2_type>;
  BOOST_CHECK((u1xu2_type::pow_10::num == 1));
  BOOST_CHECK((u1xu2_type::pow_pi::num == 1));
  BOOST_CHECK((u1xu2_type::factor::num == 1 && u1xu2_type::factor::den == 2));

  using u1xu3_type = physics::unit_multiply<u1_type, u3_type>;
  BOOST_CHECK((u1xu3_type::pow_10::num == 2));
  BOOST_CHECK((u1xu3_type::pow_pi::num == 2));
  BOOST_CHECK((u1xu3_type::factor::num == 2 && u1xu3_type::factor::den == 3));

  // unit divide
  using u1ou1_type = physics::unit_divide<u1_type, u1_type>;
  BOOST_CHECK((u1ou1_type::pow_10::num == 0));
  BOOST_CHECK((u1ou1_type::pow_pi::num == 0));
  BOOST_CHECK((u1ou1_type::factor::num == 1 && u1ou1_type::factor::den == 1));

  using u1ou2_type = physics::unit_divide<u1_type, u2_type>;
  BOOST_CHECK((u1ou2_type::pow_10::num == 1));
  BOOST_CHECK((u1ou2_type::pow_pi::num == 1));
  BOOST_CHECK((u1ou2_type::factor::num == 1 && u1ou2_type::factor::den == 2));

  using u1ou3_type = physics::unit_divide<u1_type, u3_type>;
  BOOST_CHECK((u1ou3_type::pow_10::num == 0));
  BOOST_CHECK((u1ou3_type::pow_pi::num == 0));
  BOOST_CHECK((u1ou3_type::factor::num == 3 && u1ou3_type::factor::den == 8));

  // unit_pow
  using u1p4_type = physics::unit_pow<u1_type, std::ratio<4>>;
  BOOST_CHECK((u1p4_type::pow_10::num == 4));
  BOOST_CHECK((u1p4_type::pow_pi::num == 4));
  BOOST_CHECK((u1p4_type::factor::num == 1 && u1p4_type::factor::den == 16));

  using u2p12_type = physics::unit_pow<u2_type, std::ratio<1, 2>>;
  BOOST_CHECK((u2p12_type::pow_10::num == 0));
  BOOST_CHECK((u2p12_type::pow_pi::num == 0));
  BOOST_CHECK((u2p12_type::factor::num == 1 && u2p12_type::factor::den == 1));
}

BOOST_AUTO_TEST_CASE(test_quantity) {
  using sys1_type = physics::unit_system<mm_name, ns_name, MeV_name>;
  using distance_type =
      physics::unit_dimensions<std::ratio<1>, std::ratio<0>, std::ratio<0>>;

  using pi_cm_over_2_type =
      physics::unit<sys1_type, distance_type, std::ratio<1>, std::ratio<1>,
                    std::ratio<1, 2>>;
  using mm_type = physics::unit<sys1_type, distance_type, std::ratio<0>,
                                std::ratio<0>, std::ratio<1>>;

  using distance_1_type = physics::quantity<pi_cm_over_2_type>;
  using distance_mm_type = physics::quantity<mm_type>;
  using u00_type =
      physics::unit<sys1_type, sys1_type::dimensionless, std::ratio<2>,
                    std::ratio<1>, std::ratio<1, 4>>;
  using u01_type =
      physics::unit<sys1_type, sys1_type::dimensionless, std::ratio<0>,
                    std::ratio<0>, std::ratio<3, 2>>;
  using q00_type = physics::quantity<u00_type>;
  using q01_type = physics::quantity<u01_type>;

  //
  // construction and value
  //
  constexpr distance_1_type q_pi_cm_over_2{12};
  constexpr distance_mm_type q_mm{q_pi_cm_over_2};
  {
    // casting automatically converts units
    BOOST_CHECK((q_pi_cm_over_2.value() == 12));
    double val = q_pi_cm_over_2.value() * (physics::unit_impl::pi * 10.) / 2.;
    BOOST_CHECK((q_mm.value() == val));
    // extract value in another unit
    BOOST_CHECK((q_mm.value<pi_cm_over_2_type>() == q_pi_cm_over_2.value()));
    BOOST_CHECK((q_mm.value<mm_type>() == q_mm.value()));
    // value and raw_value are the same for dimensionfull units
    BOOST_CHECK((q_mm.value() == q_mm.raw_value()));
    // rather complex dimensionless unit: scaled by 10^2, pi^1 and a factor of
    // 1/4
  }
  constexpr q00_type q00{12};
  constexpr q01_type q01{8.9};
  {
    BOOST_CHECK((q00.raw_value() == 12 && q01.raw_value() == 8.9));
    double val0 = 12 * (physics::unit_impl::pi * 100.) / 4.;
    double val1 = 8.9 * 3. / 2.;
    // dimensionless units behave like raw doubles (automatically apply scale
    // factors)
    BOOST_CHECK((q00.value() == val0 && q01.value() == val1));
  }

  //
  // multiplication and division (implicit testing of I/O,
  // construction/assignment/etc...)
  //
  // quantity<>::operator*(quantity<Unit2>)
  // quantity<>::operator/(quantity<Unit2>)
  {
    auto qm = q_mm * q_pi_cm_over_2;
    auto qd = q_mm / q_pi_cm_over_2;
    BOOST_CHECK((qm.value() == q_mm.value() * q_pi_cm_over_2.value()));
    BOOST_CHECK((qd.value() == 1));
    using qm_unit_type = typename decltype(qm)::unit;
    using qd_unit_type = typename decltype(qd)::unit;
    BOOST_CHECK((qm_unit_type::pow_10::num == 1));
    BOOST_CHECK((qm_unit_type::pow_pi::num == 1));
    BOOST_CHECK(
        (qm_unit_type::factor::num == 1 && qm_unit_type::factor::den == 2));
    BOOST_CHECK(
        (physics::unit_string<qm_unit_type>() == " x (0.5 x 10 x pi) mm^2"));
    BOOST_CHECK((qd_unit_type::pow_10::num == -1));
    BOOST_CHECK((qd_unit_type::pow_pi::num == -1));
    BOOST_CHECK(
        (qd_unit_type::factor::num == 2 && qd_unit_type::factor::den == 1));
  }
  // quantity<>::operator*(quantity<Dimensionless>)
  // quantity<>::operator*=(quantity<Dimensionless>)
  // quantity<>::operator*(double)
  // quantity<>::operator*=(double)
  // operator*(double, quantity<>)
  // quantity<>::operator/(quantity<Dimensionless>)
  // quantity<>::operator/=(quantity<Dimensionless>)
  // quantity<>::operator/(double)
  // quantity<>::operator/=(double)
  // operator/(double, quantity<>)
  {
    auto qm = q_mm * q00;
    auto qd = q_mm / q00;
    BOOST_CHECK((qm.value() == q_mm.value() * q00.raw_value()));
    BOOST_CHECK((physics::unit_string(qm) == " x (0.25 x 10^2 x pi) mm"));
    BOOST_CHECK((qd.value() == q_mm.value() / q00.raw_value()));
    BOOST_CHECK((physics::unit_string(qd) == " x (4 x 10^-2 x pi^-1) mm"));
    qm *= q00; // scaling not changed because we use operator *= here
                // (a temporary in unites of q_mm*q00*q00 is casted to qm)
    qd /= q00;
    BOOST_CHECK((qm.value() == q_mm.value() * q00.raw_value() * q00.value()));
    BOOST_CHECK((qd.value() == q_mm.value() / q00.raw_value() / q00.value()));
    qm = qm * 3.2;
    qd = qd / 3.2;
    BOOST_CHECK(
        (qm.value() == q_mm.value() * q00.raw_value() * q00.value() * 3.2));
    BOOST_CHECK(
        (qd.value() == q_mm.value() / q00.raw_value() / q00.value() / 3.2));
    qm *= 2.1;
    qd /= 2.1;
    BOOST_CHECK((qm.value() ==
                 q_mm.value() * q00.raw_value() * q00.value() * 3.2 * 2.1));
    BOOST_CHECK((qd.value() ==
                 q_mm.value() / q00.raw_value() / q00.value() / 3.2 / 2.1));
    qm = 3.4 * qm;
    auto qd_inv = 1. / qd;
    BOOST_CHECK(
        (qm.value() ==
         q_mm.value() * q00.raw_value() * q00.value() * 3.2 * 2.1 * 3.4));
    BOOST_CHECK((qd_inv.value() == 1. / qd.value()));
    BOOST_CHECK(
        (physics::unit_string(qd_inv) == " x (0.25 x 10^2 x pi) mm^-1"));
  }
  // constexpr-ness of the above operators
  {
    constexpr auto qm1 = q_mm * q00;
    constexpr auto qd1 = q_mm / q00;
    constexpr auto qm2 = q_mm * 3.2;
    constexpr auto qd2 = q_mm / 3.2;
    constexpr auto qm3 = 3.4 * q_mm;
    constexpr auto qd3 = 1. / q_mm;
  }
  // quantity<Dimensionless>::operator*(quantity<Unit2>)
  // quantity<Dimensionless>::operator/(quantity<Unit2>)
  {
    constexpr auto qm = q00 * q_mm;
    constexpr auto qd = q00 / q_mm;
    BOOST_CHECK((qm.value() == q_mm.value() * q00.raw_value()));
    BOOST_CHECK((physics::unit_string(qm) == " x (0.25 x 10^2 x pi) mm"));
    BOOST_CHECK((qd.value() == q00.raw_value() / q_mm.value()));
    BOOST_CHECK((physics::unit_string(qd) == " x (0.25 x 10^2 x pi) mm^-1"));
  }
  // quantity<Dimensionless>::operator*(quantity<Dimensionless>)
  // quantity<Dimensionless>::operator*=(quantity<Dimensionless>)
  // quantity<Dimensionless>::operator*(double)
  // quantity<Dimensionless>::operator*=(double)
  // operator*(double, quantity<Dimensionless>)
  // operator double()
  // operator*=(double, quantity<Dimenionless>) (through implicit cast)
  // quantity<Dimensionless>::operator/(quantity<Dimensionless>)
  // quantity<Dimensionless>::operator/=(quantity<Dimensionless>)
  // quantity<Dimensionless>::operator/(double)
  // quantity<Dimensionless>::operator/=(double)
  // operator/(double, quantity<Dimensionless>)
  // operator double()
  // operator/=(double, quantity<Dimenionless>) (through implicit cast)
  //
  {
    auto qm = q00 * q00;
    auto qd = q00 / q01;
    BOOST_CHECK((qm.raw_value() == q00.raw_value() * q00.raw_value()));
    BOOST_CHECK((fabs(qm.value() - q00.value() * q00.value()) <
                 1.e-8)); // finite machine precision
    BOOST_CHECK((physics::unit_string(qm) == " x (0.0625 x 10^4 x pi^2)"));
    BOOST_CHECK((qd.raw_value() == q00.raw_value() / q01.raw_value()));
    qm = qm * q00;
    BOOST_CHECK(
        (qm.raw_value() == q00.raw_value() * q00.raw_value() * q00.value()));
    qm *= q00;
    qd /= q01;
    BOOST_CHECK(
        (qm.raw_value() ==
         q00.raw_value() * q00.raw_value() * q00.value() * q00.value()));
    BOOST_CHECK(
        (qd.raw_value() == q00.raw_value() / q01.raw_value() / q01.value()));
    qm = qm * 3.2;
    qd = qd / 3.2;
    BOOST_CHECK(
        (qm.raw_value() ==
         q00.raw_value() * q00.raw_value() * q00.value() * q00.value() * 3.2));
    BOOST_CHECK((qd.raw_value() ==
                 q00.raw_value() / q01.raw_value() / q01.value() / 3.2));
    qm *= 2.1;
    qd /= 2.1;
    BOOST_CHECK((qm.raw_value() ==
                 q00.raw_value() * q00.raw_value() * q00.value() * q00.value() *
                     3.2 * 2.1));
    BOOST_CHECK((qd.raw_value() ==
                 q00.raw_value() / q01.raw_value() / q01.value() / 3.2 / 2.1));
    qm = 3.4 * qm;
    auto qd_inv = 1. / qd;
    BOOST_CHECK((qm.raw_value() ==
                 q00.raw_value() * q00.raw_value() * q00.value() * q00.value() *
                     3.2 * 2.1 * 3.4));
    BOOST_CHECK((qd_inv.value() == 1. / qd.value()));
    BOOST_CHECK((physics::unit_string(qd_inv) == " x (6 x 10^-2 x pi^-1)"));
    double dm = qm;
    double dd = qd;
    dm *= qm;
    dd /= qd;
    BOOST_CHECK((dm == qm.value() * qm.value()));
    BOOST_CHECK((dd == 1));
  }
  // constexpr-ness of the above operators
  {
    constexpr auto qm1 = q00 * q01;
    constexpr auto qd1 = q00 / q01;
    constexpr auto qm2 = q00 * 3.2;
    constexpr auto qd2 = q00 / 3.2;
    constexpr auto qm3 = 3.4 * q00;
    constexpr auto qd3 = 1. / q00;
    constexpr double d = q00;
  }
  // addition and subtraction
  //
  // quantity<>::operator+(quantity<>)
  // quantity<>::operator-(quantity<>)
  // quantity<>::operator+=(quantity<>)
  // quantity<>::operator-=(quantity<>)
  // quantity<>::operator-() (unary minus)
  {
    auto qa = q_mm + q_pi_cm_over_2;
    auto qs = q_mm - q_pi_cm_over_2;
    BOOST_CHECK((physics::unit_string(qa) == physics::unit_string(q_mm)));
    BOOST_CHECK((physics::unit_string(qs) == physics::unit_string(q_mm)));
    BOOST_CHECK((qa.value() == q_mm.value() + q_pi_cm_over_2.value<mm_type>()));
    BOOST_CHECK((qs.value() == q_mm.value() - q_pi_cm_over_2.value<mm_type>()));
    qa += q_pi_cm_over_2;
    qs -= q_pi_cm_over_2;
    BOOST_CHECK(
        (qa.value() == q_mm.value() + 2. * q_pi_cm_over_2.value<mm_type>()));
    BOOST_CHECK(
        (qs.value() == q_mm.value() - 2. * q_pi_cm_over_2.value<mm_type>()));
    BOOST_CHECK(((-qs).value() == -(qs.value())));
  }
  // constexpr-ness of the above operators
  {
    constexpr auto qa = q_mm + q_pi_cm_over_2;
    constexpr auto qs = q_mm - q_pi_cm_over_2;
  }
  // quantity<Unitless>::operator+(quantity<Unitless>) (through cast to double)
  // quantity<Unitless>::operator-(quantity<Unitless>) (through cast to double)
  // quantity<Unitless>::operator+(double)
  // quantity<Unitless>::operator-(double)
  // operator+(double, quantity<Unitless>)
  // operator-(double, quantity<Unitless>)
  // quantity<Unitless>::operator+=(quantity<Unitless>)
  // quantity<Unitless>::operator-=(quantity<Unitless>)
  // quantity<Unitless>::operator+=(double)
  // quantity<Unitless>::operator-=(double)
  // quantity<Unitless>::operator-() (unary minus)
  {
    auto qa = q00 + q01;
    auto qs = q00 - q01;
    // finite precision and complex unit conversions with powers of pi require
    // tolerance checking
    BOOST_CHECK((fabs(qa.value() - (q00.value() + q01.value())) < 1.e-10));
    BOOST_CHECK((fabs(qs.value() - (q00.value() - q01.value())) < 1.e-10));
    qa = qa + 4.1;
    qs = qs - 4.1;
    BOOST_CHECK(
        (fabs(qa.value() - (q00.value() + q01.value() + 4.1)) < 1.e-10));
    BOOST_CHECK(
        (fabs(qs.value() - (q00.value() - q01.value() - 4.1)) < 1.e-10));
    double da = 1.2 + qa;
    double ds = 1.2 - qs;
    BOOST_CHECK((fabs(da - (1.2 + qa.value())) < 1.e-10));
    BOOST_CHECK((fabs(ds - (1.2 - qs.value())) < 1.e-10));
    qa += q01;
    qs -= q01;
    BOOST_CHECK(
        (fabs(qa.value() - (q00.value() + 2 * q01.value() + 4.1)) < 1.e-10));
    BOOST_CHECK(
        (fabs(qs.value() - (q00.value() - 2 * q01.value() - 4.1)) < 1.e-10));
    qa += 5.6;
    qs -= 5.6;
    BOOST_CHECK((fabs(qa.value() -
                      (q00.value() + 2 * q01.value() + 4.1 + 5.6)) < 1.e-10));
    BOOST_CHECK((fabs(qs.value() -
                      (q00.value() - 2 * q01.value() - 4.1 - 5.6)) < 1.e-10));
    BOOST_CHECK(((-qs).value() == -(qs.value())));
  }
  // constexpr-ness of the above operators
  {
    constexpr auto qa1 = q00 + q01;
    constexpr auto qs1 = q00 - q01;
    constexpr auto qa2 = qa1 + 4.1;
    constexpr auto qs2 = qs1 - 4.1;
    constexpr double da = 1.2 + qa1;
    constexpr double ds = 1.2 - qs1;
    constexpr auto qm = -q00;
  }

  //
  // comparison operators
  // 
  // quantity<>::operator==()
  // quantity<>::operator!=()
  // quantity<>::operator<()
  // quantity<>::operator>()
  // quantity<>::operator>=()
  // quantity<>::operator<=()
  {
    constexpr distance_mm_type qa0 {q_mm};
    constexpr distance_mm_type qa1 {2*q_mm};
    constexpr distance_mm_type qa2 {3*q_mm};
    constexpr distance_1_type qb0 {qa0};
    constexpr distance_1_type qb1 {qa1};
    constexpr distance_1_type qb2 {qa2};
    // equal
    BOOST_CHECK((qa0 == qb0 && qa1 == qb1 && qa2 == qb2 && !(qa0 == qa1) &&
                 !(qa0 == qb1)));
    // not equal
    BOOST_CHECK((qa0 != qa1) && (qa0 != qa2) && (qa1 != qa2) && !(qa1 != qa1) &&
                !(qa1 != qb1));
    // less than
    BOOST_CHECK((qa0 < qa1 && qa0 < qa2 && qa1 < qa2 && !(qa1 < qa0) &&
                 !(qa2 < qa0) && !(qa2 < qa1)));
    BOOST_CHECK((qa0 < qb1 && qb0 < qa2 && qa1 < qb2 && !(qb1 < qa0) &&
                 !(qa2 < qb0) && !(qb2 < qa1)));
    // greater than
    BOOST_CHECK((qa1 > qa0 && qa2 > qa0 && qa2 > qa1 && !(qa0 > qa1) &&
                 !(qa0 > qa2) && !(qa1 > qa2)));
    BOOST_CHECK((qa1 > qb0 && qb2 > qa0 && qa2 > qb1 && !(qb0 > qa1) &&
                 !(qa0 > qb2) && !(qb1 > qa2)));
    // greater or equal
    BOOST_CHECK((qa0 <= qa1 && qa0 <= qa2 && qa1 <= qa2 && !(qa1 <= qa0) &&
                 !(qa2 <= qa0) && !(qa2 <= qa1)));
    BOOST_CHECK((qa0 <= qb1 && qb0 <= qa2 && qa1 <= qb2 && !(qb1 <= qa0) &&
                 !(qa2 <= qb0) && !(qb2 <= qa1)));
    BOOST_CHECK((qa0 <= qb0 && qa1 <= qb1 && qa2 <= qb2));
    // less or equal
    BOOST_CHECK((qa1 >= qa0 && qa2 >= qa0 && qa2 >= qa1 && !(qa0 >= qa1) &&
                 !(qa0 >= qa2) && !(qa1 >= qa2)));
    BOOST_CHECK((qa1 >= qb0 && qb2 >= qa0 && qa2 >= qb1 && !(qb0 >= qa1) &&
                 !(qa0 >= qb2) && !(qb1 >= qa2)));
    BOOST_CHECK((qa0 >= qb0 && qa1 >= qb1 && qa2 >= qb2));
  }
  // quantity<Unitless>::operator==()
  // operator==
  // quantity<Unitless>::operator!=()
  // operator!=
  // quantity<Unitless>::operator<()
  // operator<
  // quantity<Unitless>::operator>()
  // operator>
  // quantity<Unitless>::operator>=()
  // operator>=
  // quantity<Unitless>::operator<=()
  // operator<=
  // NOTE: the different factors in u00_type and u01_type makes floating point
  // precision play a role, even for doubles!
  {
    constexpr q00_type qa0 {1};
    constexpr q00_type qa1 {2};
    constexpr q00_type qa2 {3};
    constexpr q01_type qb0 {qa0};
    constexpr q01_type qb1 {qa1};
    constexpr q01_type qb2 {qa2};
    // equal
    BOOST_CHECK((qa0 == qa0.value() && qa1 == qa1.value() &&
                 qa2 == qa2.value() && !(qa0 == qa1) && !(qa0 == qb1)));
    BOOST_CHECK((qa0.value() == qa0 && qa1.value() == qa1 &&
                 qa2.value() == qa2 && !(qa0.value() == qa1.value()) &&
                 !(qa0.value() == qb1)));
    // not equal
    BOOST_CHECK((qa0 != qa1) && (qa0 != qa2) && (qa1 != qa2) && !(qa1 != qa1) &&
                !(qa1 != qa1.value()));
    BOOST_CHECK((qa0.value() != qa1) && (qa0.value() != qa2) &&
                (qa1.value() != qa2) && !(qa1.value() != qa1) &&
                !(qa1.value() != qa1));
    // less than
    BOOST_CHECK((qa0 < qa1 && qa0 < qa2 && qa1 < qa2 && !(qa1 < qa0) &&
                 !(qa2 < qa0) && !(qa2 < qa1)));
    BOOST_CHECK((qa0.value() < qa1 && qa0.value() < qa2 && qa1.value() < qa2 &&
                 !(qa1.value() < qa0) && !(qa2.value() < qa0) &&
                 !(qa2.value() < qa1)));
    BOOST_CHECK((qa0 < qb1 && qb0 < qa2 && qa1 < qb2 && !(qb1 < qa0) &&
                 !(qa2 < qb0) && !(qb2 < qa1)));
    BOOST_CHECK((qa0.value() < qb1 && qb0.value() < qa2 && qa1.value() < qb2 &&
                 !(qb1.value() < qa0) && !(qa2.value() < qb0) &&
                 !(qb2.value() < qa1)));
    // greater than
    BOOST_CHECK((qa1 > qa0 && qa2 > qa0 && qa2 > qa1 && !(qa0 > qa1) &&
                 !(qa0 > qa2) && !(qa1 > qa2)));
    BOOST_CHECK((qa1.value() > qa0 && qa2.value() > qa0 && qa2.value() > qa1 &&
                 !(qa0.value() > qa1) && !(qa0.value() > qa2) &&
                 !(qa1.value() > qa2)));
    BOOST_CHECK((qa1 > qb0 && qb2 > qa0 && qa2 > qb1 && !(qb0 > qa1) &&
                 !(qa0 > qb2) && !(qb1 > qa2)));
    BOOST_CHECK((qa1.value() > qb0 && qb2.value() > qa0 && qa2.value() > qb1 &&
                 !(qb0.value() > qa1) && !(qa0.value() > qb2) &&
                 !(qb1.value() > qa2)));
    // greater or equal
    BOOST_CHECK((qa0 <= qa1 && qa0 <= qa2 && qa1 <= qa2 && !(qa1 <= qa0) &&
                 !(qa2 <= qa0) && !(qa2 <= qa1)));
    BOOST_CHECK((qa0.value() <= qa1 && qa0.value() <= qa2 &&
                 qa1.value() <= qa2 && !(qa1.value() <= qa0) &&
                 !(qa2.value() <= qa0) && !(qa2.value() <= qa1)));
    BOOST_CHECK((qa0 <= qb1 && qb0 <= qa2 && qa1 <= qb2 && !(qb1 <= qa0) &&
                 !(qa2 <= qb0) && !(qb2 <= qa1)));
    BOOST_CHECK((qa0.value() <= qb1 && qb0.value() <= qa2 &&
                 qa1.value() <= qb2 && !(qb1.value() <= qa0) &&
                 !(qa2.value() <= qb0) && !(qb2.value() <= qa1)));
    BOOST_CHECK(
        (qa0 <= qa0.value() && qa1 <= qa1.value() && qa2 <= qa2.value()));
    BOOST_CHECK(
        (qa0.value() <= qa0 && qa1.value() <= qa1 && qa2.value() <= qa2));
    // less or equal
    BOOST_CHECK((qa1 >= qa0 && qa2 >= qa0 && qa2 >= qa1 && !(qa0 >= qa1) &&
                 !(qa0 >= qa2) && !(qa1 >= qa2)));
    BOOST_CHECK((qa1.value() >= qa0 && qa2.value() >= qa0 &&
                 qa2.value() >= qa1 && !(qa0.value() >= qa1) &&
                 !(qa0.value() >= qa2) && !(qa1.value() >= qa2)));
    BOOST_CHECK((qa1 >= qb0 && qb2 >= qa0 && qa2 >= qb1 && !(qb0 >= qa1) &&
                 !(qa0 >= qb2) && !(qb1 >= qa2)));
    BOOST_CHECK((qa1.value() >= qb0 && qb2.value() >= qa0 &&
                 qa2.value() >= qb1 && !(qb0.value() >= qa1) &&
                 !(qa0.value() >= qb2) && !(qb1.value() >= qa2)));
    BOOST_CHECK(
        (qa0 >= qa0.value() && qa1 >= qa1.value() && qa2 >= qa2.value()));
    BOOST_CHECK(
        (qa0.value() >= qa0 && qa1.value() >= qa1 && qa2.value() >= qa2));
  }
  // quantity<>.pow<Ratio>()
  // quantity<>.pow<N, D>()
  // quantity<>.sqrt()
  // quantity<>.cbrt()
  {
    auto qpr = q_mm.pow<std::ratio<2,3>>();
    BOOST_CHECK((physics::unit_string(qpr) == " mm^(2/3)"));
    BOOST_CHECK((qpr.value() == std::pow(q_mm.value(), 2./3)));
    auto qpn = qpr.pow<3,2>();
    BOOST_CHECK((physics::unit_string(qpn) == " mm"));
    BOOST_CHECK((fabs(qpn.value() - q_mm.value()) < 1.e-10));
    auto qs = q_mm.sqrt();
    BOOST_CHECK((physics::unit_string(qs) == " mm^(1/2)"));
    BOOST_CHECK((qs.value() == std::sqrt(q_mm.value())));
    auto qc = q_mm.cbrt();
    BOOST_CHECK((physics::unit_string(qc) == " mm^(1/3)"));
    BOOST_CHECK((qc.value() == std::cbrt(q_mm.value())));
  }
  // pow<Ratio>(quantity<>)
  // pow<N, D>(quantity<>)
  // sqrt(quantity<>)
  // cbrt(quantity<>)
  {
    auto qpr = pow<std::ratio<2, 3>>(q_mm);
    BOOST_CHECK((physics::unit_string(qpr) == " mm^(2/3)"));
    BOOST_CHECK((qpr.value() == std::pow(q_mm.value(), 2./3)));
    auto qpn = pow<3, 2>(qpr);
    BOOST_CHECK((physics::unit_string(qpn) == " mm"));
    BOOST_CHECK((fabs(qpn.value() - q_mm.value()) < 1.e-10));
    auto qpn2 = pow<3>(q_mm);
    BOOST_CHECK((physics::unit_string(qpn2) == " mm^3"));
    BOOST_CHECK((qpn2.value() == std::pow(q_mm.value(), 3.)));
    auto qs = sqrt(q_mm);
    BOOST_CHECK((physics::unit_string(qs) == " mm^(1/2)"));
    BOOST_CHECK((qs.value() == std::sqrt(q_mm.value())));
    auto qc = cbrt(q_mm);
    BOOST_CHECK((physics::unit_string(qc) == " mm^(1/3)"));
    BOOST_CHECK((qc.value() == std::cbrt(q_mm.value())));
  }
  // constexpr-ness of the above functions (integer powers only)
  {
    constexpr auto qpr1 = q_mm.pow<std::ratio<2>>();
    // constexpr auto qpr2 = q_mm.pow<std::ratio<2,3>>(); // ==> should not
                                                          //     compile
    constexpr auto qpr3 = pow<std::ratio<4>>(q_mm);
    // constexpr auto qpr4 = pow<std::ratio<2,3>>(q_mm); // ==> should not
                                                         //     compile
  }
  // quantity<Dimensionless>.pow<Ratio>()
  // quantity<Dimensionless>.pow<N, D>()
  // quantity<Dimensionless>.sqrt()
  // quantity<Dimensionless>.cbrt()
  {
    auto qpr = q00.pow<std::ratio<2,3>>();
    BOOST_CHECK((physics::unit_string(qpr) == " x (10^(4/3) x pi^(2/3))"));
    BOOST_CHECK((qpr.value() == std::pow(q00.value(), 2./3)));
    auto qpn = qpr.pow<3,2>();
    BOOST_CHECK((physics::unit_string(qpn) == " x (10^2 x pi)"));
    BOOST_CHECK((fabs(qpn.value() - q00.value()) < 1.e-10));
    auto qs = q01.sqrt();
    BOOST_CHECK((physics::unit_string(qs) == ""));
    BOOST_CHECK((qs.value() == std::sqrt(q01.value())));
    auto qc = q01.cbrt();
    BOOST_CHECK((physics::unit_string(qc) == ""));
    BOOST_CHECK((qc.value() == std::cbrt(q01.value())));
  }
  // pow<Ratio>(quantity<Dimensionless>)
  // pow<N, D>(quantity<Dimensionless>)
  // sqrt(quantity<Dimensionless>)
  // cbrt(quantity<Dimensionless>)
  {
    auto qpr = pow<std::ratio<2,3>>(q00);
    BOOST_CHECK((physics::unit_string(qpr) == " x (10^(4/3) x pi^(2/3))"));
    BOOST_CHECK((qpr.value() == std::pow(q00.value(), 2./3)));
    auto qpn = pow<3,2>(qpr);
    BOOST_CHECK((physics::unit_string(qpn) == " x (10^2 x pi)"));
    BOOST_CHECK((fabs(qpn.value() - q00.value()) < 1.e-10));
    auto qs = sqrt(q01);
    BOOST_CHECK((physics::unit_string(qs) == ""));
    BOOST_CHECK((qs.value() == std::sqrt(q01.value())));
    auto qc = cbrt(q01);
    BOOST_CHECK((physics::unit_string(qc) == ""));
    BOOST_CHECK((qc.value() == std::cbrt(q01.value())));
  }
}

namespace myunits {
using system = physics::unit_system<mm_name, ns_name>;
using distance_dim = physics::unit_dimensions<std::ratio<1>, std::ratio<0>>;
using m_unit = physics::unit<system, distance_dim, std::ratio<3>>;
namespace distance {
PHYSICS_DEFINE_PREFIX_QUANTITIES(m, m_unit);
}
}
PHYSICS_DEFINE_PREFIX_QUANTITY_NAMES(myunits::distance, m);

BOOST_AUTO_TEST_CASE(test_unit_prefix) {
  // TODO finalize test_unit_prefix
  using namespace myunits;
  distance::m d1 {5};
  distance::mm d2 {d1};
  distance::dam d3 {0.2};
  BOOST_CHECK((d1 == d2));
  BOOST_CHECK((d2 > d3));
//  std::cout << physics::standard_units::constants::c
//            << physics::unit_string(physics::standard_units::constants::c)
//            << std::endl;
//  std::cout << physics::standard_units::constants::electron_mass
//            << physics::unit_string(
//                   physics::standard_units::constants::electron_mass)
//            << std::endl;
  physics::standard_units::mass::kg em =
      physics::standard_units::constants::electron_mass;
//  std::cout << em << physics::unit_string(em) << std::endl;
}
