#include "Calculator.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
Calculator::Calculator() {}
Calculator::Calculator(std::string s) { Calculator(); parse(s); }
constexpr int Calculator::getPrecedence(char c) noexcept {
  switch (c) {
  case '+':
  case '-':
    return 2;
  case '*':
  case '/':
    return 3;
  case '`': // backtick used internally for implicit multiplication
  case '^':
    return 4;
  case 'f':
    return 5;
  default:
    return 0;
  }
}
mpf_class Calculator::operate(mpf_class x, mpf_class y, char c) noexcept {
  switch (c) {
  case '`':
  case '*':
    return x * y;
  case '/':
    return x / y;
  case '+':
    return x + y;
  case '-':
    return x - y;
  case '^':
    return Func::mpf_pow(x, y);
  default:
    return x;
  }
}

mpf_class Calculator::operate(mpf_class x, std::string str) noexcept {
  str = str.substr(3, str.length() - 1);
  if (str == "sin")
    return Func::mpf_sin(x);
  if (str == "cos")
    return Func::mpf_cos(x);
  if (str == "tan")
    return Func::mpf_tan(x);
  if (str == "asin")
    return Func::mpf_asin(x);
  return 0;
}
mpf_class Calculator::convertVariable(std::string str) noexcept {
  if (str == "pi")
    return M_PI;
  return 0;
}
mpf_class Calculator::evaluate() {
  if (!stack.empty())
    throw "stack is not clear";
  if(values.empty()) return 0;
  std::vector<mpf_class> tmp;
  int j = 0;
  for (std::string i : queue) {
    if (i == "%d") {
      tmp.push_back(values[j++]);
    } else {
      auto popLast = [&]() {
        mpf_class x = tmp.back();
        tmp.pop_back();
        return x;
      };
      if (i.length() == 1) {
        if (tmp.size() < 2)
          break;
        mpf_class x, y;
        y = popLast();
        x = popLast();
        tmp.push_back(operate(x, y, i[0]));
      } else if (i.substr(0, 1) == "f") {
        char params = i[1];
        i = i.substr(3, i.length() - 1);
        switch(params) {
          case '1':
            if(tmp.size() < 1) throw "not enough params";
            tmp.push_back(functions[i](popLast()));
            break;
          case '2':
            if(tmp.size() < 2) throw "not enough params";
            mpf_class x = popLast();
            tmp.push_back(functions2[i](popLast(), x));
            break;
        }
      } else if (i.substr(0, 1) == "v") {
        tmp.push_back(convertVariable(i));
      }
    }
  }
  if (tmp.size() > 2)
    throw "tmp ";
  this->vars["ans"] = tmp.back();
  return this->vars["ans"];
}
void Calculator::parse(std::string s) {
  queue.clear();
  stack.clear();
  values.clear();
  if (s.empty() || s == "\n")
    return;
  std::string tmp;
  bool in_digit = 0;
  auto pushTmp = [&]() {
    if (tmp.front() == '.')
      tmp = '0' + tmp;
    push_back(tmp);
    tmp.clear();
  };
  for (size_t i = 0; i < s.length(); ++i) {
    if (isspace(s[i]))
      continue;
    if(tmp.empty() && s[i] == '-' && isdigit(s[i+1])) {
      tmp += s[i++];
    }
    if (isdigit(s[i]) || s[i] == '.' ) {
      tmp += s[i];
      in_digit = true;
    } else {
      if (in_digit) {
        in_digit = false;
        pushTmp();
      }
      if (isalpha(s[i])) {
        std::string str;
        auto seekNPush = [&](auto map, std::string prefix) {
          auto f = map.find(str);
          if (f != map.end()) {
            push_back(prefix + str);
            return true;
          }
          return false;
        };
        while (isalpha(s[i]) || isdigit(s[i])) // eat up alpha characters
          str += s[i++];
       
        --i;
        if(seekNPush(this->functions, "f1_")) continue;
        if(seekNPush(this->functions2, "f2_")) continue;
        if(seekNPush(this->const_variables, "c_")) continue;
        if(seekNPush(this->vars, "v_")) continue;
      }
      if (!isspace(s[i])) {
        push_back(std::string() + s[i]);
      }
    }
  }
  if (!tmp.empty())
    pushTmp();
  push();
  push_back("\n");
}
std::string Calculator::getStack() const {
  std::string s;
  for (std::string i : stack) {
    s += i;
  }
  return s;
}
// pushes
void Calculator::push_back(mpf_class d) {
  values.push_back(d);
  queue.push_back("%d");
}
void Calculator::push_back(std::string c) {
  static bool in_paren{false};
  static equation_type lastType;
  static std::vector<std::string> parenStack;
  // if i encounter a newline, reset the state of the function so i can read
  // more equations
  if (c[0] == '\n') {
    parenStack.clear();
    lastType = NO;
    in_paren = false;
    return;
  }
  if (isdigit(c[0]) || (c[0] == '-' && c.length() > 1)) {
    if (lastType == PAREN_CLOSE) {
      push_back("`");
    }
    queue.push_back("%d");
    values.emplace_back(c);
    lastType = DIGIT;
  } else if (c.size() == 1) {
    if (c == "(") {
      parenStack.push_back(c);
      in_paren = true;
      if ((lastType == DIGIT || lastType == PAREN_CLOSE))
        push_back("`");
      lastType = PAREN_OPEN;
    } else if (c == ")") {
      while (parenStack.back() != "(" && !parenStack.empty()) {
        queue.push_back(parenStack.back());
        parenStack.pop_back();
      }
      in_paren = false;
      parenStack.pop_back();
      lastType = PAREN_CLOSE;
    } else if (c.size() == 1) {
      lastType = OPERATOR;
      int precedence = getPrecedence(c[0]);
      if(precedence == 0) return;
      std::vector<std::string> &use = in_paren ? parenStack : stack;
      if (!use.empty() && precedence <= getPrecedence(use.back()[0])) {
        push(use);
      }
      if (c == "`")
        stack.push_back("*");
      else
        use.push_back(c);
    }
  } else if (!isspace(c[0])) {
    if (lastType == DIGIT)
      push_back("`");
    std::string str = c.substr(2);
    switch(c[0]) {
      case 'f':
        stack.push_back(c);
        lastType = FUNCTION;
        break;
      case 'c':
        values.push_back(const_variables.at(str));
        queue.push_back("%d");
        lastType = DIGIT;
        break;
      case 'v':
        values.push_back(vars.at(str));
        queue.push_back("%d");
        lastType = DIGIT;
        break;
    }
  }
}
void Calculator::push() { push(this->stack); }
void Calculator::push(std::vector<std::string> &stacc) {
  while (stacc.size() > 0) {
    if (stacc.back() == "(")
      return;
    if (!isspace(stacc.back()[0])) {
      queue.push_back(stacc.back());
    }
    stacc.pop_back();
  }
}
std::string Calculator::getRPN() const {
  int i = 0;
  std::string res;
  for (auto ii : queue) {
    if (ii == "%d")
      res += Func::to_string(values[i++]) + ' ';
    else
      res += ii + ' ';
  }
  return res;
}
std::string Calculator::getQueue() const {
  std::string tmp;
  for (auto i : queue) {
    tmp += i + ' ';
  }
  return tmp;
}
std::vector<std::string> Calculator::genVocab() {
  //bad code to generate a vocab vector for autocompletion
  std::vector<std::string> vocab;
  auto addToVocab = [&](auto a) {
    for(auto &i : a)
      vocab.push_back((i.first));
  };
  addToVocab(functions);
  addToVocab(functions2);
  addToVocab(vars);
  addToVocab(const_variables);
  return vocab;
}
