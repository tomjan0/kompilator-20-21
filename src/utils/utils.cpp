#include "utils.hpp"

using namespace std;

/* Define registers */
Register Registers::A = Register("a");
Register Registers::B = Register("b");
Register Registers::C = Register("c");
Register Registers::D = Register("d");
Register Registers::E = Register("e");
Register Registers::F = Register("f");

/* Define instructions */
string Instructions::GET(Register reg) { return "GET " + reg.id; }

string Instructions::PUT(Register reg) { return "PUT " + reg.id; };

string Instructions::LOAD(Register reg1, Register reg2) {
    return "LOAD " + reg1.id + " " + reg2.id;
};
string Instructions::STORE(Register reg1, Register reg2) {
    return "STORE " + reg1.id + " " + reg2.id;
};

string Instructions::ADD(Register reg1, Register reg2) {
    return "ADD " + reg1.id + " " + reg2.id;
};
string Instructions::SUB(Register reg1, Register reg2) {
    return "SUB " + reg1.id + " " + reg2.id;
};

string Instructions::RESET(Register reg) { return "RESET " + reg.id; };
string Instructions::INC(Register reg) { return "INC " + reg.id; };
string Instructions::DEC(Register reg) { return "DEC " + reg.id; };
string Instructions::SHR(Register reg) { return "SHR " + reg.id; };
string Instructions::SHL(Register reg) { return "SHL " + reg.id; };

string Instructions::JUMP(long j) { return "JUMP " + to_string(j); };
string Instructions::JZERO(Register reg, long j) {
    return "JZERO " + reg.id + " " + to_string(j);
};
string Instructions::JODD(Register reg, long j) {
    return "JODD " + reg.id + " " + to_string(j);
};

void concatStringsVectors(vector<string>* base, vector<string>* toConcat) {
    base->insert(base->end(), toConcat->begin(), toConcat->end());
}

string decToBin(long number) {
    string binString;
    while (number > 0) {
        binString = (number % 2 == 0 ? "0" : "1") + binString;
        number /= 2;
    }
    return binString;
}