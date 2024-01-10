#pragma once
#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP
#define _GLIBCXX_USE_CXX11_ABI 0
#include <array>
#include <gmpxx.h>
#include <functional>
#include <set>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include "func.hpp"
class Calculator {
  enum equation_type { OPERATOR, DIGIT, PAREN_OPEN, PAREN_CLOSE, FUNCTION, VARIABLE, NO };

public:
  Calculator();
  Calculator(std::string s);
  void push_back(std::string c);
  void push_back(mpf_class d);
  // pushes off everything that remains on the stack
  void push();
  void push(std::vector<std::string> &stacc);
  // evaluates the RPN stored in queue
  mpf_class evaluate();
  // returns the queue
  std::string getQueue() const;
  //returns the reverse polish notation version of the equation.
  std::string getRPN() const;
  // parses a string. WILL CLEAR BOTH THE QUEUE AND THE STACK
  void parse(std::string s);

private:
  mpf_class operate(mpf_class x, mpf_class y, char c) noexcept;
  mpf_class operate(mpf_class x, std::string str) noexcept;
  mpf_class convertVariable(std::string str) noexcept;
  constexpr int getPrecedence(char c) noexcept;
  std::string getStack() const;

  // stores a representation of the rpn. %d represents an mpf_class value in the values vector.
  std::vector<std::string> queue;
  //stores values as mpf_classes
  std::vector<mpf_class> values;
  // temporarily stores operators
  std::vector<std::string> stack;

  // functions
  std::map<std::string, std::function<mpf_class(mpf_class)> > functions = {
          {"sin", Func::mpf_sin}, {"cos", Func::mpf_cos}, {"tan", Func::mpf_tan}, 
{"asin", Func::mpf_asin}, {"acos", Func::mpf_acos}, {"atan", Func::mpf_atan},
{"abs", Func::abs}, {"sqrt", Func::sqrt}
  };

  std::map<std::string, std::function<mpf_class(mpf_class, mpf_class)> > functions2 = {
    {"pow", Func::mpf_pow}, {"atan2", Func::mpf_atan2}
  };
  //variables
  const std::map<std::string, mpf_class> const_variables = {
      {"pi", std::numbers::pi}, {"e", std::numbers::e}, {"phi", std::numbers::phi}
  };
//not fully implemented
  std::map<std::string, mpf_class> vars = {
      {"ans", 0}, {"x", 0}
  };
};
#endif
