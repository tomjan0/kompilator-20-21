#pragma once

#include <string.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "symbols.hpp"
#include "utils/utils.hpp"
#include "values/values.hpp"
#include "expressions/expressions.hpp"

using namespace std;

void test();

void read(Value* value);
void write(Value* value);
void assign(Value* identifierValue, Expression* expression);

void printCommands();
void writeCommands();
void set_output_filename(char*);
void open_file();