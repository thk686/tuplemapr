#ifndef __TUPLEMAPR_H__
#define __TUPLEMAPR_H__

#include <tuple>
#include <cmath>
#include <array>
#include <string>
#include <utility>
#include <ostream>
#include <type_traits>

namespace keittlab {
namespace tuple {
namespace details {

template<typename... Ts>
using first_of = std::remove_reference_t<
  std::tuple_element_t<0, std::tuple<Ts...>>
>;

template<typename T>
using indices_spanning = std::make_index_sequence<
  std::tuple_size_v<
    std::remove_reference_t<T>
  >
>;

template<typename T>
constexpr bool is_void = std::is_same_v<void, T>;

// Modified from https://stackoverflow.com/a/25909944/1691101

template<typename T>
constexpr decltype(auto) fw(std::remove_reference_t<T>&& t)
{
  return std::forward<T>(t);
}

template<typename T>
constexpr decltype(auto) fw(std::remove_reference_t<T>& t)
{
  return std::forward<T>(t);
}

// Modified from https://stackoverflow.com/a/16905404/1691101

template<typename>
struct is_std_array : std::false_type {};
                    
template<typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template<typename T>
constexpr bool is_std_array_v = is_std_array<std::decay_t<T>>::value;
        
template<typename>
struct is_std_tuple : std::false_type {};

template<typename... Ts>
struct is_std_tuple<std::tuple<Ts...>> : std::true_type {};

template<typename T>
constexpr bool is_std_tuple_v = is_std_tuple<std::decay_t<T>>::value;

template<typename>
struct is_std_pair : std::false_type {};
                    
template<typename T, typename U>
struct is_std_pair<std::pair<T, U>> : std::true_type {};

template<typename T>
constexpr bool is_std_pair_v = is_std_pair<std::decay_t<T>>::value;

template<std::size_t I, typename... Ts>
constexpr decltype(auto) pick(Ts&&... ts) {
  return std::make_tuple(std::get<I>(fw<Ts>(ts))...);
}

template<typename F, std::size_t... Is, typename... Ts>
constexpr decltype(auto) map_tuple_impl(F&& f, std::index_sequence<Is...>, Ts&&... ts) {
  return std::make_tuple(std::apply(fw<F>(f), pick<Is>(fw<Ts>(ts)...))...);
}

template<typename F, std::size_t... Is, typename... Ts>
constexpr decltype(auto) map_array_impl(F&& f, std::index_sequence<Is...>, Ts&&... ts) {
  return std::array{std::apply(fw<F>(f), pick<Is>(fw<Ts>(ts)...))...};
}

template<typename F, std::size_t... Is, typename... Ts>
constexpr decltype(auto) map_pair_impl(F&& f, std::index_sequence<Is...>, Ts&&... ts) {
  return std::make_pair(std::apply(fw<F>(f), pick<Is>(fw<Ts>(ts)...))...);
}

template<typename F, std::size_t... Is, typename... Ts>
constexpr void map_void_impl(F&& f, std::index_sequence<Is...>, Ts&&... ts) {
  (std::apply(fw<F>(f), pick<Is>(fw<Ts>(ts)...)), ...);
}

template<typename F, typename... Ts>
constexpr decltype(auto) map0(F&& f, Ts&&... ts) {
  return std::apply(fw<F>(f), pick<0>(fw<Ts>(ts)...));
}

} // namespace details

template<typename F, typename... Ts>
constexpr decltype(auto) map_tuple(F&& f, Ts&&... ts) {
  using namespace details;
  using T = first_of<Ts...>;
  return map_tuple_impl(fw<F>(f), indices_spanning<T>{}, fw<Ts>(ts)...);
}

template<typename F, typename... Ts>
constexpr decltype(auto) map_array(F&& f, Ts&&... ts) {
  using namespace details;
  using T = first_of<Ts...>;
  return map_array_impl(fw<F>(f), indices_spanning<T>{}, fw<Ts>(ts)...);
}

template<typename F, typename... Ts>
constexpr decltype(auto) map_pair(F&& f, Ts&&... ts) {
  using namespace details;
  using T = first_of<Ts...>;
  return map_pair_impl(fw<F>(f), indices_spanning<T>{}, fw<Ts>(ts)...);
}

template<typename F, typename... Ts>
constexpr void map_void(F&& f, Ts&&... ts) {
  using namespace details;
  using T = first_of<Ts...>;
  map_void_impl(fw<F>(f), indices_spanning<T>{}, fw<Ts>(ts)...);
}

template<typename F, typename... Ts>
constexpr decltype(auto) map(F&& f, Ts&&... ts) {
  using namespace details;
  using T = first_of<Ts...>;
  using ret = decltype(map0(fw<F>(f), fw<Ts>(ts)...));
  if constexpr (is_void<ret>) {
    map_void(fw<F>(f), fw<Ts>(ts)...);
  } else if constexpr (is_std_pair_v<T>) {
    return map_pair(fw<F>(f), fw<Ts>(ts)...);
  } else if constexpr (is_std_array_v<T>) {
    return map_array(fw<F>(f), fw<Ts>(ts)...);
  } else {
    return map_tuple(fw<F>(f), fw<Ts>(ts)...);
  }
}

namespace details {

template<typename F, typename... Ts>
constexpr decltype(auto)
_map(F&& f, Ts&&... ts) {
  using details::fw;
  static_assert(sizeof...(ts) > 0);
  if constexpr (sizeof...(ts) > 1) {
    return map(fw<F>(f), fw<Ts>(ts)...);
  } else {
    return std::apply(fw<F>(f), fw<Ts>(ts)...);
  }
};

} // namespace details

template<typename T>
constexpr decltype(auto)
_not(T&& t) {
  using details::fw;
  return map([](auto&& x) {
    return !x;
  }, fw<T>(t));
}

template<typename... Ts>
constexpr decltype(auto)
sum(Ts&&... ts) {
  using namespace details;
  return _map([](auto&&... xs) {
    return (xs + ...);
  }, fw<Ts>(ts)...);
}

template<typename... Ts>
constexpr decltype(auto)
product(Ts&&... ts) {
  using namespace details;
  return _map([](auto&&... xs) {
    return (xs * ...);
  }, fw<Ts>(ts)...);
}

template<typename... Ts>
constexpr decltype(auto)
mean(Ts&&... ts) {
  using namespace details;
  return _map([](auto&&... xs) {
    return (xs + ...) / static_cast<double>(sizeof...(xs));
  }, fw<Ts>(ts)...);
}

template<typename... Ts>
constexpr decltype(auto)
all_true(Ts&&... ts) {
  using namespace details;
  return _map([](auto&&... xs) {
    return (xs && ...);
  }, fw<Ts>(ts)...);
}

template<typename... Ts>
constexpr decltype(auto)
all_false(Ts&&... ts) {
  using namespace details;
  return _map([](auto&&... xs) {
    return (!xs && ...);
  }, fw<Ts>(ts)...);
}

template<typename... Ts>
constexpr decltype(auto)
any_true(Ts&&... ts) {
  using namespace details;
  return _map([](auto&&... xs) {
    return (xs || ...);
  }, fw<Ts>(ts)...);
}

template<typename... Ts>
constexpr decltype(auto)
any_false(Ts&&... ts) {
  using namespace details;
  return _map([](auto&&... xs) {
    return (!xs || ...);
  }, fw<Ts>(ts)...);
}

// not constexpr
template<typename T>
constexpr decltype(auto)
pow(T&& t, double exp) {
  using details::fw;
  return map([exp](double base) {
    return std::pow(base, exp);
  }, fw<T>(t));
}

// not constexpr
template<typename T>
constexpr decltype(auto)
abs(T&& t) {
  using details::fw;
  return map([](auto x) {
    return std::abs(x);
  }, fw<T>(t));
}

// not constexpr
template<typename T>
constexpr decltype(auto)
pnorm(T&& t, double exp) {
  using details::fw;
  return std::pow(sum(abs(pow(fw<T>(t), exp))), 1 / exp);
}

// binary

template<typename T, typename U>
constexpr decltype(auto)
is_same(T&& t, U&& u) {
  using details::fw;
  return map([](auto&& a, auto&& b) {
    return std::is_same_v<decltype(a), decltype(b)>;
  }, fw<T>(t), fw<U>(u));
}

template<typename T, typename U>
constexpr decltype(auto)
equal(T&& t, U&& u) {
  using details::fw;
  return map([](auto&& a, auto&& b) {
    return a == b;
  }, fw<T>(t), fw<U>(u));
}

template<typename T, typename U>
constexpr decltype(auto)
all_equal(T&& t, U&& u) {
  using details::fw;
  return all_true(equal(fw<T>(t), fw<U>(u)));
}

template<typename T, typename U>
constexpr decltype(auto)
none_equal(T&& t, U&& u) {
  using details::fw;
  return all_false(equal(fw<T>(t), fw<U>(u)));
}

template<typename T, typename U>
constexpr decltype(auto)
less(T&& t, U&& u) {
  using details::fw;
  return map([](auto&& a, auto&& b) {
    return a < b;
  }, fw<T>(t), fw<U>(u));
}

template<typename T, typename U>
constexpr decltype(auto)
all_less(T&& t, U&& u) {
  using details::fw;
  return all_true(less(fw<T>(t), fw<U>(u)));
}

template<typename T, typename U>
constexpr decltype(auto)
none_less(T&& t, U&& u) {
  using details::fw;
  return all_false(less(fw<T>(t), fw<U>(u)));
}

template<typename T, typename U>
constexpr decltype(auto)
add(T&& t, U&& u) {
  using details::fw;
  if constexpr (std::is_arithmetic_v<U>) {
    return map([u](auto&& a) {
      return a + u;
    }, fw<T>(t));
  } else {
    return map([](auto&& a, auto&& b) {
      return a + b;
    }, fw<T>(t), fw<U>(u));
  }
}

template<typename T, typename U>
constexpr decltype(auto)
subtract(T&& t, U&& u) {
  using details::fw;
  if constexpr (std::is_arithmetic_v<U>) {
    return map([u](auto&& a) {
      return a - u;
    }, fw<T>(t));
  } else {
    return map([](auto&& a, auto&& b) {
      return a - b;
    }, fw<T>(t), fw<U>(u));
  }
}

template<typename T, typename U>
constexpr decltype(auto)
multiply(T&& t, U&& u) {
  using details::fw;
  if constexpr (std::is_arithmetic_v<U>) {
    return map([u](auto&& a) {
      return a * u;
    }, fw<T>(t));
  } else {
    return map([](auto&& a, auto&& b) {
      return a * b;
    }, fw<T>(t), fw<U>(u));
  }
}

template<typename T, typename U>
constexpr decltype(auto)
divide(T&& t, U&& u) {
  using details::fw;
  if constexpr (std::is_arithmetic_v<U>) {
    return map([u](auto&& a) {
      return a / u;
    }, fw<T>(t));
  } else {
    return map([](auto&& a, auto&& b) {
      return a / b;
    }, fw<T>(t), fw<U>(u));
  }
}

template<typename T, typename U>
constexpr decltype(auto)
dotprod(T&& t, U&& u) {
  using details::fw;
  return sum(multiply(fw<T>(t), fw<U>(u)));
}

// not constexpr
template<typename T, typename U>
constexpr decltype(auto)
pdist(T&& t, U&& u, double exp) {
  using details::fw;
  return pnorm(subtract(fw<T>(t), fw<U>(u)), exp);
}

template<typename T, typename U>
constexpr decltype(auto)
hamming(T&& t, U&& u) {
  using details::fw;
  return sum(_not(equal(fw<T>(t), fw<U>(u))));
}

template<typename F, typename T, typename U>
constexpr decltype(auto)
choose(F&& f, T&& t, U&& u) {
  using details::fw;
  return map([&f](auto&& a, auto&& b){
    return f() ? a : b;
  }, fw<T>(t), fw<U>(u));
}

template<typename T, typename U>
constexpr decltype(auto)
wmean(T&& t, U&& u) {
  using details::fw;
  return dotprod(fw<T>(t), fw<U>(u)) / sum(fw<U>(u));
}

} // namespace tuple

#ifndef OMIT_TESTS

namespace details {
namespace tests {

constexpr static std::array<double, 3>
  a1 = {{1, 2, 3}}, a2 = {{4, 5, 6}}, a3 = {{7, 8, 9}};

constexpr static std::tuple<const char*, double, bool>
  t1{"test", 3.14, false};

constexpr static std::tuple<double, bool, const char*>
  t2{0.33, true, "t2"};

static_assert(tuple::all_false(tuple::is_same(t1, t2)));
static_assert(tuple::all_true(tuple::is_same(a1, a2)));

constexpr static auto t3 = tuple::map([](auto&&... xs){ return true; }, t1, t2, a1);

static_assert(tuple::details::is_std_tuple_v<decltype(t3)>);

constexpr static auto p1 = std::make_pair(1, true);

constexpr static auto p2 = tuple::map([](auto&&...){ return true; }, p1, a1, a2, a3, t1, t2);

static_assert(tuple::details::is_std_pair_v<decltype(p2)>);
static_assert(std::get<0>(p2) && std::get<1>(p2));

constexpr static auto a4 = tuple::map([](auto&&... xs)->double{
  return (0 + ... + xs);
}, a1, a2, a3);

static_assert(tuple::details::is_std_array_v<decltype(a4)>);

static_assert(tuple::sum(a1) == 6);
static_assert(std::get<0>(tuple::sum(a1, a2)) == 5 &&
              std::get<1>(tuple::sum(a1, a2)) == 7 &&
              std::get<2>(tuple::sum(a1, a2)) == 9);

static_assert(tuple::mean(a1) == 2);

constexpr static auto a5 = tuple::map([](double x)->bool{ return x < 4; }, a1);

static_assert(tuple::all_true(a5));
static_assert(tuple::all_false(tuple::_not(a5)));

static_assert(tuple::any_true(a5));
static_assert(!tuple::any_false(a5));

static_assert(tuple::all_less(a1, a2));

static_assert(tuple::sum(tuple::add(a1, a3)) == 30);
static_assert(tuple::sum(tuple::add(a1, 2)) == 12);

static_assert(tuple::sum(tuple::subtract(a1, a3)) == -18);
static_assert(tuple::sum(tuple::subtract(a1, 2)) == 0);

static_assert(tuple::sum(tuple::multiply(a1, a3)) == 50);
static_assert(tuple::sum(tuple::multiply(a1, 2)) == 12);

static_assert(tuple::sum(tuple::divide(a1, a3)) - 0.7261905 < 1e-5);
static_assert(tuple::sum(tuple::divide(a1, 2)) == 3);

static_assert(tuple::dotprod(a1, a3) == 50);

static_assert(tuple::hamming(a1, a2) == 3);

static_assert(tuple::all_equal(tuple::choose([](auto&&...){ return true; }, a1, a2), a1));
static_assert(tuple::all_equal(tuple::choose([](auto&&...){ return false; }, a1, a2), a2));

static_assert(tuple::wmean(a1, a2) - 2.133333 < 1e-5);

} // namespace tests
} // namespace details

#endif // OMIT_TESTS

} // namespace keittlab

#endif // __TUPLEMAPR_H__
