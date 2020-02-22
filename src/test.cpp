#include <Rcpp.h>
using namespace Rcpp;

#include <array>

#include "tuplemapr.h"
using namespace keittlab;

// [[Rcpp::export]]
void test_map() {
  tuple::map([](auto&& x)->void{ Rcout << x << std::endl; }, std::make_tuple("test"));
}

// [[Rcpp::export]]
void test_choose() {}