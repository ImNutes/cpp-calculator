#include <chrono>
#include <iomanip>
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>
#include "Calculator.hpp"
const std::string suffix = "~> ";
std::vector<std::string> vocab;

// wraps readline and the history functions
std::string myread(const std::string &prompt) {
  char *line = readline(prompt.c_str());
  std::string res = line;
  if (line && *line)
    add_history(line);
  free(line);
  return res;
}

char *cmd_gen(const char *text, int state) {
  static int len;
  static unsigned long list_index;
  // char *name = NULL;
  if (!state) {
    len = strlen(text);
    list_index = 0;
  }
  while (list_index < vocab.size()) {
    const char *name = vocab[list_index++].c_str();
    if (strncmp(name, text, len) == 0)
      return strdup(name);
  }
  return (char *)NULL;
}

char **completion(const char *text, int start, int end) {
  char **matches;
  matches = (char **)NULL;
  rl_attempted_completion_over = 1;
  matches = rl_completion_matches(text, cmd_gen);
  return matches;
}
void readInit() {
  using_history();
  rl_readline_name = "Calculator";
  rl_attempted_completion_function = completion;
}
enum outputFlags {
  NONE = 1 << 0,
  RPN = 1 << 1,
  HEX = 1 << 2,
  OCT = 1 << 3,
  DEC = 1 << 4
};
int main(int argc, char *argv[]) {
  Calculator calculator;
  std::string prompt = '[' + calculator.getSettings() + ']' + suffix;
  vocab = calculator.genVocab();
  readInit();
  mpf_set_default_prec(64);
  std::string input;
  unsigned int out = 0;
  while (true) {
    input = myread(prompt);
    if (input == "exit" || input[0] == 'q')
      break;
    if(input.compare(0, 3, "set") == 0) {
      std::string operation = input.substr(4);
      if(operation.compare(0, 5, "angle") == 0) {
        //todo: instead of taking a specific string, pass the operation to the updateMode function
        std::cout << calculator.updateMode(operation.substr(6, operation.length() - 1)) << '\n';
      } else if(operation.compare(0, 3, "out") == 0) {
        //modify output, like oct, hex, etc
      std::cout << "ora\n";
        switch(tolower(operation[4])) {
          case 'r':
            out ^= RPN;
            break;
          case 'h':
            out ^= HEX;
            break;
          case 'o':
            out ^= OCT;
            break;
          case 'd':
            out ^= DEC;
            break;
          case 'f':
            out = -1; //set all bits
            break;
        }
      std::cout << out << '\n';
      }
      prompt = '[' + calculator.getSettings() + ']' + suffix;
    } else if(input.compare(0, 3, "out") == 0) {
    } else {
      try {
        mpf_class res = calculator.parse(input);
        if(out & HEX) std::cout << "hex: " << std::hex << res << '\n';
        if(out & OCT) std::cout << "oct: " << std::oct << res << '\n'; //there is a less readable way to do this
        if(out & RPN) std::cout << "rpn: " << calculator.getRPN() << '\n';
        std::cout << std::setprecision(10) << std::dec << res << "\n";
        //std::cout << calculator.getRPN() << "\n";
      } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
      }
    }
  }
  std::cout << "hasta luego\n";
  return 0;
}