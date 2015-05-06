#ifndef PHYSICS_UTIL_MATH_LOADED
#define PHYSICS_UTIL_MATH_LOADED

#include <cstdint>
#include <ratio>

// Implementatation of basical mathematical routines using constexpr and
// compile-time algorithms
// Implementation of compile-time math is for now using the C++11 constexpr
// without the the C++14 syntax relaxation in order to maximize compiler
// compatibility.

namespace physics {

// =============================================================================
// purely compile-time routines 
// =============================================================================
// constexpr absolute value and sign, as well as std::ratio versions
template <class T> constexpr T abs(T x) { return x < T{0} ? -x : x; }
template <class T> constexpr int sgn(T x) { return x < T{0} ? -1 : 1; }
template <class Ratio> using ratio_abs = std::ratio<abs(Ratio::num), Ratio::den>;
template <class Ratio> using ratio_sgn = std::ratio<sgn(Ratio::num)>;

// generic root, as well as explicit sqrt and cbrt functions
// Uses a binary search implemention (~log_2(n)) based on
// http://baptiste-wicht.com/posts/2014/07/compile-integer-square-roots-at-compile-time-in-cpp.html
template <std::uintmax_t N> constexpr std::intmax_t iroot(std::intmax_t n);
template <> constexpr std::intmax_t iroot<1>(std::intmax_t n);
constexpr std::intmax_t isqrt(std::intmax_t n);
constexpr std::intmax_t icbrt(std::intmax_t n);

// compile time power n^N, uses simple tail recursion
template <std::uintmax_t N> constexpr intmax_t ipow(std::intmax_t n);

// compile time power n^(N/D) combines ipow<N> and iroot<D>
template <class Ratio> constexpr std::intmax_t ipow(std::intmax_t n);
template <uintmax_t N, uintmax_t D>
constexpr std::intmax_t ipow(std::intmax_t n);

// take a std::ratio to the power of another std::ratio (R1^R2),
// as well as ratio_sqrt, ratio_cbrt shortcuts
template <class Ratio1, class Ratio2>
using ratio_pow = std::ratio < (Ratio2::num > 0)
                      ? ipow<ratio_abs<Ratio2>>(Ratio1::num)
                      : ipow<ratio_abs<Ratio2>>(Ratio1::den),
      (Ratio2::num > 0) ? ipow<ratio_abs<Ratio2>>(Ratio1::den)
                        : ipow<ratio_abs<Ratio2>>(Ratio1::num) > ;
template <class Ratio> using ratio_sqrt = ratio_pow<Ratio, std::ratio<1, 2>>;
template <class Ratio> using ratio_cbrt = ratio_pow<Ratio, std::ratio<1, 3>>;

// =============================================================================
// floating point: mixed compile-time/runtime routine
// =============================================================================
// calculate the power of a number using a constexpr compile-time algorithm for
// integer powers, and std::pow for fractional powers
template <class Ratio> constexpr double pow(double x);
template <intmax_t N, intmax_t D = 1> constexpr double pow(double x);

} // namespace physics


// =============================================================================
// implementation: pow x^N
// =============================================================================
namespace physics {
// generic integer recursive power implementation, also useful for e.g. doubles
namespace pow_impl {
template <class T> constexpr T recursive_pow(T x, std::intmax_t n) {
  return (n < 0 ? 1 / recursive_pow(x, -n)
                : (n == 0 ? 1 : x * recursive_pow(x, n - 1)));
}
} // namespace pow_impl
template <std::uintmax_t N> constexpr std::intmax_t ipow(std::intmax_t n) {
  return pow_impl::recursive_pow(n, N);
}
} // namespace physics


// =============================================================================
// implementation: iroot(n) using a binary search algorithm
// =============================================================================
namespace physics {
namespace iroot_impl {
constexpr std::intmax_t mid(std::intmax_t a, std::intmax_t b) {
  return (a + b) / 2;
}
template <std::uintmax_t N>
constexpr std::intmax_t iroot(std::intmax_t n, std::intmax_t left,
                              std::intmax_t right) {
  return (left == right)
             ? right
             : iroot<N>(
                   n,
                   ipow<N>(mid(left, right)) >= n ? left : mid(left, right) + 1,
                   ipow<N>(mid(left, right)) >= n ? mid(left, right) : right);
}
} // namespace iroot_impl
template <std::uintmax_t N> constexpr std::intmax_t iroot(std::intmax_t n) {
  static_assert(N != 0, "Cannot take degree 0 root.");
  return iroot_impl::iroot<N>(n, 1, n);
}
template <> constexpr std::intmax_t iroot<1>(std::intmax_t n) { return n; }

constexpr std::intmax_t isqrt(std::intmax_t n) { return iroot<2>(n); }
constexpr std::intmax_t icbrt(std::intmax_t n) { return iroot<3>(n); }
} // namespace physics

// =============================================================================
// implementation: pow x^(N/D)
// =============================================================================
namespace physics {
template <class Ratio> constexpr std::intmax_t ipow(std::intmax_t n) {
  return (Ratio::num >= 0) ? ipow<Ratio::num>(iroot<Ratio::den>(n)) : 0;
}
template <std::uintmax_t N, std::uintmax_t D>
constexpr std::intmax_t ipow(std::intmax_t n) {
  return ipow<std::ratio<N, D>>(n);
}
// =============================================================================
// implementation: pow(x)
// =============================================================================
namespace pow_impl {
// using a struct with ::call member to allow for partial specialization into
// integer an integer and non-integer power case. The former can be calculated
// with compile-time recursion (constexpr), while the latter requires a std::pow
// call (runtime)
// The struct has a third dummy template argument to allow the generic ::call
// case to be constexpr, needed to allow for use of pow in constexpr functions
template <std::intmax_t N, std::intmax_t D, class Dummy = void> struct pow {
  constexpr static double call(double x);
};
// fractional powers: use builtin std::pow (no constexpr)
template <std::intmax_t N, std::intmax_t D> struct pow<N, D, void> {
  static double call(double x) { return std::pow(x, double(N) / D); }
};
// use standard sqrt/cbrt when applicable
template <> struct pow<1, 2, void> {
  static double call(double x) { return sqrt(x); }
};
template <> struct pow<1, 3, void> {
  static double call(double x) { return cbrt(x); }
};
// integer powers: use recursive implementation
template <std::intmax_t N> struct pow<N, 1, void> {
  constexpr static double call(double x) { return recursive_pow(x, N); }
};
} // namespace pow_impl
template <class Ratio> constexpr double pow(double x) {
  return pow_impl::pow<Ratio::num, Ratio::den>::call(x);
}
template <intmax_t N, intmax_t D> constexpr double pow(double x) {
  return pow_impl::pow<N, D>::call(x);
}

} // namespace physics
#endif
