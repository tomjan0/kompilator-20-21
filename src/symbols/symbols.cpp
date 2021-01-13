#include "symbols.hpp"

SymbolTable symbolTable;
int scopeDepth = 0;

void scopeInvoke() {
    scopeDepth++;
    cout << "Scope depth: " << scopeDepth << endl;
}

void scopeRevoke() {
    scopeDepth--;
    cout << "Scope depth: " << scopeDepth << endl;
}

SymbolTable *getSymbolTable() { return &symbolTable; }
