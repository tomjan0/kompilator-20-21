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
        if (variable->varType == ARRAY) {
            ArrayVariable *array = (ArrayVariable *)variable;
            if (index >= array->startId &&
                index <= array->startId + array->length - 1) {
                ArrayNumberValue *value = new ArrayNumberValue;
                value->type = ARRAY_NUMBER;
                value->array = array;
                value->index = index;
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
        if (variable->varType == ARRAY) {
            PointerValue *indexValue = getValue(indexIdentifier);
            if (indexValue) {
                ValueVariable *indexVariable = indexValue->variable;
                if (indexVariable->initialized) {
                    ArrayPointerValue *value = new ArrayPointerValue;
                    value->type = ARRAY_POINTER;
                    value->array = (ArrayVariable *)variable;
                    value->indexVariable = indexVariable;
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

bool checkValuesInitialization(Value *value) {
    if (value->isInitialized()) {
        return true;
    } else {
        valueNotInitialized(value);
    }
    return false;
}

bool checkValuesInitialization(Value *left, Value *right) {
    if (left->isInitialized()) {
        if (right->isInitialized()) {
            return true;
        } else {
            valueNotInitialized(right);
        }
    } else {
        valueNotInitialized(left);
    }
    return false;
}

void valueNotInitialized(Value *value) {
    error("Użycie niezainicjalizowanej zmiennej " + value->toString());
}