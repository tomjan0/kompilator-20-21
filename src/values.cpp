#include "values.hpp"

/* Value for just a number */
NumberValue *getValue(long number) {
    if (number >= 0) {
        NumberValue *value = new NumberValue;
        value->type = NUMBER;
        value->number = number;
        return value;
    } else {
        error("Niedozwolona wartość: '" + to_string(number) + "'");
    }
    return nullptr;
}

/* Value for variable */
PointerValue *getValue(string identifier) {
    if (getSymbolTable()->variableDeclared(identifier)) {
        Variable *variable = getSymbolTable()->getVariable(identifier);
        if (variable->varType == VAL) {
            PointerValue *value = new PointerValue;
            value->type = POINTER;
            value->variable = (ValueVariable *)variable;
            return value;
        } else {
            error("Zmienna '" + identifier + "' jest tablicą");
        }
    } else {
        error("Użycie niezadeklarowanej zmiennej '" + identifier + "'");
    }
    return nullptr;
}

/* Value for array with number as index */
ArrayNumberValue *getValue(string identifier, long index) {
    if (getSymbolTable()->variableDeclared(identifier)) {
        Variable *variable = getSymbolTable()->getVariable(identifier);
        if (variable->varType == ARR) {
            ArrayVariable *array = (ArrayVariable *)variable;
            if (index >= array->startIdx &&
                index <= array->startIdx + array->length - 1) {
                ArrayNumberValue *value = new ArrayNumberValue;
                value->type = ARRAY_NUMBER;
                value->array = array;
                return value;
            } else {
                error("Nieprawidłowy indeks '" + to_string(index) +
                      "' dla tablicy '" + identifier + "'");
            }
        } else {
            error("Zmienna '" + identifier + "' nie jest tablicą");
        }
    } else {
        error("Użycie niezadeklarowanej tablicy '" + identifier + "'");
    }
    return nullptr;
}

/* Value for array with variable as index */
ArrayPointerValue *getValue(string arrayIdentifier, string indexIdentifier) {
    if (getSymbolTable()->variableDeclared(arrayIdentifier)) {
        Variable *variable = getSymbolTable()->getVariable(arrayIdentifier);
        if (variable->varType == ARR) {
            PointerValue *indexValue = getValue(indexIdentifier);
            if (indexValue) {
                ValueVariable *indexVariable = indexValue->variable;
                if (indexVariable->initialized) {
                    ArrayPointerValue *value = new ArrayPointerValue;
                    value->type = ARRAY_POINTER;
                    value->array = (ArrayVariable *)variable;
                    return value;
                } else {
                    error("Użycie niezainicjalizowanej zmiennej '" +
                          indexIdentifier + "'");
                }
            }
        } else {
            error("Zmienna '" + arrayIdentifier + "' nie jest tablicą");
        }
    } else {
        error("Użycie niezadeklarowanej tablicy '" + arrayIdentifier + "'");
    }
    return nullptr;
}