#pragma once

#include <string.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "symbols.hpp"
#include "values.hpp"

using namespace std;

void read(Value* value);
void write(Value* value);
void printCommands();
void writeCommands();
void set_output_filename(char*);
void open_file();