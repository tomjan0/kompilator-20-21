#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void error(string str);

enum SymbolType { VAR, CONST };
enum VariableType { VAL, ARRAY};

/* Record of symbol table. Can be a variable or const */
class Symbol {
   public:
    SymbolType type;
    long memoryId;
    string identifier;
};

/* Variable type symbol */
class Variable : public Symbol {
   public:
    VariableType varType;
};

/* Holds value */
class ValueVariable : public Variable {
   public:
    long value;
    bool initialized;
    bool iterator;
};

/* Holds information about array */
class ArrayVariable : public Variable {
   public:
    vector<long> initializedIds;
    long length;
    long startId;

    bool isInitialized(long id) {
        for (long i = 0; i < initializedIds.size(); i++) {
            if (initializedIds.at(i) == id) {
                return true;
            }
        }
        return false;
    }

    void initalizeId(long id) {
        if (!isInitialized(id)) {
            initializedIds.push_back(id);
        }
    }

    long getMemoryId(long id) { return memoryId + id - startId; }
};

/* Holds all registered symbols, both variables and constant values */
class SymbolTable {
   public:
    vector<Symbol*> *records;
    long long int occupiedMemory;
    SymbolTable() {
        this->records = new vector<Symbol*>;
    }

    void addValueVariable(string identifier) {
        if (getSymbolIdx(identifier) == -1) {
            ValueVariable* newVar = new ValueVariable;
            newVar->type = VAR;
            newVar->identifier = identifier;
            newVar->memoryId = occupiedMemory;
            newVar->value = -1;
            newVar->varType = VAL;
            newVar->initialized = false;
            newVar->iterator = false;
            records->push_back(newVar);
            cout << "add value var -- "<<identifier<<" -- "<<newVar->type<< " ---- "; 
            cout << records->back()->type<<endl;
            occupiedMemory++;
        } else {
            error("Zmienna '" + identifier + "' została już zadeklarowana");
        }
    }

    void addIteratorVariable(string identifier) {
        addValueVariable(identifier);
        auto iterator = ((ValueVariable*)getVariable(identifier));
        iterator->initialized = true;
        iterator->iterator = true;
    }

    void addArrayVariable(string identifier, long startId, long endId) {
        if (getSymbolIdx(identifier) == -1) {
            if (startId >= 0 && endId >= startId) {
                ArrayVariable* newVar = new ArrayVariable;
                newVar->type = VAR;
                newVar->identifier = identifier;
                newVar->memoryId = occupiedMemory;
                newVar->startId = startId;
                newVar->length = endId - startId + 1;
                newVar->varType = ARRAY;
                records->push_back(newVar);

                occupiedMemory += newVar->length;
            } else {
                error("Nieprawiłowy zakres tablicy '" + identifier + "'");
            }
        } else {
            error("Zmienna '" + identifier + "' została już zadeklarowana");
        }
    }

    long getSymbolIdx(string identifier) {
        cout << "READ "<<identifier<<endl;
        cout << "VEC SIZE "<<records->size()<<endl;
        long i =0;
        // for(auto record: records) {
        //     cout<<record->identifier<<" "<<i<<endl;
        
        //     if(record->identifier == identifier) {
        //         cout<< "HELLLO"<<endl;
        //         return i;
        //     }
        //     i++;
        // }
        for (vector<string>::size_type i = 0; i != records->size(); i++) {
            cout<<"i: "<<i<<endl;
            if (records->at(i)->identifier == identifier) {
                cout<<" i 2: "<<i<<endl;
                return i;
            }
        }
        return -1;
    }

    bool symbolDeclared(string identifier) {
        return getSymbolIdx(identifier) >= 0;
    }

    bool variableDeclared(string identifier) {
        cout << "LOKING FOR " << identifier << " ---- ";
        long id = getSymbolIdx(identifier);
        cout << "FOUND " << id << " ---- ";
        // cout << "TYPE " << records->at(id)->type << "----" <<endl;
        if (id >= 0 && records->at(id)->type == VAR) {
            cout << "CHECK TRUE " << endl;
        }
        return id >= 0 && records->at(id)->type == VAR;
    }

    Symbol* getSymbol(string identifier) {
        long id = getSymbolIdx(identifier);
        return id >= 0 ? records->at(id) : nullptr;
    }

    Variable* getVariable(string identifier) {
        return (Variable*)getSymbol(identifier);
    }

    long getTempMemoryId() { return occupiedMemory; }
};

SymbolTable* getSymbolTable();

void scopeInvoke();
void scopeRevoke();