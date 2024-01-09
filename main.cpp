#include "Calculator.hpp"
#include <iostream>
#include <iomanip>
int main(int argc, char *argv[]) {
  mpf_set_default_prec(64);
  std::string input;
  if (argc == 2) {
    input = argv[2];
  } else {
    std::cout << "enter equation"
              << "\n";
    std::getline(std::cin, input);
  }
  Calculator calculator;
  while(input != "exit" || input != "q") {
    calculator.parse(input);
    std::cout << "RPN: " << calculator.getRPN() << "\n";
    try {
      std::cout << std::setprecision(10) << calculator.evaluate()<< "\n";
    } catch (const char *message) {
      std::cout << message;
    }
    std::cout << "Enter equation" << "\n";
    std::getline(std::cin, input);
  }
  return 0;
}