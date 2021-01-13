#pragma once

#include "../symbols/symbols.hpp"
#include "../utils/utils.hpp"

using namespace std;

enum ValueType { NUMBER, POINTER, ARRAY_NUMBER, ARRAY_POINTER };

class Value {
   public:
    ValueType type;
    virtual bool isInitialized() { return true; }
    virtual void initialize() {}
    virtual string toString() = 0;
};

class NumberValue : public Value {
   public:
    long number;
    string toString() { return to_string(number); }
};

class PointerValue : public Value {
   public:
    ValueVariable* variable;
    bool isInitialized() {
        cout << "check ponter" << endl;
        return variable->initialized;
    }
    void initialize() {
        cout << "init pointer" << endl;
        variable->initialized = true;
    }
    string toString() { return variable->identifier; };
};

class ArrayNumberValue : public Value {
   public:
    ArrayVariable* array;
    long index;
    bool isInitialized() {
        cout << "check array number " << endl;
        return array->isInitialized(index);
    }
    void initialize() {
        cout << "init array number " << endl;
        array->initalizeId(index);
    }
    string toString() {
        return array->identifier + "(" + to_string(index) + ")";
    };
};

class ArrayPointerValue : public Value {
   public:
    ArrayVariable* array;
    ValueVariable* indexVariable;
    string toString() {
        return array->identifier + "(" + indexVariable->identifier + ")";
    }
};

PointerValue* getValue(string identifier);
NumberValue* getValue(long number);
ArrayNumberValue* getValue(string identifier, long index);
ArrayPointerValue* getValue(string arrayIdentifier, string indexIdentifier);

void checkIterator(Value *value);
bool checkValuesInitialization(Value* value);
bool checkValuesInitialization(Value* left, Value* right);
void valueNotInitialized(Value* value);

vector<string> dynamicArrayPositionToRegister(ArrayVariable* array,
                                              ValueVariable* index,
                                              Register reg);
vector<string> valueAdressToRegister(Value* value, Register reg);
vector<string> valueToRegister(Value* value, Register reg);