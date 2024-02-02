#include "Calculator.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
Calculator::Calculator() {}
Calculator::Calculator(const std::string &s) { parse(s); }
constexpr int Calculator::getPrecedence(char c) noexcept {
  switch (c) {
  case '=':
    return 1;
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

mpf_class Calculator::evaluate() {
  if (!stack.empty())
    throw "stack is not clear";
  if(values.empty()) return 0;
  std::vector<mpf_class> tmp;
  int j = 0;
  for (std::string i : queue) {
    if (i == "%d" || (i == "%v" && j != 0)) {
      tmp.push_back(values[j++]);
    } else {
      auto popLast = [&]() {
        mpf_class x = tmp.back();
        tmp.pop_back();
        return x;
      };
      if (i.length() == 1 && i != ",") {
        if (tmp.size() < 2)
          throw "tmp is bad";
        mpf_class x, y;
        y = popLast();
        x = popLast();
        tmp.push_back(operate(x, y, i[0]));
        continue;
      }
      size_t delim = i.find('_');
      if(delim == std::string::npos) continue;
      std::string prefix = i.substr(0, delim);
      char params = prefix[1];
      char set{' '}; //setting on some functions
      if(prefix.length() > 2) set = prefix[2];
      i = i.substr(delim + 1, i.length() - 1);
      if(prefix[0] == 'f') {
        switch(params) {
          case '1': {
            mpf_class ans;
            if(set == 't') {
              ans = trig_functions[i](popLast(), m_settings & flags::DEGREES);
            } else {
              ans = functions[i](popLast());
            }
            tmp.push_back(ans);
            break;
          }
          case '2':
            if(tmp.size() < 2) throw "not enough params";
            mpf_class x = popLast();
            tmp.push_back(functions2[i](popLast(), x));
            break;
        }
      }
    }
  }
  if (tmp.size() > 2)
    throw "tmp brok ";
  this->vars["ans"] = tmp.back();
  return this->vars["ans"];
}
bool Calculator::updateMode(const std::string &mode) {
  if(mode == "deg" || mode == "degrees")
    m_settings |= DEGREES;
  else if(mode == "rad" || mode == "radians")
    m_settings &= ~DEGREES;
  return true;
}
mpf_class Calculator::parse(const std::string &s) {
  queue.clear();
  stack.clear();
  values.clear();
  lastType = NO;
  if (s.empty() || s == "\n")
    return 0;
  std::string tmp;
  bool in_digit = 0;
  auto pushTmp = [&]() {
    if (tmp.front() == '.')
      tmp = '0' + tmp;
    push_back(mpf_class(tmp));
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
        auto seekNPush = [&](auto map, const std::string &prefix) {
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
        if(seekNPush(this->trig_functions, "f1t_")) continue;
        if(seekNPush(this->functions2, "f2_")) continue;
        if(seekNPush(this->const_variables, "c_")) continue;
        if(seekNPush(this->vars, "v_")) continue;
      }
      if (!isspace(s[i])) {
        push_back(s[i]);
      }
    }
  }
  if (!tmp.empty()){
    pushTmp();
  }
  push();
  push_back("\n");
  return evaluate();
}
std::string Calculator::getStack() const {
  std::string s;
  for (std::string i : stack) {
    s += i;
  }
  return s;
}
// pushes an int directly onto the stack
void Calculator::push_back(mpf_class d) {
  values.push_back(d);
  queue.push_back("%d");
  lastType = DIGIT;
}

void Calculator::clear() {
  while(!parenStack.empty()) {
    queue.push_back(parenStack.back());
    parenStack.pop_back();
  }
  lastType = NO;
  in_paren = 0;
}
void Calculator::push_back(char c) {
  switch(c) {
    case '\n':
      clear();
      break;
    case '(':
      parenStack.push_back(std::string() + c);
      ++in_paren;
      if ((lastType == DIGIT || lastType == PAREN_CLOSE)) {
        push_back('`');
      }
      lastType = PAREN_OPEN;
      break;
    case ')':
      if(in_paren == 0) return;
      while (parenStack.back() != "(" && !parenStack.empty()) {
        queue.push_back(parenStack.back());
        parenStack.pop_back();
      }
      --in_paren;
      parenStack.pop_back();
      lastType = PAREN_CLOSE;
      break;
    default:
      lastType = OPERATOR;
      int precedence = getPrecedence(c);
      if(precedence == 0) return;
      std::vector<std::string> &use = in_paren > 0 ? parenStack : stack;
      if (!use.empty() && precedence <= getPrecedence(use.back()[0])) {
        push(use);
      }
      if (c == '`') {
        stack.push_back("*");
      }
      else
        use.push_back(std::string() + c);
      break;
  }
}
void Calculator::push_back(const std::string &c) {
  // if i encounter a newline, reset the state of the function so i can read
  // more equations
  if (c[0] == '\n') {
    clear();
    return;
  }
  if (isdigit(c[0]) || (c[0] == '-' && c.length() > 1)) {
    if (lastType == PAREN_CLOSE) {
      push_back('`');
    }
    queue.push_back("%d");
    values.emplace_back(c);
    lastType = DIGIT;
  } else if(c.size() == 1)
    std::cout << c << " size 1 string passed\n";
  else {
    if (lastType == DIGIT) {
      push_back('`');
    }
    std::string str = c.substr(2);
    switch(c[0]) {
      case 'f': {
        stack.push_back(c);
        lastType = FUNCTION;
        break;
      }
      case 'c':
        values.push_back(const_variables.at(str));
        queue.push_back("%d");
        lastType = DIGIT;
        break;
      case 'v':
        values.push_back(vars.at(str));
        queue.push_back("%v");
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
const std::vector<std::string> Calculator::genVocab() const {
  //bad code to generate a vocab vector for autocompletion
  std::vector<std::string> vocab;
  auto addToVocab = [&](auto a) {
    for(auto &i : a) {
      vocab.push_back((i.first));
      std::cout << i.first;
    }
    std::cout << std::endl;
  };
  addToVocab(trig_functions);
  addToVocab(functions);
  addToVocab(functions2);
  addToVocab(vars);
  addToVocab(const_variables);
  return vocab;
}
