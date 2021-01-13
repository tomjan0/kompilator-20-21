#include "conditions.hpp"

Condition* getCondition(ConditionType type, Value* left, Value* right) {
    if (checkValuesInitialization(left, right)) {
        return new Condition(type, left, right);
    }
    return nullptr;
}

Condition* getEqCondition(Value* left, Value* right) {
    return getCondition(COND_EQ, left, right);
}
Condition* getNeqCondition(Value* left, Value* right) {
    return getCondition(COND_NEQ, left, right);
}
Condition* getLeCondition(Value* left, Value* right) {
    return getCondition(COND_LE, left, right);
}
Condition* getGeCondition(Value* left, Value* right) {
    return getCondition(COND_GE, left, right);
}
Condition* getLeqCondition(Value* left, Value* right) {
    return getCondition(COND_LEQ, left, right);
}
Condition* getGeqCondition(Value* left, Value* right) {
    return getCondition(COND_GEQ, left, right);
}

vector<string> evalConditionToRegister(Condition* condition, Register reg) {
    vector<string> instructions;
    switch (condition->type) {
        case COND_EQ: {
            /* code */
            break;
        }
        case COND_NEQ: {
            /* code */
            break;
        }
        case COND_LE: {
            /* code */
            break;
        }
        case COND_GE: {
            /* code */
            break;
        }
        case COND_LEQ: {
            /* code */
            break;
        }
        case COND_GEQ: {
            /* code */
            break;
        }

        default:
            break;
    }
    return instructions;
}
