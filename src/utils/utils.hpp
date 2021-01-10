#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Register {
   public:
    string id;
    Register(string regId) : id(regId){};
};

class Registers {
   public:
    static Register A;
    static Register B;
    static Register C;
    static Register D;
    static Register E;
    static Register F;
};

class Instructions {
   public:
    static string GET(Register reg);
    static string PUT(Register reg);

    static string LOAD(Register reg1, Register reg2);
    static string STORE(Register reg1, Register reg2);

    static string ADD(Register reg1, Register reg2);
    static string SUB(Register reg1, Register reg2);
    
    static string RESET(Register reg);
    static string INC(Register reg);
    static string DEC(Register reg);
    static string SHR(Register reg);
    static string SHL(Register reg);

    static string JUMP(long j);
    static string JZERO(Register reg, long j);
    static string JODD(Register reg, long j);
};



void concatStringsVectors(vector<string>* base, vector<string>* toConcat);
string decToBin(long number);