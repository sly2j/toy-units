#ifndef PHYSICS_UTIL_MIXIN_LOADED
#define PHYSICS_UTIL_MIXIN_LOADED

// set of CRTP mixins-from-above to flesh out a class with
// additional functionality

namespace physics {

// add !=, <=, > and >= for to a class that defines == and <
template <class T, class Rhs = T> class comparison_mixin {
public:
  constexpr bool operator!=(const Rhs& rhs) const { return !(derived() == rhs); }
  constexpr bool operator<=(const Rhs& rhs) const {
    return (derived() == rhs || derived() < rhs);
  }
  constexpr bool operator>(const Rhs& rhs) const { return !(derived() <= rhs); }
  constexpr bool operator>=(const Rhs& rhs) const { return !(derived() < rhs); }

private:
  constexpr const T& derived() const { return static_cast<const T&>(*this); }
};

// add operator-,+,*,/ to classes defining operator+=, -=, *=, /=
template <class T> class arithmetic_mixin {
public:
  template <class Rhs> T operator-(const Rhs& rhs) const {
    T ret{derived()};
    return ret -= rhs;
  }
  template <class Rhs> T operator+(const Rhs& rhs) const {
    T ret{derived()};
    return ret += rhs;
  }
  template <class Rhs> T operator/(const Rhs& rhs) const {
    T ret{derived()};
    return ret /= rhs;
  }
  template <class Rhs> T operator*(const Rhs& rhs) const {
    T ret{derived()};
    return ret *= rhs;
  }

private:
  T& derived() { return static_cast<T&>(*this); }
  const T& derived() const { return static_cast<const T&>(*this); }
};

} // namespace

#endif
