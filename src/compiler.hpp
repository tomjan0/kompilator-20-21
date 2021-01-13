#pragma once

#include <string.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "conditions/conditions.hpp"
#include "expressions/expressions.hpp"
#include "symbols/symbols.hpp"
#include "utils/utils.hpp"
#include "values/values.hpp"

using namespace std;

void test();

vector<string>* read(Value* value);
vector<string>* write(Value* value);
vector<string>* assign(Value* identifierValue, Expression* expression);
vector<string>* ifThen(Condition* condition, vector<string>* commands);
vector<string>* ifThenElse(Condition* condition, vector<string>* thenCommands,
                           vector<string>* elseCommands);
vector<string>* mergeInstructions(vector<string>* commands,
                                  vector<string>* command);

void printCommands();
void writeCommands();
void writeCommands(vector<string>* cmds);
void set_output_filename(char*);
void open_file();