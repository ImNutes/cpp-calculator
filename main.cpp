#include <iostream>
#include <iomanip>
#include <readline/readline.h>
#include <readline/history.h>
#include "Calculator.hpp"
const std::string prompt = "~> ";
std::string myread(const std::string &prompt = ::prompt) {
  char * line = readline(prompt.c_str());
  std::string res = line;
  if(line && *line)
    add_history(line);
  free(line);
  return res;
}
int main(int argc, char *argv[]) {
  mpf_set_default_prec(64);
  std::string input;
  if (argc == 2) {
    input = argv[2];
  } else {
    std::cout << "enter equation"
              << "\n";
    input = myread(prompt);
  }
  Calculator calculator;
  try {
  while(true) {
    if(input == "exit" || input[0] == 'q') break;
    calculator.parse(input);
    std::cout << "RPN: " << calculator.getRPN() << "\n";
    std::cout <<  std::setprecision(10) << calculator.evaluate() << "\n";
    std::cout << "enter equation"
              << "\n";
    input = myread(prompt);
  }
  } catch(char const* msg) {
    std::cout << msg << "\n";
  }
  return 0;
}