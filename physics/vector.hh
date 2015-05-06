#ifndef PHYSICS_VECTOR_LOADED
#define PHYSICS_VECTOR_LOADED

#include <cmath>
#include <physics/unit.hh>
#include <physics/vector/io.hh>

namespace physics {

// =============================================================================
// definition: physics::vector
//
// a 3-vector that supports both physics::quantity and raw doubles
// =============================================================================
template <class Quantity, class Radian = double> struct vector {
  using quantity_type = Quantity;
  using radian_type = Radian;

  quantity_type x1;
  quantity_type x2;
  quantity_type x3;

  constexpr vector() : x1{0}, x2{0}, x3{0} {}
  explicit constexpr vector(quantity_type x123)
      : x1{x123}, x2{x123}, x3{x123} {}
  // cartesian coordinate constructor
  constexpr vector(quantity_type y1, quantity_type y2, quantity_type y3)
      : x1{y1}, x2{y2}, x3{y3} {}
  // spherical coordinate constructor
  // (SFINAE disabled if Quantity=Radian=double)
  template <class RadianType = radian_type,
            typename std::enable_if<
                !std::is_same<Quantity, RadianType>::value &&
                std::is_same<Radian, RadianType>::value>::type* Dummy = nullptr>
  vector(quantity_type r, RadianType theta, RadianType phi)
      : x1{r * sin(theta) * cos(phi)}
      , x2{r * sin(theta) * sin(phi)}
      , x3{r * cos(theta)} {}
  // cilindrical coordinate constructor
  // (SFINAE disabled if Quantity=Radian=double)
  template <class RadianType = radian_type,
            typename std::enable_if<
                !std::is_same<Quantity, RadianType>::value &&
                std::is_same<Radian, RadianType>::value>::type* Dummy = nullptr>
  vector(quantity_type r, RadianType theta, quantity_type y3)
      : x1{r * cos(theta)}, x2{r * sin(theta)}, x3{y3} {}

  // construction and assignment from a compatible vector
  template <class Q2, class R2>
  constexpr vector(const vector<Q2, R2>& v)
      : x1{v.x1}, x2{v.x2}, x3{v.x3} {}
  template <class Q2, class R2> vector& operator=(const vector<Q2, R2>& v) {
    x1 = v.x1;
    x2 = v.x2;
    x3 = v.x3;
    return *this;
  }
  // comparison with compatible vector
  template <class Q2, class R2>
  constexpr bool operator==(const vector<Q2, R2>& v) const {
    return (x1 == v.x1 && x2 == v.x2 && x3 == v.x3);
  }
  template <class Q2, class R2>
  constexpr bool operator!=(const vector<Q2, R2>& v) const {
    return !(*this == v);
  }
  //
  // vector arithmetic
  //
  // add and subtract vectors (units dimensions need to be compatible)
  constexpr auto operator+(const vector& v) const {
    return vector{x1 + v.x1, x2 + v.x2, x3 + v.x3};
  }
  constexpr auto operator-(const vector& v) const {
    return vector{x1 - v.x1, x2 - v.x2, x3 - v.x3};
  }
  // multiply/divide vector by scalar
  template <class Q2> constexpr auto operator*(Q2 q) const {
    return vector<decltype(x1* q), radian_type>{x1 * q, x2 * q, x3 * q};
  }
  template <class Q2> constexpr auto operator/(Q2 q) const {
    return vector<decltype(x1 / q), radian_type>{x1 / q, x2 / q, x3 / q};
  }
  // scalar product (unit dimensions can differ)
  template <class Q2, class R2>
  constexpr auto operator*(const vector<Q2, R2>& v) const {
    return x1 * v.x1 + x2 * v.x2 + x3 * v.x3;
  }
  // cross product (unit dimensions can differ)
  template <class Q2, class R2>
  constexpr auto operator^(const vector<Q2, R2>& v) const {
    return vector<decltype(x1* v.x1), radian_type>{
        x2 * v.x3 - x3 * v.x2, x3 * v.x1 - x1 * v.x3, x1 * v.x2 - x2 * v.x1};
  }
  //
  // arithmetic assignment
  //
  vector& operator+=(vector v) {
    *this = (*this + v);
    return *this;
  }
  vector& operator-=(vector v) {
    *this = *this - v;
    return *this;
  }
  // multiplication and division-assigment only by doubles (and dimensionless
  // quantities)
  vector& operator*=(double d) {
    *this = *this * d;
    return *this;
  }
  vector& operator/=(double d) {
    *this = *this / d;
    return *this;
  }
  // magnitude squared
  constexpr auto mag2() const { return *this * *this; }
  // magnitude
  auto mag() const { return std::sqrt(mag2()); }
};
// vector magnitude
template <class Quantity, class Radian>
constexpr auto mag2(const vector<Quantity, Radian>& v) {
  return v.mag2();
}
template <class Quantity, class Radian>
constexpr auto mag(const vector<Quantity, Radian>& v) {
  return v.mag();
}
} // namespace physics
// global vector multiplication operators with quantities and doubles
// a little more explicit to prevent the operators from being too flexible
template <class Unit, class Quantity, class Radian>
constexpr auto operator*(physics::quantity<Unit> q,
                         const physics::vector<Quantity, Radian>& v) {
  return v * q;
}
template <class Quantity, class Radian>
constexpr auto operator*(double d, const physics::vector<Quantity, Radian>& v) {
  return v * d;
}

