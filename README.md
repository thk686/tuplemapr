
<!-- README.md is generated from README.Rmd. Please edit that file -->

# tuplemapr

<!-- badges: start -->

[![Lifecycle:
retired](https://img.shields.io/badge/lifecycle-retired-orange.svg)](https://www.tidyverse.org/lifecycle/#retired)
<!-- badges: end -->

**Note: This package is no longer developed. I’ve incorporated the this
code into [kdtools](https://github.com/thk686/kdtools). You can include
the header using the `// [[Rcpp::depends(kdtools)]]` attribute or the
`LinkingTo: kdtools` directive in your package DESCRIPTION.**

I found myself using a lot of compile-time fixed-length objects in
projects like [kdtools](https://github.com/thk686/kdtools) and wanted a
better framework for manipulating them. Tuplemapr exports [a C++
header](https://github.com/thk686/tuplemapr/blob/master/inst/include/tuplemapr.h)
that defines functions for applying invokables on tuples or tuple-like
objects, including `std::array` and `std::pair`. At its core, tuplemapr
defines a function `tuple::map`, which has the signature:

    map(F&& f, Ts&&... ts)

Here, `f` is an invokable object and `ts` is a list of *any number of
tuple-like objects*. It returns the equivalent of:

    f(get<0>(ts1), get<0>(ts2), ...),
    f(get<1>(ts1), get<1>(ts2), ...), ...

The expansion above is done at compile-time using nested parameter pack
expansions.

The main difference between this and the many similar map-over-tuple
implementations available is the ability to map over a set of tuple-like
objects. That makes it simple to write expressions like:

``` cpp
// [[Rcpp::plugins(cpp17)]]    
// [[Rcpp::depends(tuplemapr)]]
#include <Rcpp.h>
using namespace Rcpp;
#include <tuplemapr.h>
using namespace keittlab;
std::array<double, 3> a = {{0.2, 4.1, 1.3}}, b = {{2.6, 1.2, 0.8}};
auto f = [](auto&&... xs){ return (0 + ... + xs); };
auto g = std::multiplies<double>{};
auto x = std::apply(f, tuple::map(g, a, b)); // dot product
//Rcout << x << std::endl;
```

The above can be computed at compile-time by adding `constexpr`. Since
we can map over any number of data objects, we can also do something
like:

``` cpp
// [[Rcpp::plugins(cpp17)]]    
// [[Rcpp::depends(tuplemapr)]]
#include <Rcpp.h>
using namespace Rcpp;
#include <tuplemapr.h>
using namespace keittlab;
std::array<double, 3>
  a = {{0.2, 4.1, 1.3}},
  b = {{2.6, 1.2, 0.8}},
  c = {{3.1, 8.4, 2.4}};
auto f = [](double ai, double bi, double ci) {
  return (ai - bi) / ci;  // scaled difference
};
auto x = tuple::map(f, a, b, c); // return array
// tuple::map([](auto&& e){ Rcout << e; }, x);
```

Given a single tuple-like object and a void-returning invokable,
`tuple::map` behaves just like a tuple for-each. In other cases,
`tuple::map` will return either a pair, array or tuple, depending on the
type of the first object in the data list. The number of elements in the
output is also determined by the number of elements in the first data
object.
