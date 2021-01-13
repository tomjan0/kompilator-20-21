#pragma once

#include "../values/values.hpp"
enum ConditionType { COND_EQ, COND_NEQ, COND_LE, COND_GE, COND_LEQ, COND_GEQ };

class Condition {
   public:
    ConditionType type;
    Value* left;
    Value* right;
    Condition(ConditionType type, Value* left, Value* right) {
        this->type = type;
        this->left = left;
        this->right = right;
    }
};

Condition* getEqCondition(Value* left, Value* right);
Condition* getNeqCondition(Value* left, Value* right);
Condition* getLeCondition(Value* left, Value* right);
Condition* getGeCondition(Value* left, Value* right);
Condition* getLeqCondition(Value* left, Value* right);
Condition* getGeqCondition(Value* left, Value* right);


vector<string> evalConditionToRegister(Condition* condition, Register reg);