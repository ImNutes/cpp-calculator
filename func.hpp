#pragma once
#ifndef FUNC_HPP
#define FUNC_HPP
#define ACCURACY 50
#include <iostream>
#include <algorithm>
#include <gmpxx.h>
#include <vector>
#include <functional>
#include <sstream>
//mpf helper functions
namespace Func {
  const mpf_class pi = 3.1415926535897932384626433832795;
  //accepts a function taking a template and an int representing the nth iteration
  mpf_class summation(std::function<mpf_class(int)> fn, int x = ACCURACY, int y = 0);
  inline mpf_class reduceRad(mpf_class t) {
    while (t > pi) t -= 2*pi;
    return t;
  }
  mpz_class mpz_fac(unsigned long x);
  mpf_class pow(mpf_class t, long b);
  mpf_class pow(mpf_class t, mpf_class b);
  mpf_class mpf_pow(mpf_class t, mpf_class b);
  mpf_class sqrt(mpf_class t);
  mpf_class abs(mpf_class t);
  inline std::string to_string(mpf_class t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
  }
  
  //borrowed from https://rosettacode.org/wiki/Bernoulli_numbers#C
  mpq_class bernoulli(unsigned int n);
  /*
  Maclaurin series approximations functions
  accurate to the 10th decimal
  although it may be faster to use a table, these functions should allow for an arbitrary amount of precision
  */
  mpf_class mpf_sin(mpf_class t);
  mpf_class mpf_cos(mpf_class t);
  mpf_class mpf_tan(mpf_class t);
  //these guys kind of suck, ranked in order of sucking from least to greatest
  mpf_class mpf_atan(mpf_class t);
  mpf_class mpf_atan2(mpf_class y, mpf_class x);
  mpf_class mpf_asin(mpf_class t);
  mpf_class mpf_acos(mpf_class t);
}
#endif