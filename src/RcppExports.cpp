// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include "../inst/include/tuplemapr.h"
#include <Rcpp.h>

using namespace Rcpp;

// test_map
void test_map();
RcppExport SEXP _tuplemapr_test_map() {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    test_map();
    return R_NilValue;
END_RCPP
}
// test_choose
void test_choose();
RcppExport SEXP _tuplemapr_test_choose() {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    test_choose();
    return R_NilValue;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_tuplemapr_test_map", (DL_FUNC) &_tuplemapr_test_map, 0},
    {"_tuplemapr_test_choose", (DL_FUNC) &_tuplemapr_test_choose, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_tuplemapr(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
