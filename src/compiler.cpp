#include "compiler.hpp"

vector<string> commands;
string output_filename;
ofstream file;

void set_output_filename(char* filename) { output_filename = filename; }

void open_file() { file.open(output_filename); }

vector<string> generateNumberInRegister(long number, string reg) {
    vector<string> res;
    res.push_back("RESET " + reg);
    for (int i = 0; i < number; i++) {
        res.push_back("INC " + reg);
    }
    return res;
}

void read(Value* value) {
    vector<string> cmds;
    if (value->type == POINTER) {
        PointerValue* pvalue = (PointerValue*)value;
        vector<string> genCmds =
            generateNumberInRegister(pvalue->variable->memoryId, "a");
        for (int i = 0; i < genCmds.size(); i++) {
            cmds.push_back(genCmds.at(i));
        }
        cmds.push_back("GET a");
    }

    for (int i = 0; i < cmds.size(); i++) {
        commands.push_back(cmds.at(i));
    }
}

void write(Value* value) {
    vector<string> cmds;
    if (value->type == POINTER) {
        PointerValue* pvalue = (PointerValue*)value;
        vector<string> genCmds =
            generateNumberInRegister(pvalue->variable->memoryId, "a");
        for (int i = 0; i < genCmds.size(); i++) {
            cmds.push_back(genCmds.at(i));
        }
        cmds.push_back("PUT a");
    }

    for (int i = 0; i < cmds.size(); i++) {
        commands.push_back(cmds.at(i));
    }
}

void printCommands() {
    for (int i = 0; i < commands.size(); i++) {
        cout << commands.at(i) << endl;
    }
}

void writeCommands() {
    for (int i = 0; i < commands.size(); i++) {
        file << commands.at(i) << endl;
    }
    file << "HALT" << endl;
    file.close();
}