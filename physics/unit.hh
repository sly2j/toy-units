#ifndef PHYSICS_UNIT_LOADED
#define PHYSICS_UNIT_LOADED

#include <ratio>
#include <cmath>

#include <physics/unit/detail.hh>
#include <physics/unit/type_traits.hh>
#include <physics/unit/math.hh>
#include <physics/unit/io.hh>
#include <physics/unit/prototype.hh>

#include <physics/util/math.hh>
#include <physics/util/mixin.hh>

namespace physics {

// =============================================================================
// unit_dimensions: described by a set of std::ratio<> types
// =============================================================================
template <class... Dimensions> struct unit_dimensions {
  static constexpr auto size = sizeof...(Dimensions);
  // compile-time check
  static_assert(unit_impl::are_valid_ratios<Dimensions...>::value,
                "Dimensions each need to be a valid std::ratio<> type.");
};
template <intmax_t... Dimensions>
using make_unit_dimensions = unit_dimensions<std::ratio<Dimensions>...>;

// =============================================================================
// unit_system: a set of orthogonal units
// =============================================================================
template <const char* const&... Names> struct unit_system {
  static constexpr auto size = sizeof...(Names);
  using dimensionless = typename unit_impl::make_dimensionless<size>::type;
};

// =============================================================================
// unit<System, Dimensions, Pow10, PowPi, Factor>: 
// A unit, with:
//  * unit_system System 
//  * unit_dimensions Dimensions
// (optional):
//  * Pow10, PiPower and Factor, where the unit is related to the base unit by
//      <unit> = Factor * 10^DecimalPower * pi^PiPower * <base_unit>.
//
// Notes:
//  * that Pow10, PowPi and Factor should be a std::ratio<> type. This way,
//    they will be guaranteed to be handled compile-time.
//  * The use of Pow10, PowPi and Factor separately, as opposed to using just a
//    factor has the advantage of being more robust against integer overflows
//    when taking powers of units.
//  * Invalid unit definitions will be caught compile-time.
// =============================================================================
template <class System, class Dimensions, class Pow10 = std::ratio<0>,
          class PowPi = std::ratio<0>, class Factor = std::ratio<1>>
struct unit {
  using system = System;
  using dimensions = Dimensions;
  using pow_10 = Pow10;
  using pow_pi = PowPi;
  using factor = Factor;
  // compile-time checks
  static_assert(unit_impl::is_unit_system<System>::value,
                "System must be a valid unit_system<>.");
  static_assert(unit_impl::is_unit_dimensions<Dimensions>::value,
                "Dimensions must be a valid unit_dimensions<>.");
  static_assert(System::size == Dimensions::size,
                "System and Dimensions must share the same dimensionality.");
  static_assert(
      unit_impl::are_valid_ratios<Pow10, PowPi, Factor>::value,
      "Pow10, PowPi and Factor must be a valid std::ratio<>.");
};

// compatible units (from same unit system) can be multiplied and divided
template <class Unit1, class Unit2>
using unit_multiply = unit<
    typename std::enable_if<unit_impl::have_same_system<Unit1, Unit2>::value,
                            typename Unit1::system>::type,
    unit_impl::dimensions_add<typename Unit1::dimensions,
                              typename Unit2::dimensions>,
    std::ratio_add<typename Unit1::pow_10, typename Unit2::pow_10>,
    std::ratio_add<typename Unit1::pow_pi, typename Unit2::pow_pi>,
    std::ratio_multiply<typename Unit1::factor, typename Unit2::factor>>;
template <class Unit1, class Unit2>
using unit_divide = unit<
    typename std::enable_if<unit_impl::have_same_system<Unit1, Unit2>::value,
                            typename Unit1::system>::type,
    unit_impl::dimensions_subtract<typename Unit1::dimensions,
                                   typename Unit2::dimensions>,
    std::ratio_subtract<typename Unit1::pow_10, typename Unit2::pow_10>,
    std::ratio_subtract<typename Unit1::pow_pi, typename Unit2::pow_pi>,
    std::ratio_divide<typename Unit1::factor, typename Unit2::factor>>;
// a unit can be taken to a rational power Unit^Ratio
// also defines unit_sqrt<> and unit_cbrt aliases
template <class Unit, class Ratio>
using unit_pow =
    unit<typename Unit::system,
         unit_impl::dimensions_multiply<typename Unit::dimensions, Ratio>,
         std::ratio_multiply<typename Unit::pow_10, Ratio>,
         std::ratio_multiply<typename Unit::pow_pi, Ratio>,
         ratio_pow<typename Unit::factor, Ratio>>;
template <class Unit> using unit_sqrt = unit_pow<Unit, std::ratio<1, 2>>;
template <class Unit> using unit_cbrt = unit_pow<Unit, std::ratio<1, 3>>;

// =============================================================================
// quantity<Unit>: a physics quantity
// 
// Notes: 
//  * In the member functions below I typically capture quantity by value,
//    rather than constant reference. This does not impact performance as,
//    ultimately, quantity is just a type-rich wrapper around a double value.
//  * The optimized specialization for dimensionless units automatically
//    applies the different scaling factors to allow for a more intuitive
//    raw-double-like behavior.
//  * When taking a power/root of quantity, the resulting unit will have a
//    unit::factor of 1, with the original factor directly applied to the value.
//    This is done to avoid loss of precision when taking roots of a
//    rational number. This does not apply for the unit::pow_10 and unit::pow_pi
//    values.
//
// =============================================================================
template <class Unit> class quantity : public comparison_mixin<quantity<Unit>> {
public:
  using unit = Unit;
  static_assert(unit_impl::is_unit<Unit>::value,
                "Unit must be a valid physics::unit<>.");

  // constructors
  //
  // 1. default constructor
  constexpr quantity() : value_{0.} {}
  // 2. from double (explicit!)
  constexpr explicit quantity(double v) : value_{v} {}
  // 3. from other compatible quantity
  template <class Pow10, class PowPi, class Factor>
  constexpr quantity(
      quantity<physics::unit<typename unit::system, typename unit::dimensions,
                             Pow10, PowPi, Factor>> rhs)
      : value_{unit_impl::rescale_value<unit, typename decltype(rhs)::unit>(
            rhs.raw_value())} {}

  // assignment
  quantity& operator=(quantity rhs) {
    value_ = rhs.value_;
    return *this;
  }

  // extract the numerical value of a quantity
  constexpr double value() const { return value_; }
  // raw_value() returns the value_ property for all quantities, while the
  // behavior of value() differes for dimensionless quantities. (See the
  // dimensionless quantity specialization below for more info.)
  constexpr double raw_value() const { return value_; }

  // extract the numeric value of the quantity in another unit (or quantity with
  // compatible unit)
  template <class UnitOrQuantity> constexpr double value() const {
    using other_unit = typename std::conditional<
        physics::unit_impl::is_quantity<UnitOrQuantity>::value,
        typename UnitOrQuantity::unit, UnitOrQuantity>::type;
    static_assert(unit_impl::are_compatible<unit, other_unit>::value,
                  "Attempting to convert quantity to incompatible unit");
    return quantity<other_unit>{*this}.value();
  }

  // multiply and divide quantities
  // using raw_value to obtain the values of the second term to correctly handle
  // dimensionless quantities
  template <class Unit2> constexpr auto operator*(quantity<Unit2> q) const {
    return quantity<unit_multiply<unit, Unit2>>{value_ * q.raw_value()};
  }
  template <class Unit2> constexpr auto operator/(quantity<Unit2> q) const {
    return quantity<unit_divide<unit, Unit2>>{value_ / q.raw_value()};
  }
  constexpr auto operator*(double d) const { return quantity{value_ * d}; }
  constexpr auto operator/(double d) const { return quantity{value_ / d}; }
  // add and subtract quantities
  constexpr auto operator+(quantity q) const { return quantity{value_ + q.value_}; }
  constexpr auto operator-(quantity q) const { return quantity{value_ - q.value_}; }
  // unary minus sign
  constexpr auto operator-() const { return quantity{-value_}; }

  // power and sqrt/cbrt
  // the scaling factor is automatically applied to avoid losing precision
  // when taking roots of rational numbers (the scale factor is rational)
  template <class Ratio> constexpr auto pow() const {
    using unit_scaled =
        physics::unit<typename unit::system, typename unit::dimensions,
                      typename unit::pow_10, typename unit::pow_pi>;
    return quantity<unit_pow<unit_scaled, Ratio>>{
        physics::pow<Ratio>(value_ * unit::factor::num / unit::factor::den)};
  }
  template <std::intmax_t N, std::intmax_t D = 1> constexpr auto pow() const {
    return pow<std::ratio<N, D>>();
  }
  auto sqrt() const { return pow<1,2>(); }
  auto cbrt() const { return pow<1,3>(); }
  
  // *=, /=, += and -= assignment
  quantity& operator*=(double d) {
    value_ *= d;
    return *this;
  }
  quantity& operator/=(double d) {
    value_ /= d;
    return *this;
  }
  quantity& operator+=(quantity q) {
    value_ += q.value_;
    return *this;
  }
  quantity& operator-=(quantity q) {
    value_ -= q.value_;
    return *this;
  }
  // compare units (be careful with the intrinsic unit conversion, as floating
  // point math is not exact!)
  // !=, >, => and <= are defined through comparison_mixin inheritance
  constexpr bool operator==(quantity q) const { return value_ == q.value_; }
  constexpr bool operator<(quantity q) const { return value_ < q.value_; }

private:
  double value_;
}; // quantity<>

// =============================================================================
// quantity specialization: dimensionless case 
//  * behaves as if it is a raw double
//  * implicit cast to double
// =============================================================================
template <class System, class Pow10, class PowPi, class Factor>
class quantity<
    unit<System, typename System::dimensionless, Pow10, PowPi, Factor>>
    : public comparison_mixin<quantity<
          unit<System, typename System::dimensionless, Pow10, PowPi, Factor>>> {
public:
  using unit = physics::unit<System, typename System::dimensionless, Pow10,
                             PowPi, Factor>;

  // constructors
  //
  // 1. from double (explicit!)
  constexpr explicit quantity(double v) : value_{v} {}
  // 2. from other compatible quantity (optimized)
  template <class U2Pow10, class U2PowPi, class U2Factor>
  constexpr quantity(
      quantity<physics::unit<typename unit::system, typename unit::dimensions,
                             U2Pow10, U2PowPi, U2Factor>> rhs)
      : value_{unit_impl::rescale_value<unit, typename decltype(rhs)::unit>(
            rhs.raw_value())} {}

  // assignment
  quantity& operator=(quantity rhs) {
    value_ = rhs.value_;
    return *this;
  }
  // (implicit) conversion to double
  constexpr operator double() const { return value(); }

  // extract the numerical value, automatically performing the necessary
  // rescaling steps in order to emulate the behavior of a raw double
  constexpr double value() const {
    return quantity<
               physics::unit<typename unit::system, typename unit::dimensions>>{
        *this}.raw_value();
  }
  // return the raw internal value, proportional to value() through Pow10, PowPi
  // and Factor
  constexpr double raw_value() const { return value_; }

  // multiply and divide
  template <class Unit2>
  constexpr auto operator*(quantity<Unit2> q) const {
    return quantity<unit_multiply<unit, Unit2>>{value_ * q.raw_value()};
  }
  template <class Unit2>
  constexpr auto operator/(quantity<Unit2> q) const {
    return quantity<unit_divide<unit, Unit2>>{value_ / q.raw_value()};
  }
  constexpr auto operator*(double d) const { return quantity{value_ * d}; }
  constexpr auto operator/(double d) const { return quantity{value_ / d}; }
  // add and subtract from quantity and raw double
  constexpr auto operator+(double d) const {
    return quantity{
        raw_value() +
        unit_impl::rescale_value<
            unit, physics::unit<typename unit::system,
                                typename unit::system::dimensionless>>(d)};
  }
  constexpr auto operator-(double d) const {
    return quantity{
        raw_value() -
        unit_impl::rescale_value<
            unit, physics::unit<typename unit::system,
                                typename unit::system::dimensionless>>(d)};
  }
  // unary minus sign
  constexpr auto operator-() const { return quantity{-value_}; }
  
  // power and sqrt/cbrt
  // the scaling factor is automatically applied to avoid losing precision
  // when taking roots of rational numbers (the scale factor is rational)
  template <class Ratio> constexpr auto pow() const {
    using unit_scaled =
        physics::unit<typename unit::system, typename unit::dimensions,
                      typename unit::pow_10, typename unit::pow_pi>;
    return quantity<unit_pow<unit_scaled, Ratio>>{
        physics::pow<Ratio>(value_ * unit::factor::num / unit::factor::den)};
  }
  template <std::intmax_t N, std::intmax_t D = 1> constexpr auto pow() const {
    return pow<std::ratio<N, D>>();
  }
  auto sqrt() const { return pow<1,2>(); }
  auto cbrt() const { return pow<1,3>(); }

  // *=, /=, += and -= assigment
  quantity& operator*=(double d) {
    value_ *= d;
    return *this;
  }
  quantity& operator/=(double d) {
    value_ /= d;
    return *this;
  }
  quantity& operator+=(double d) {
    value_ = (*this + d).value_;
    return *this;
  }
  quantity& operator-=(double d) {
    value_ = (*this - d).value_;
    return *this;
  }
  // compare quantityes; !=, >, => and <= are defined through the
  // comparison_mixing
  constexpr bool operator==(double d) const { return value() == d; }
  constexpr bool operator<(double d) const { return value() < d; }

private:
  double value_;
};

// powers, sqrt and fabs for quantities
  template <std::intmax_t N, std::intmax_t D, class Unit>
  constexpr auto pow(quantity<Unit> q) {
    return q.template pow<N, D>();
  }
  template <std::intmax_t N, class Unit>
  constexpr auto pow(quantity<Unit> q) {
    return q.template pow<N>();
  }
  template <class Ratio, class Unit>
  constexpr auto pow(quantity<Unit> q) {
    return q.template pow<Ratio>();
  }

  template <class Unit> constexpr auto sqrt(quantity<Unit> q) {
    return q.sqrt();
  }
  template <class Unit> constexpr auto cbrt(quantity<Unit> q) {
    return q.cbrt();
  }
  template <class Unit> constexpr auto fabs(quantity<Unit> q) { return abs(q); }
} // namespace physics;

// allow sqrt and cbrt to be used in the global and std namespace
#ifndef PHYSICS_DISABLE_GLOBAL_TYPEDEFS
using physics::pow;
using physics::sqrt;
using physics::cbrt;
using physics::fabs;
using physics::sgn; // from util/math.h
#endif
namespace std {
using physics::pow;
using physics::sqrt;
using physics::cbrt;
using physics::fabs;
using physics::sgn;
}

// multiply and divide doubles and quantities, add and subtract doubles and
// dimensionless quantities
template <class Unit>
constexpr auto operator*(double d, physics::quantity<Unit> q) {
  return q * d;
}
template <class Unit>
constexpr auto operator/(double d, physics::quantity<Unit> q) {
  return physics::quantity<physics::unit_pow<Unit, std::ratio<-1>>>{
      d / q.raw_value()};
}

#endif
