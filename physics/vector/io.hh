#ifndef PHYSICS_VECTOR_IO_LOADED
#define PHYSICS_VECTOR_IO_LOADED

#include <ostream>
#include <istream>
#include <sstream>
#include <string>

#include <physics/vector/prototype.hh>
#include <physics/unit/io.hh>

// =============================================================================
// I/O stream definitions. 
//
// vector unit can be converted string, uses unit_string<Unit>() internally
//
// vector<> and lorenzvector can be streamed to a ostream and from an istream
// =============================================================================
namespace physics {
// print human readible units
template <class Radian> std::string unit_string(vector<double, Radian>);
template <class Quantity, class Radian>
std::string unit_string(vector<Quantity, Radian>);
template <class Radian> std::string unit_string(lorentzvector<double, Radian>);
template <class Quantity, class Radian>
std::string unit_string(lorentzvector<Quantity, Radian>);
}
// stream vector and lorentzvector
template <class Quantity, class Radian>
std::ostream& operator<<(std::ostream& os,
                         const physics::vector<Quantity, Radian>& v);
template <class Quantity, class Radian>
std::ostream& operator<<(std::ostream& os,
                         const physics::lorentzvector<Quantity, Radian>& v);
template <class Quantity, class Radian>
std::istream& operator>>(std::istream& is,
                         physics::vector<Quantity, Radian>& v);
template <class Quantity, class Radian>
std::istream& operator>>(std::istream& is,
                         physics::lorentzvector<Quantity, Radian>& v);

// =============================================================================
// implementation: vector unit streaming -> forward to unit/io routines
// =============================================================================
namespace physics {
template <class Quantity, class Radian>
std::string unit_string(vector<Quantity, Radian>) {
  return unit_string<typename Quantity::unit>();
}
template <class Radian> std::string unit_string(vector<double, Radian>) {
  return "";
}
template <class Quantity, class Radian>
std::string unit_string(lorentzvector<Quantity, Radian>) {
  return unit_string<typename Quantity::unit>();
}
template <class Radian> std::string unit_string(lorentzvector<double, Radian>) {
  return "";
}
} // physics

// =============================================================================
// implementation: vector I/O
// =============================================================================
template <class Quantity, class Radian>
std::ostream& operator<<(std::ostream& os,
                         const physics::vector<Quantity, Radian>& v) {
  os << "[" << v.x1 << ", " << v.x2 << ", " << v.x3 << "]";
  return os;
}
template <class Quantity, class Radian>
std::ostream& operator<<(std::ostream& os,
                         const physics::lorentzvector<Quantity, Radian>& v){
  os << "(" << v.x0 << ", " << v.x << ")";
  return os;
}
template <class Quantity, class Radian>
std::istream& operator>>(std::istream& is,
                         physics::vector<Quantity, Radian>& v) {
  is >> v.x1 >> v.x2 >> v.x3;
  return is;
}
template <class Quantity, class Radian>
std::istream& operator>>(std::istream& is,
                         physics::lorentzvector<Quantity, Radian>& v) {
  is >> v.x0 >> v.x;
  return is;
}

#endif
