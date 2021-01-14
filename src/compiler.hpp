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

vector<string>* read(Value* value);
vector<string>* write(Value* value);
vector<string>* assign(Value* identifierValue, Expression* expression);
vector<string>* ifThen(Condition* condition, vector<string>* commands);
vector<string>* ifThenElse(Condition* condition, vector<string>* thenCommands, vector<string>* elseCommands);
vector<string>* whileDo(Condition* condition, vector<string>* commands);
vector<string>* repeatUntil(vector<string>* commands, Condition* condition);
void initFor(string identifier);
vector<string>* forTo(string identifier, Value* from, Value* to, vector<string>* commands);
vector<string>* forDownTo(string identifier, Value* from, Value* to, vector<string>* commands);

vector<string>* mergeInstructions(vector<string>* commands, vector<string>* command);

void writeCommands(vector<string>* commands);
void setOutputFilename(string filename);