// =============================================================================
// definition: physics::lorentzvector
//
// an efficient 4-vector that supports both physics::quantity and raw doubles
// =============================================================================
namespace physics {
template <class Quantity, class Radian = double> struct lorentzvector {
  using quantity_type = Quantity;
  using radian_type = Radian;
  using vector_type = vector<Quantity, Radian>;

  quantity_type x0;
  vector_type x;

  constexpr lorentzvector() : x0{quantity_type{0}}, x{quantity_type{0}} {}
  explicit constexpr lorentzvector(quantity_type x0123) : x0{x0123}, x{x0123} {}
  constexpr lorentzvector(quantity_type y0, const vector_type& y)
      : x0{y0}, x{y} {}

  // construction and assignment from a compatible lorentzvector
  template <class Q2, class R2>
  constexpr lorentzvector(const lorentzvector<Q2, R2>& v)
      : x0{v.x0}, x{v.x} {}
  template <class Q2, class R2>
  lorentzvector& operator=(const lorentzvector<Q2, R2>& v) {
    x0 = v.x0;
    x = v.x;
    return *this;
  }
  // comparison with compatible lorentzvector
  template <class Q2, class R2>
  constexpr bool operator==(const lorentzvector<Q2, R2>& v) const {
    return (x0 == v.x0 && x == v.x);
  }
  template <class Q2, class R2>
  constexpr bool operator!=(const lorentzvector<Q2, R2>& v) const {
    return !(*this == v);
  }
  //
  // lorentzvector arithmetic
  //
  // add and subtract lorentzvectors (unit dimensions need to be compatible)
  constexpr auto operator+(const lorentzvector& v) const {
    return lorentzvector{x0 + v.x0, x + v.x};
  }
  constexpr auto operator-(const lorentzvector& v) const {
    return lorentzvector{x0 - v.x0, x - v.x};
  }
  // multiply/divide vector by scalar
  template <class Q2> constexpr auto operator*(Q2 q) const {
    return lorentzvector<decltype(x0* q), radian_type>{x0 * q, x * q};
  }
  template <class Q2> constexpr auto operator/(Q2 q) const {
    return lorentzvector<decltype(x0 / q), radian_type>{x0 / q, x / q};
  }
  // scalar product (unit dimensions can differ)
  template <class Q2, class R2>
  constexpr auto operator*(const lorentzvector<Q2, R2>& v) const {
    return x0 * v.x0 - x * v.x;
  }
  //
  // arithmetic assignment
  //
  lorentzvector& operator+=(const lorentzvector& v) {
    *this = *this + v;
    return *this;
  }
  lorentzvector& operator-=(const lorentzvector& v) {
    *this = *this - v;
    return *this;
  }
  // multiplication and division-assignment only by doubles (and dimensionless
  // quantities)
  lorentzvector& operator*=(double d) {
    *this = *this * d;
    return *this;
  }
  lorentzvector& operator/=(double d) {
    *this = *this / d;
    return *this;
  }
  // magnitude squared
  constexpr auto mag2() const { return *this * *this; }
  // magnitude; similar to ROOT equal to a negative number if it should have
  // been a complex number (i.e., the magnitude squared is negative)
  constexpr auto mag() const {
    using mag2_type = decltype(mag2());
    return (mag2() >= mag2_type{0}) ? std::sqrt(mag2()) : -std::sqrt(-mag2());
  }
  // the relative velocity beta=v/c of this particle
  constexpr auto beta() const { return x / x0; }
  // lorenz boost, transforming (x0, x) in O -> (x0', x') in O', where O' moves
  // relative to the O frame with relative velocity beta=v/c
  template <class Q2, class R2>
  lorentzvector boost(const vector<Q2, R2>& beta) const {
    double gamma{1. / std::sqrt(1. - beta.mag2())};
    auto y = x + beta * ((gamma - 1) / beta.mag2() * (x * beta) - gamma * x0);
    auto y0 = gamma * (x0 - beta * x);
    return {y0, y};
  }
};
// vector magnitude
template <class Quantity, class Radian>
constexpr auto mag2(const lorentzvector<Quantity, Radian>& v) {
  return v.mag2();
}
template <class Quantity, class Radian>
constexpr auto mag(const lorentzvector<Quantity, Radian>& v) {
  return v.mag();
}
template <class Quantity, class Radian>
constexpr auto beta(const lorentzvector<Quantity, Radian>& v) {
  return v.beta();
}
template <class Q1, class R1, class Q2, class R2>
auto boost(const lorentzvector<Q1, R1>& v, const vector<Q2, R2>& beta) {
  return v.boost(beta);
}
} // namespace physics
// global lorentzvector multiplication operators with quantities and doubles.
// Similar as for the vector case, the definitions are explicit to prevent too
// much flexibility of the operator
template <class Unit, class Quantity, class Radian>
constexpr auto operator*(physics::quantity<Unit> q,
                         const physics::lorentzvector<Quantity, Radian>& v) {
  return v * q;
}
template <class Quantity, class Radian>
constexpr auto operator*(double d,
                         const physics::lorentzvector<Quantity, Radian>& v) {
  return v * d;
}

#endif
