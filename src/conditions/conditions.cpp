#include "conditions.hpp"

Condition* getCondition(ConditionType type, Value* left, Value* right) {
    if (checkValuesInitialization(left, right)) {
        return new Condition(type, left, right);
    }
    return nullptr;
}

Condition* getEqCondition(Value* left, Value* right) { return getCondition(COND_EQ, left, right); }
Condition* getNeqCondition(Value* left, Value* right) { return getCondition(COND_NEQ, left, right); }
Condition* getLeCondition(Value* left, Value* right) { return getCondition(COND_LE, left, right); }
Condition* getGeCondition(Value* left, Value* right) { return getCondition(COND_GE, left, right); }
Condition* getLeqCondition(Value* left, Value* right) { return getCondition(COND_LEQ, left, right); }
Condition* getGeqCondition(Value* left, Value* right) { return getCondition(COND_GEQ, left, right); }

vector<string> evalConditionToRegister(Condition* condition, Register reg) {
    vector<string> instructions;
    if (condition->type == COND_GE || condition->type == COND_GEQ || condition->type == COND_LE ||
        condition->type == COND_LEQ) {
        auto left = condition->left;
        auto right = condition->right;

        if (condition->type == COND_GE || condition->type == COND_GEQ) {
            left = condition->right;
            right = condition->left;
        }

        auto leftInstructions = valueToRegister(left, reg);
        auto rightInstructions = valueToRegister(right, Registers::B);

        concatStringsVectors(&instructions, &leftInstructions);
        concatStringsVectors(&instructions, &rightInstructions);
        if (condition->type == COND_LE || condition->type == COND_GE) {
            instructions.push_back(Instructions::INC(reg));
        }
        instructions.push_back(Instructions::SUB(reg, Registers::B));
        instructions.push_back(Instructions::JZERO(reg, 3));
        instructions.push_back(Instructions::RESET(reg));
        instructions.push_back(Instructions::JUMP(2));
        instructions.push_back(Instructions::INC(reg));
    } else if (condition->type == COND_EQ) {
        condition->type = COND_GEQ;
        auto leftGEQright = evalConditionToRegister(condition, reg);
        condition->type = COND_LEQ;
        auto leftLEQright = evalConditionToRegister(condition, reg);
        condition->type = COND_EQ;

        concatStringsVectors(&instructions, &leftGEQright);
        instructions.push_back(Instructions::JZERO(reg, leftLEQright.size() + 3));
        concatStringsVectors(&instructions, &leftLEQright);
        instructions.push_back(Instructions::JZERO(reg, 2));
        instructions.push_back(Instructions::JUMP(2));
        instructions.push_back(Instructions::RESET(reg));
    } else {  // COND_NEQ
        condition->type = COND_GE;
        auto leftGEright = evalConditionToRegister(condition, reg);
        condition->type = COND_LE;
        auto leftLEright = evalConditionToRegister(condition, reg);
        condition->type = COND_NEQ;

        concatStringsVectors(&instructions, &leftGEright);
        instructions.push_back(Instructions::JZERO(reg, 2));
        instructions.push_back(Instructions::JUMP(leftLEright.size() + 4));
        concatStringsVectors(&instructions, &leftLEright);
        instructions.push_back(Instructions::JZERO(reg, 2));
        instructions.push_back(Instructions::JUMP(2));
        instructions.push_back(Instructions::RESET(reg));
    }
    return instructions;
}
