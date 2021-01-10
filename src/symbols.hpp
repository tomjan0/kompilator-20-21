#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void error(string str);

enum SymbolType { VAR, CONST };
enum VariableType { VAL, ARR };

/* Record of symbol table. Can be a variable - simple and array or const */
class Symbol {
   public:
    SymbolType type;
    long memoryId;
    string identifier;
};

/* Variable type symbol */
class Variable : public Symbol {
   public:
    SymbolType type;
    VariableType varType;
};

/* Holds value */
class ValueVariable : public Variable {
   public:
    long value;
    bool initialized;
};

/* Holds information about array */
class ArrayVariable : public Variable {
   public:
    vector<long> initializedIds;
    long length;
    long startIdx;
};

/* Holds all registered symbols, both variables and constant values */
class SymbolTable {
   public:
    vector<Symbol*> records;
    long long int occupiedMemory;

    void addValueVariable(string identifier) {
        if (getSymbolIdx(identifier) == -1) {
            ValueVariable* newVar = new ValueVariable;
            newVar->type = VAR;
            newVar->identifier = identifier;
            newVar->memoryId = occupiedMemory;
            newVar->value = -1;
            newVar->varType = VAL;
            records.push_back(newVar);

            occupiedMemory++;
        } else {
            error("Zmienna '" + identifier + "' została już zadeklarowana");
        }
    }

    void addArrayVariable(string identifier, long startIdx, long endIdx) {
        if (getSymbolIdx(identifier) == -1) {
            if (startIdx >= 0 && endIdx >= startIdx) {
                ArrayVariable* newVar = new ArrayVariable;
                newVar->type = VAR;
                newVar->identifier = identifier;
                newVar->memoryId = occupiedMemory;
                newVar->startIdx = startIdx;
                newVar->length = endIdx - startIdx + 1;
                newVar->varType = ARR;
                records.push_back(newVar);

                occupiedMemory += newVar->length;
            } else {
                error("Nieprawiłowy zakres tablicy '" + identifier + "'");
            }
        } else {
            error("Zmienna '" + identifier + "' została już zadeklarowana");
        }
    }

    long getSymbolIdx(string identifier) {
        for (long i = 0; i < records.size(); i++) {
            if (records.at(i)->identifier == identifier) {
                return i;
            }
        }
        return -1;
    }

    bool symbolDeclared(string identifier) {
        return getSymbolIdx(identifier) >= 0;
    }

    bool variableDeclared(string identifier) {
        long id = getSymbolIdx(identifier);
        return id >= 0 && records.at(id)->type == VAR;
    }

    Symbol* getSymbol(string identifier) {
        long id = getSymbolIdx(identifier);
        return id >= 0 ? records[id] : nullptr;
    }

    Variable* getVariable(string identifier) {
        return reinterpret_cast<Variable*>(getSymbol(identifier));
    }
};

SymbolTable* getSymbolTable();

void error(string str);
void error(string str, int lineno);

void scopeInvoke();
void scopeRevoke();