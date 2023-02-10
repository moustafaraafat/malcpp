CXXFLAGS ?= -g -O2 -Werror -Wall -Wextra -std=c++20

build: step0_repl step1_read_print step2_eval step3_env step4_if_fn_do

step0_repl: step0_repl.cpp
	$(CXX) $(CXXFLAGS) -o step0_repl step0_repl.cpp

step1_read_print: step1_read_print.cpp reader.cpp reader.h printer.cpp printer.h types.h
	$(CXX) $(CXXFLAGS) -o step1_read_print step1_read_print.cpp reader.cpp printer.cpp

step2_eval: step2_eval.cpp reader.cpp reader.h printer.cpp printer.h types.h
	$(CXX) $(CXXFLAGS) -o step2_eval step2_eval.cpp reader.cpp printer.cpp

step3_env: step3_env.cpp reader.cpp reader.h printer.cpp printer.h types.h env.h
	$(CXX) $(CXXFLAGS) -o step3_env step3_env.cpp reader.cpp printer.cpp

step4_if_fn_do: step4_if_fn_do.cpp reader.cpp reader.h printer.cpp printer.h types.h env.h
	$(CXX) $(CXXFLAGS) -o step4_if_fn_do step4_if_fn_do.cpp reader.cpp printer.cpp

