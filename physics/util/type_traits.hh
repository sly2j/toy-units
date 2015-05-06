#ifndef PHYSICS_UTIL_TYPE_TRAITS_LOADED
#define PHYSICS_UTIL_TYPE_TRAITS_LOADED

#include <type_traits>

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace physics {
// =============================================================================
// is_container: evaluates to std::true_type for ITERABLE STL containers
//               and basic arrays
// =============================================================================
template <class T> struct is_container : std::false_type {};
template <class T, size_t N> struct is_container<T[N]> : std::true_type {};
template <class T, size_t N>
struct is_container<std::array<T, N>> : std::true_type {};
template <class T, class Alloc>
struct is_container<std::deque<T, Alloc>> : std::true_type {};
template <class T, class Alloc>
struct is_container<std::forward_list<T, Alloc>> : std::true_type {};
template <class T, class Alloc>
struct is_container<std::list<T, Alloc>> : std::true_type {};
template <class Key, class T, class Compare, class Alloc>
struct is_container<std::map<Key, T, Compare, Alloc>> : std::true_type {};
template <class Key, class T, class Compare, class Alloc>
struct is_container<std::multimap<Key, T, Compare, Alloc>> : std::true_type {};
template <class Key, class Compare, class Alloc>
struct is_container<std::set<Key, Compare, Alloc>> : std::true_type {};
template <class Key, class Compare, class Alloc>
struct is_container<std::multiset<Key, Compare, Alloc>> : std::true_type {};
template <class Key, class T, class Hash, class Pred, class Alloc>
struct is_container<std::unordered_map<Key, T, Hash, Pred, Alloc>>
    : std::true_type {};
template <class Key, class T, class Hash, class Pred, class Alloc>
struct is_container<std::unordered_multimap<Key, T, Hash, Pred, Alloc>>
    : std::true_type {};
template <class Key, class Hash, class Pred, class Alloc>
struct is_container<std::unordered_set<Key, Hash, Pred, Alloc>>
    : std::true_type {};
template <class Key, class Hash, class Pred, class Alloc>
struct is_container<std::unordered_multiset<Key, Hash, Pred, Alloc>>
    : std::true_type {};
template <class T, class Alloc>
struct is_container<std::vector<T, Alloc>> : std::true_type {};

// =============================================================================
// is_map: true for maps, false for everything else
// =============================================================================
template <class T> struct is_map : std::false_type {};
template <class T, class Alloc>
struct is_map<std::map<T, Alloc>> : std::true_type {};
}

#endif
