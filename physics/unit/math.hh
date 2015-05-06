#ifndef PHYSICS_UNIT_MATH_LOADED
#define PHYSICS_UNIT_MATH_LOADED

#include <ratio>

#include <physics/unit/prototype.hh>
#include <physics/util/math.hh>

// utility structs to implement compile-time unit multiplication/division
// and other utility mathematical functions for quantity rescaling
// as well as optimized power calculations, retaining constexpr-ness where
// possible

namespace physics {
namespace unit_impl {

// dimensions_add<> and dimensions_subtract
template <class Dim1, class Dim2> struct dimensions_add_impl;
template <class Dim1, class Dim2> struct dimensions_subtract_impl;
template <class Dim1, class Dim2>
using dimensions_add = typename dimensions_add_impl<Dim1, Dim2>::type;
template <class Dim1, class Dim2>
using dimensions_subtract = typename dimensions_subtract_impl<Dim1, Dim2>::type;

// dimensions_multiply and dimensions_divide
template <class Dim, class Ratio> struct dimensions_multiply_impl;
template <class Dim, class Ratio>
using dimensions_divide_impl =
    dimensions_multiply_impl<Dim, std::ratio_divide<std::ratio<1>, Ratio>>;
template <class Dim, class Ratio>
using dimensions_multiply = typename dimensions_multiply_impl<Dim, Ratio>::type;
template <class Dim, class Ratio>
using dimensions_divide = typename dimensions_divide_impl<Dim, Ratio>::type;

// rescale the RHS value to match the unit of the LHS value
template <class UnitLHS, class UnitRHS> constexpr double rescale_value(double
rhs_value);

// maximum accuracy pi constant, to avoid dependence on boost::constants, or
// non-standard compiler features
constexpr const double
pi{3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651e+00};

} // namespace unit_impl
} // namespace physics

// =============================================================================
// implementation: add/subtract arithmetic
// =============================================================================
namespace physics {
namespace unit_impl {
template <std::intmax_t... N1, std::intmax_t... D1, std::intmax_t... N2,
          std::intmax_t... D2>
constexpr auto dimensions_add_func(unit_dimensions<std::ratio<N1, D1>...>,
                                   unit_dimensions<std::ratio<N2, D2>...>)
    -> unit_dimensions<
          std::ratio_add<std::ratio<N1, D1>, std::ratio<N2, D2>>...> {
  return {};
}
template <std::intmax_t... N1, std::intmax_t... D1, std::intmax_t... N2,
          std::intmax_t... D2>
constexpr auto dimensions_subtract_func(unit_dimensions<std::ratio<N1, D1>...>,
                                        unit_dimensions<std::ratio<N2, D2>...>)
    -> unit_dimensions<
          std::ratio_subtract<std::ratio<N1, D1>, std::ratio<N2, D2>>...> {
  return {};
}
template <class Dim1, class Dim2> struct dimensions_add_impl {
  using type = decltype(dimensions_add_func(Dim1{}, Dim2{}));
};
template <class Dim1, class Dim2> struct dimensions_subtract_impl {
  using type = decltype(dimensions_subtract_func(Dim1{}, Dim2{}));
};
} // namespace unit_impl
} // namespace physics

// =============================================================================
// implementation: multiplication arithmetic
// =============================================================================
namespace physics {
namespace unit_impl {
template <std::intmax_t... N, std::intmax_t... D, std::intmax_t Fnum,
          std::intmax_t Fden>
constexpr auto dimensions_multiply_func(unit_dimensions<std::ratio<N, D>...>,
                                        std::ratio<Fnum, Fden>)
    -> unit_dimensions<
          std::ratio_multiply<std::ratio<N, D>, std::ratio<Fnum, Fden>>...> {
  return {};
}
template <class Dim, class Ratio> struct dimensions_multiply_impl {
  using type = decltype(dimensions_multiply_func(Dim{}, Ratio{}));
};
} // namespace unit_impl
} // namespace physics

// =============================================================================
// implementation: rescale_value 
// =============================================================================
namespace physics {
namespace unit_impl {
template <class UnitLHS, class UnitRHS> struct rescale_impl {
  constexpr static double pow_10() {
    using delta_pow_10 =
        std::ratio_subtract<typename UnitRHS::pow_10, typename UnitLHS::pow_10>;
    return pow<delta_pow_10>(10.);
  }
  constexpr static double pow_pi() {
    using delta_pow_pi =
        std::ratio_subtract<typename UnitRHS::pow_pi, typename UnitLHS::pow_pi>;
    return pow<delta_pow_pi>(physics::unit_impl::pi);
  }
  constexpr static double factor() {
    using factor =
        std::ratio_divide<typename UnitRHS::factor, typename UnitLHS::factor>;
    return double(factor::num) / factor::den;
  }
};
// no-op specialization when no rescaling is needed
template <class Unit> struct rescale_impl<Unit, Unit> {
  constexpr static std::intmax_t pow_10() { return 1; }
  constexpr static std::intmax_t pow_pi() { return 1; }
  constexpr static std::intmax_t factor() { return 1; }
};
template <class UnitLHS, class UnitRHS> constexpr double rescale_value(double
rhs_value) {
  return rhs_value * rescale_impl<UnitLHS, UnitRHS>::pow_10() *
         rescale_impl<UnitLHS, UnitRHS>::pow_pi() *
         rescale_impl<UnitLHS, UnitRHS>::factor();
}

}
}
#endif
