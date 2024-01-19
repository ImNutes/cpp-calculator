#include "func.hpp"
#include <chrono>
#include <gmpxx.h>
#include <mutex>
#include <atomic>
#include <thread>
mpf_class Func::summation(std::function<mpf_class(int)> fn, int x, int y) {
  mpf_class sum = 0;
  for (int i = y; i < x + y; ++i)
    sum += fn(i);
  // so that sin(pi) doesn't return a ridiculously small number and instead
  // returns what you expect
  return abs(sum) > 0.000000000000001 ? sum : 0;
}
mpz_class Func::mpz_fac(unsigned long x) {
  mpz_class res = 1;
  mpz_fac_ui(res.get_mpz_t(), x);
  return res;
}
mpf_class Func::pow(mpf_class t, long b) {
  mpf_class res(0);
  mpf_pow_ui(res.get_mpf_t(), t.get_mpf_t(), b);
  return res;
}
mpf_class Func::mpf_pow(mpf_class t, mpf_class b) { return pow(t, b.get_ui()); }
mpf_class Func::sqrt(mpf_class t) {
  mpf_class res(0);
  mpf_sqrt(res.get_mpf_t(), t.get_mpf_t());
  return res;
}

mpf_class Func::abs(mpf_class t) {
  mpf_class res(0);
  mpf_abs(res.get_mpf_t(), t.get_mpf_t());
  return res;
}

mpq_class Func::bernoulli(unsigned int n) {
  unsigned int m, j;
  std::vector<mpq_class> vec(n + 1);
  mpq_class res = 0;
  for (auto i : vec)
    mpq_init(i.get_mpq_t());
  for (m = 0; m <= n; ++m) {
    mpq_set_ui(vec[m].get_mpq_t(), 1, m + 1);
    for (j = m; j > 0; --j) {
      vec[j - 1] = vec[j] - vec[j - 1];
      mpq_set_ui(res.get_mpq_t(), j, 1);
      vec[j - 1] *= res;
    }
  }
  res = vec[0];
  return res;
}

mpf_class Func::mpf_sin(mpf_class t) {
  t = reduceRad(t);
  std::function<mpf_class(int n)> fn = [=](int n) {
    mpf_class numerator = pow(-1, n);
    mpz_class denominator = mpz_fac(2 * n + 1);
    mpf_class sum = 0;
    sum = (mpf_class(numerator) * (pow(t, 2 * n + 1))) / denominator;
    return sum;
  };
  t = summation(fn);
  return t;
}

mpf_class Func::mpf_cos(mpf_class t) {
  t = reduceRad(t);
  std::function<mpf_class(int n)> fn = [=](int n) {
    mpf_class numerator = pow(-1, n);
    mpf_class denominator = mpz_fac(2 * n);
    mpf_class sum = 0;
    sum = (mpf_class(numerator) * (pow(t, 2 * n))) / denominator;
    return sum;
  };
  t = summation(fn);
  return t;
}

mpf_class Func::mpf_tan(mpf_class t) {
  return mpf_sin(t) / mpf_cos(t);
  // the below solution works and should be faster but only if T < pi/2, the
  // above solution always works
  
  // mpf_class res = 0;
  // std::function<mpf_class(int n)> fn = [=](int n) {
  //   mpf_class numerator =
  //       mpf_class(bernoulli(2 * n)) * pow(-4, n) * (1 - pow(4, n));
  //   mpf_class denominator = mpz_fac(2 * n);
  //   mpf_class sum = 0;
  //   sum = (numerator * pow(t, 2 * n - 1)) / denominator;
  //   return sum;
  // };
  // return summation(fn, ACCURACY * 2, 1);
}

//from https://stackoverflow.com/a/42542593
mpf_class Func::mpf_atan(mpf_class t) {
  if(t > 1) return pi/2 - mpf_atan(1/t);
  //this is cheating
  if(t == 1) return pi/4;
  std::function<mpf_class(int)> fn = [=](int n) {
    mpf_class numerator = pow(-1, n);
    mpf_class denominator = (2 * n + 1);
    mpf_class res = pow(t, (2 * n + 1));
    res *= (numerator / denominator);
    return res;
  };
  return summation(fn, ACCURACY * 2);
}

mpf_class Func::mpf_asin(mpf_class t) {
  if(abs(t) > 1) throw "invalid input";
  return mpf_atan2(t, sqrt(1 - pow(t,2)));
  // std::function<mpf_class(int)> fn = [=](int n) {
  //   mpf_class numerator = mpz_fac(2 * n);
  //   mpf_class denominator = pow(4, n) * pow(mpz_fac(n), 2) * (2 * n + 1);
  //   mpf_class sum = 0;
  //   sum = numerator * (pow(t, (2 * n + 1)));
  //   sum /= denominator;
  //   return sum;
  // };
  // return summation(fn, ACCURACY * 2);
}
mpf_class Func::mpf_acos(mpf_class t) {
  if(abs(t) > 1) throw "invalid input";
  return (pi / 2) - mpf_asin(t);
}
mpf_class Func::mpf_atan2(mpf_class y, mpf_class x) {
  return mpf_atan(y / x) + (x < 0 ? pi : 0);
}

//end of trig functions