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

void checkIterator(Value *value) {
    if (value->type == POINTER) {
        auto pointerValue = (PointerValue *)value;
        if (pointerValue->variable->iterator) {
            error("Iterator (" + pointerValue->variable->identifier + ") nie może być modyfikowany wewnątrz pętli");
        }
    }
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

vector<string> dynamicArrayPositionToRegister(ArrayVariable *array,
                                              ValueVariable *index,
                                              Register reg) {
    vector<string> instructions;
    vector<string> genIndexMem = generateNumberInRegister(index->memoryId, reg);
    vector<string> genArrayMem =
        generateNumberInRegister(array->memoryId, Registers::E);
    vector<string> genArrayStart =
        generateNumberInRegister(array->startId, Registers::F);

    concatStringsVectors(&instructions, &genIndexMem);
    instructions.push_back(Instructions::LOAD(reg, reg));
    concatStringsVectors(&instructions, &genArrayMem);
    concatStringsVectors(&instructions, &genArrayStart);
    instructions.push_back(Instructions::ADD(reg, Registers::E));
    instructions.push_back(Instructions::SUB(reg, Registers::F));

    return instructions;
}

vector<string> valueAdressToRegister(Value *value, Register reg) {
    vector<string> instructions;
    switch (value->type) {
        case NUMBER: {
            auto numberValue = (NumberValue *)value;
            auto genInstructions =
                tempConstToRegister(numberValue->number, reg);

            concatStringsVectors(&instructions, &genInstructions);
            break;
        }
        case POINTER: {
            auto pointerValue = (PointerValue *)value;
            auto genInstructions =
                generateNumberInRegister(pointerValue->variable->memoryId, reg);

            concatStringsVectors(&instructions, &genInstructions);
            break;
        }
        case ARRAY_NUMBER: {
            auto arrayNumberValue = (ArrayNumberValue *)value;
            auto genInstructions = generateNumberInRegister(
                arrayNumberValue->array->getMemoryId(arrayNumberValue->index),
                reg);

            concatStringsVectors(&instructions, &genInstructions);
            break;
        }
        case ARRAY_POINTER: {
            auto arrayPointerValue = (ArrayPointerValue *)value;
            auto genInstructions = dynamicArrayPositionToRegister(
                arrayPointerValue->array, arrayPointerValue->indexVariable,
                reg);
            concatStringsVectors(&instructions, &genInstructions);
            break;
        }
        default:
            break;
    }
    return instructions;
}

vector<string> valueToRegister(Value *value, Register reg) {
    vector<string> instructions;
    switch (value->type) {
        case NUMBER: {
            auto numberValue = (NumberValue *)value;
            auto genInstructions =
                generateNumberInRegister(numberValue->number, reg);
            concatStringsVectors(&instructions, &genInstructions);
            break;
        }
        default: {
            auto genInstructions = valueAdressToRegister(value, reg);
            concatStringsVectors(&instructions, &genInstructions);
            instructions.push_back(Instructions::LOAD(reg, reg));
            break;
        }
    }
    return instructions;
}