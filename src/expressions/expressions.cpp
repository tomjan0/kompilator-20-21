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

vector<string> evalExpressionToRegister(Expression* expression, Register reg) {
    vector<string> instructions;
    switch (expression->type) {
        case EX_SIMPLE: {
            auto leftInstructions = valueToRegister(expression->left, reg);
            concatStringsVectors(&instructions, &leftInstructions);
            break;
        }
        case EX_ADD: {
            auto leftInstructions = valueToRegister(expression->left, reg);
            auto rightInstructions = valueToRegister(expression->right, Registers::C);
            concatStringsVectors(&instructions, &leftInstructions);
            concatStringsVectors(&instructions, &rightInstructions);
            instructions.push_back(Instructions::ADD(reg, Registers::C));
            break;
        }
        case EX_SUB: {
            auto leftInstructions = valueToRegister(expression->left, reg);
            auto rightInstructions = valueToRegister(expression->right, Registers::C);
            concatStringsVectors(&instructions, &leftInstructions);
            concatStringsVectors(&instructions, &rightInstructions);
            instructions.push_back(Instructions::SUB(reg, Registers::C));
            break;
        }
        case EX_MUL: {
            auto A = reg;
            auto B = Registers::C;
            auto Q = Registers::D;

            auto loadLeft = valueToRegister(expression->left, B);
            auto loadRight = valueToRegister(expression->right, Q);
            concatStringsVectors(&instructions, &loadLeft);
            concatStringsVectors(&instructions, &loadRight);
            instructions.push_back(Instructions::RESET(A));

            instructions.push_back(Instructions::JZERO(B, 9));  // 1
            instructions.push_back(Instructions::JZERO(Q, 8));  // 2
            instructions.push_back(Instructions::INC(Q));       // 3
            instructions.push_back(Instructions::JODD(Q, 2));   // 4
            instructions.push_back(Instructions::ADD(A, B));    // 5
            instructions.push_back(Instructions::DEC(Q));       // 6
            instructions.push_back(Instructions::SHL(B));       // 7
            instructions.push_back(Instructions::SHR(Q));       // 8
            instructions.push_back(Instructions::JUMP(-7));     // 9

            break;
        }
        case EX_DIV: {
            auto A = Registers::D;
            auto B = Registers::C;
            auto Q = reg;
            auto D = Registers::A;
            auto E = Registers::E;
            auto F = Registers::F;

            auto loadLeft = valueToRegister(expression->left, A);
            auto loadRight = valueToRegister(expression->right, B);
            concatStringsVectors(&instructions, &loadLeft);
            instructions.push_back(Instructions::RESET(Q));
            instructions.push_back(Instructions::JZERO(A, 35 + loadRight.size() + 1));

            concatStringsVectors(&instructions, &loadRight);
            instructions.push_back(Instructions::JZERO(B, 35));

            instructions.push_back(Instructions::RESET(D));
            instructions.push_back(Instructions::RESET(E));
            instructions.push_back(Instructions::RESET(F));

            // D = len(A)
            instructions.push_back(Instructions::ADD(E, A));
            instructions.push_back(Instructions::JZERO(E, 4));
            instructions.push_back(Instructions::SHR(E));
            instructions.push_back(Instructions::INC(D));
            instructions.push_back(Instructions::JUMP(-3));

            // F = len(B)
            instructions.push_back(Instructions::ADD(E, B));
            instructions.push_back(Instructions::JZERO(E, 4));
            instructions.push_back(Instructions::SHR(E));
            instructions.push_back(Instructions::INC(F));
            instructions.push_back(Instructions::JUMP(-3));

            // D = D - F = len(A) - len(B)
            instructions.push_back(Instructions::SUB(D, F));

            // E = D + 1 = len(A) - len(B) + 1
            instructions.push_back(Instructions::RESET(E));
            instructions.push_back(Instructions::ADD(E, D));
            instructions.push_back(Instructions::INC(E));

            // aligin B to A to the left
            instructions.push_back(Instructions::JZERO(D, 4));
            instructions.push_back(Instructions::SHL(B));
            instructions.push_back(Instructions::DEC(D));
            instructions.push_back(Instructions::JUMP(-3));

            instructions.push_back(Instructions::RESET(Q));

            // loop start
            instructions.push_back(Instructions::JZERO(E, 12));
            instructions.push_back(Instructions::RESET(F));
            instructions.push_back(Instructions::INC(F));
            instructions.push_back(Instructions::ADD(F, A));
            instructions.push_back(Instructions::SUB(F, B));
            instructions.push_back(Instructions::SHL(Q));
            // if A < B
            instructions.push_back(Instructions::JZERO(F, 3));
            // else
            instructions.push_back(Instructions::INC(Q));
            instructions.push_back(Instructions::SUB(A, B));
            // endif
            instructions.push_back(Instructions::SHR(B));
            instructions.push_back(Instructions::DEC(E));
            instructions.push_back(Instructions::JUMP(-11));
            // loop end

            break;
        }
        case EX_MOD: {
            auto A = reg;
            auto B = Registers::C;
            auto Q = Registers::D;
            auto D = Registers::A;
            auto E = Registers::E;
            auto F = Registers::F;

            auto loadLeft = valueToRegister(expression->left, A);
            auto loadRight = valueToRegister(expression->right, B);
            concatStringsVectors(&instructions, &loadLeft);
            instructions.push_back(Instructions::RESET(Q));
            instructions.push_back(Instructions::JZERO(A, 36 + loadRight.size() + 1));

            concatStringsVectors(&instructions, &loadRight);
            instructions.push_back(Instructions::JZERO(B, 35));

            instructions.push_back(Instructions::RESET(D));
            instructions.push_back(Instructions::RESET(E));
            instructions.push_back(Instructions::RESET(F));

            // D = len(A)
            instructions.push_back(Instructions::ADD(E, A));
            instructions.push_back(Instructions::JZERO(E, 4));
            instructions.push_back(Instructions::SHR(E));
            instructions.push_back(Instructions::INC(D));
            instructions.push_back(Instructions::JUMP(-3));

            // F = len(B)
            instructions.push_back(Instructions::ADD(E, B));
            instructions.push_back(Instructions::JZERO(E, 4));
            instructions.push_back(Instructions::SHR(E));
            instructions.push_back(Instructions::INC(F));
            instructions.push_back(Instructions::JUMP(-3));

            // D = D - F = len(A) - len(B)
            instructions.push_back(Instructions::SUB(D, F));

            // E = D + 1 = len(A) - len(B) + 1
            instructions.push_back(Instructions::RESET(E));
            instructions.push_back(Instructions::ADD(E, D));
            instructions.push_back(Instructions::INC(E));

            // aligin B to A to the left
            instructions.push_back(Instructions::JZERO(D, 4));
            instructions.push_back(Instructions::SHL(B));
            instructions.push_back(Instructions::DEC(D));
            instructions.push_back(Instructions::JUMP(-3));

            instructions.push_back(Instructions::RESET(Q));

            // loop start
            instructions.push_back(Instructions::JZERO(E, 13));
            instructions.push_back(Instructions::RESET(F));
            instructions.push_back(Instructions::INC(F));
            instructions.push_back(Instructions::ADD(F, A));
            instructions.push_back(Instructions::SUB(F, B));
            instructions.push_back(Instructions::SHL(Q));
            // if A < B
            instructions.push_back(Instructions::JZERO(F, 3));
            // else
            instructions.push_back(Instructions::INC(Q));
            instructions.push_back(Instructions::SUB(A, B));
            // endif
            instructions.push_back(Instructions::SHR(B));
            instructions.push_back(Instructions::DEC(E));
            instructions.push_back(Instructions::JUMP(-11));
            // loop end

            // reset if B == 0
            instructions.push_back(Instructions::RESET(A));
            break;
        }
        default: {
            auto leftInstructions = valueToRegister(expression->left, reg);
            concatStringsVectors(&instructions, &leftInstructions);
            break;
        }
    }
    return instructions;
}
