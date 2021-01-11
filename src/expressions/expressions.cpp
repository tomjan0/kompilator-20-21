#include "expressions.hpp"

Expression* getSimpleExpression(Value* value) {
    if (checkValuesInitialization(value)) {
        return new Expression(EX_SIMPLE, value);
    }
    return nullptr;
}
Expression* getAddExpression(Value* left, Value* right) {
    if (checkValuesInitialization(left, right)) {
        return new Expression(EX_ADD, left, right);
    }
    return nullptr;
}
Expression* getSubExpression(Value* left, Value* right) {
    if (checkValuesInitialization(left, right)) {
        return new Expression(EX_SUB, left, right);
    }
    return nullptr;
}
Expression* getMulExpression(Value* left, Value* right) {
    if (checkValuesInitialization(left, right)) {
        return new Expression(EX_MUL, left, right);
    }
    return nullptr;
}
Expression* getDivExpression(Value* left, Value* right) {
    if (checkValuesInitialization(left, right)) {
        return new Expression(EX_DIV, left, right);
    }
    return nullptr;
}
Expression* getModExpression(Value* left, Value* right) {
    if (checkValuesInitialization(left, right)) {
        return new Expression(EX_MOD, left, right);
    }
    return nullptr;
}
