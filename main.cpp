#include <iostream>
#include <iomanip>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include "Calculator.hpp"

const std::string prompt = "~> ";
std::vector<char *> vocab;
//wraps readline and the history functions
std::string myread(const std::string &prompt = ::prompt) {
  char * line = readline(prompt.c_str());
  std::string res = line;
  if(line && *line)
    add_history(line);
  free(line);
  return res;
}
char* cmd_gen(const char * text, int state) {
  static int list_index, len;
  char *name = NULL;
  if(!state) {
    list_index = 0;
    len = strlen(text);
  }
  while((name = vocab[list_index])) {
    list_index++;
    if(strncmp(name, text, len) == 0)
      return strdup(name);
  }
  return (char*) NULL;
}
char ** completion(const char * text, int start, int end) {
  char ** matches;
  matches = (char **)NULL;
  matches = rl_completion_matches(text, cmd_gen);
  return matches;
}
//various readline function intitialization
void readInit() {
  using_history();
  rl_readline_name = "Calculator";
  rl_attempted_completion_function = completion;
}
int main(int argc, char *argv[]) {
  Calculator calculator;
  vocab = calculator.genVocab();
  readInit();
  mpf_set_default_prec(64);
  std::string input;
    while(true) {
      input = myread(prompt);
      if(input == "exit" || input[0] == 'q') break;
      try {
        calculator.parse(input);
        std::cout << "RPN: " << calculator.getRPN() << "\n";
        std::cout <<  std::setprecision(10) << calculator.evaluate() << "\n";
      } catch(const char * msg) {
        std::cerr << msg << "\n";
      }
    }
  std::cout << "hasta luego\n";
  return 0;
}