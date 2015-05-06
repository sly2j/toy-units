#ifndef PHYSICS_UNIT_IO_LOADED
#define PHYSICS_UNIT_IO_LOADED

#include <ostream>
#include <istream>
#include <sstream>
#include <string>

#include <physics/unit/prototype.hh>

// =============================================================================
// I/O stream definitions. 
//
// unit can be converted string,
// in order to override the default unit name, simply specialize the
// unit_string<Unit>() template (or use the PHYSICS_UNIT_SET_NAME(QUANTITY,
// NAME) shortcut)
//
// and quantity<> can be streamed to a ostream,
// and an istream can be streamed to a quantity.
// =============================================================================
namespace physics {

// print human readible unit
template <class Unit> std::string unit_string();
template <class Quantity> std::string unit_string(Quantity) {
  return unit_string<typename Quantity::unit>();
}
}
#define PHYSICS_DEFINE_UNIT_NAME(UNIT, NAME)                                   \
  template <> inline std::string physics::unit_string<UNIT>() {                \
    return " " NAME;                                                           \
  }
#define PHYSICS_DEFINE_QUANTITY_NAME(QUANTITY, NAME)                           \
  PHYSICS_DEFINE_UNIT_NAME(typename QUANTITY::unit, NAME)

// stream value(), equivalent to <ostream> << q.value();
template <class Unit>
std::ostream& operator<<(std::ostream& os, const physics::quantity<Unit>& q);
// store new double value
template <class Unit>
std::istream& operator>>(std::istream& is, physics::quantity<Unit>& q);

// =============================================================================
// implementation: unit streaming
// =============================================================================
namespace physics {
namespace unit_impl {
template <const char* const& Name, class Dimension>
struct single_name_dimension_streamer {
  single_name_dimension_streamer(std::ostream& os) {
    if (Dimension::num) {
      os << " " << Name;
      if (Dimension::den != 1) {
        os << "^(" << Dimension::num << "/" << Dimension::den << ")";
      } else if (Dimension::num != 1) {
        os << "^" << Dimension::num;
      }
    }
  }
};
template <class... NameDimensionStreamers>
struct unit_name_dimensions_streamer : NameDimensionStreamers... {
  unit_name_dimensions_streamer(std::ostream& os)
      : NameDimensionStreamers(os)... {}
};
template <const char* const&... Names, class... Dimensions>
void stream_unit_name_dimensions(std::ostream& os, unit_system<Names...>,
                                 unit_dimensions<Dimensions...>) {
  auto us = unit_name_dimensions_streamer<
      single_name_dimension_streamer<Names, Dimensions>...>{os};
}
} // unit_impl

template <class Unit> std::string unit_string() {
  using system = typename Unit::system;
  using dimensions = typename Unit::dimensions;
  using factor = typename Unit::factor;
  using pow_10 = typename Unit::pow_10;
  using pow_pi = typename Unit::pow_pi;
  std::stringstream os;

  // intrinsic factors for our unit
  bool have_factor{std::ratio_not_equal<factor, std::ratio<1>>::value};
  bool have_pow_10{std::ratio_not_equal<pow_10, std::ratio<0>>::value};
  bool have_pow_pi{std::ratio_not_equal<pow_pi, std::ratio<0>>::value};
  if (have_factor || have_pow_10 || have_pow_pi) {
    bool first = true;
    os << " x (";
    if (have_factor) {
      os << double(factor::num) / factor::den;
      first = false;
    }
    if (have_pow_10) {
      if (!first) {
        os << " x ";
      } else {
        first = false;
      }
      os << "10";
      if (pow_10::den != 1) {
        os << "^(" << pow_10::num << "/" << pow_10::den << ")";
      } else if (pow_10::num != 1) {
        os << "^" << pow_10::num;
      }
    }
    if (have_pow_pi) {
      if (!first) {
        os << " x ";
      } else {
        first = false;
      }
      os << "pi";
      if (pow_pi::den != 1) {
        os << "^(" << pow_pi::num << "/" << pow_pi::den << ")";
      } else if (pow_pi::num != 1) {
        os << "^" << pow_pi::num;
      }
    }
    os << ")";
  }

  // actual unit names
  physics::unit_impl::stream_unit_name_dimensions(os, system{}, dimensions{});
  return os.str();
}
} // physics

// =============================================================================
// implementation: quantity I/O
// =============================================================================
template <class Unit>
std::ostream& operator<<(std::ostream& os, const physics::quantity<Unit>& q) {
  os << q.value();
  return os;
}
template <class Unit>
std::istream& operator>>(std::istream& is, physics::quantity<Unit>& q){
  double value;
  is >> value;
  q = physics::quantity<Unit>{q};
  return is;
}

#endif
