#pragma once
#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include <array>
#include <gmpxx.h>
#include <functional>
#include <set>
#include <string>
#include <stack>
#include <deque>
#include <map>
#include <cmath>
#include "func.hpp"

class Calculator {
enum equation_type { NO, OPERATOR, DIGIT, PAREN_OPEN, PAREN_CLOSE, FUNCTION, VARIABLE};
public:
  Calculator();
  Calculator(const std::string &s);
  void push_back(const std::string &c);
  void push_back(char c);
  void push_back(mpf_class d);
  // pushes off everything that remains on the stack
  void push();
  void push(std::stack<std::string> &stacc);

  mpf_class evaluate();
  void clear();
  std::string getQueue() const;
  std::string getRPN() const;

  bool updateMode(const std::string &mode);

  //WILL CLEAR BOTH THE QUEUE AND THE STACK
  mpf_class parse(const std::string &s);
  const std::vector<std::string> genVocab() const;
  const std::string getSettings() const;
private:
  mpf_class ans;
  enum flags {
    DEGREES = 1 << 0
  };
  //manages general calculator settings as defined in flags variable
  int m_settings{0};
  //manages state of push_back and parsing functions
  equation_type lastType{NO};
  int in_paren{0};
  
  mpf_class operate(mpf_class x, mpf_class y, char c) const noexcept;
  constexpr int getPrecedence(char c) const noexcept;
  std::string getStack() const;
  std::vector<std::string> queue;
  std::vector<mpf_class> values;
  std::stack<std::string> stack;
  std::stack<std::string> parenStack;


  //functions

  //1 param
  std::map<std::string, std::function<mpf_class(mpf_class)> > functions = {
    {"abs", Func::abs}, {"sqrt", Func::sqrt}
  };

  //trig functions, must be aware of the flags setting (deg or rad)
  std::map<std::string, std::function<mpf_class(mpf_class, bool)> >  trig_functions = {
    {"sin", Func::mpf_sin}, {"cos", Func::mpf_cos}, {"tan", Func::mpf_tan},
    {"asin", Func::mpf_asin}, {"acos", Func::mpf_acos}, {"atan", Func::mpf_atan}
  };


  //2 param
  std::map<std::string, std::function<mpf_class(mpf_class, mpf_class)> > functions2 = {
    {"pow", Func::mpf_pow}
  };
  //variables
  const std::map<std::string, mpf_class> const_variables = {
      {"pi", std::numbers::pi}, {"e", std::numbers::e}, {"phi", std::numbers::phi}
  };

//not fully implemented
  // std::map<std::string, mpf_class> vars = {
  //     {"ans", 0}, {"x", 0}
  // };
};
#endif
