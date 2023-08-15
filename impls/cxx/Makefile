CXXFLAGS=-std=c++17 -g -Wall -Wextra -Werror

all: step0_repl step1_read_print step2_eval step3_env

clean:
	rm -rf step0_repl step0_repl.dSYM step1_read_print step1_read_print.dSYM step2_eval step2_eval.dSYM step3_env step3_env.dSYM

step0_repl: step0_repl.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

step1_read_print: step1_read_print.cpp reader.cpp printer.cpp types.cpp string_helpers.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

step2_eval: step2_eval.cpp reader.cpp printer.cpp types.cpp string_helpers.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

step3_env: step3_env.cpp reader.cpp printer.cpp types.cpp string_helpers.cpp env.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^