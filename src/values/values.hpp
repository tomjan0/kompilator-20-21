#pragma once
#include "../symbols.hpp"

using namespace std;

enum ValueType { NUMBER, POINTER, ARRAY_NUMBER, ARRAY_POINTER };

class Value {
   public:
    ValueType type;
};

class NumberValue : public Value {
   public:
    long number;
};

class PointerValue : public Value {
   public:
    ValueVariable* variable;
};

class ArrayPointerValue : public Value {
   public:
    ArrayVariable* array;
    ValueVariable* indexVariable;
};

class ArrayNumberValue : public Value {
   public:
    ArrayVariable* array;
    long index;
};

PointerValue* getValue(string identifier);
NumberValue* getValue(long number);
ArrayNumberValue* getValue(string identifier, long index);
ArrayPointerValue* getValue(string arrayIdentifier, string indexIdentifier);