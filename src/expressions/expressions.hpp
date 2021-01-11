#pragma once

#include "../values/values.hpp"
enum ExpressionType { EX_SIMPLE, EX_ADD, EX_SUB, EX_MUL, EX_DIV, EX_MOD };

class Expression {
   public:
    ExpressionType type;
    Value* left;
    Value* right;
    Expression(ExpressionType type, Value* left) {
        this->type = type;
        this->left = left;
        this->right = nullptr;
    }
    Expression(ExpressionType type, Value* left, Value* right) {
        this->type = type;
        this->left = left;
        this->right = right;
    }
};

Expression* getSimpleExpression(Value* value);
Expression* getAddExpression(Value* left, Value* right);
Expression* getSubExpression(Value* left, Value* right);
Expression* getMulExpression(Value* left, Value* right);
Expression* getDivExpression(Value* left, Value* right);
Expression* getModExpression(Value* left, Value* right);
