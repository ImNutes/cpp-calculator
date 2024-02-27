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
int main(int argc, char *argv[]) {
  Calculator calculator;
  std::string prompt = calculator.getSettings() + suffix;
  vocab = calculator.genVocab();
  readInit();
  mpf_set_default_prec(64);
  std::string input;
  while (true) {
    input = myread(prompt);
    if (input == "exit" || input[0] == 'q')
      break;
    // only works on linux
    if (input == "clear") {
      std::cout << "\033[2J\033[1;1H" << std::endl;
    }
    if (input.compare(0, 4, "mode") == 0) {
      std::cout << calculator.updateMode(input.substr(5, input.length() - 1)) << std::endl;
      prompt = calculator.getSettings() + suffix;
    } else {
      try {
        std::cout << std::setprecision(10) << calculator.parse(input) << "\n";
        //std::cout << calculator.getRPN() << "\n";
      } catch (const char *msg) {
        std::cerr << msg << "\n";
      }
    }
  }
  std::cout << "hasta luego\n";
  return 0;
}