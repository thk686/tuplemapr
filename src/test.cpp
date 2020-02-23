#include <Rcpp.h>
using namespace Rcpp;

#include <array>

#include "tuplemapr.h"
using namespace keittlab;

// [[Rcpp::export]]
void test_map() {
  std::array<double, 3>
  a = {{0.2, 4.1, 1.3}},
    b = {{2.6, 1.2, 0.8}},
    c = {{3.1, 8.4, 2.4}};
  auto f = [](auto&& ai, auto&& bi, auto&& ci) {
    return (ai - bi) / ci;  // scaled difference
  };
  auto x = tuple::map(f, a, b, c); // return array
  tuple::map([](auto&& e){ Rcout << e; }, x);
}

// [[Rcpp::export]]
void test_choose() {}