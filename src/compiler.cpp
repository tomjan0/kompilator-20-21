#include "compiler.hpp"

vector<string> commands;
string output_filename;
ofstream file;
bool debug = false;

void test() {}

void set_output_filename(char* filename) { output_filename = filename; }

void open_file() { file.open(output_filename); }

vector<string> generateNumberInRegister(long number, Register reg) {
    vector<string> genInstructions;

    genInstructions.push_back(Instructions::RESET(reg));
    if (number > 0) {
        string binString = decToBin(number);
        for (long i = 0; i < binString.length() - 1; i++) {
            char curr = binString.at(i);
            if (curr == '1') {
                genInstructions.push_back(Instructions::INC(reg));
            }
            genInstructions.push_back(Instructions::SHL(reg));
        }
        if (binString.back() == '1') {
            genInstructions.push_back(Instructions::INC(reg));
        }
    }
    return genInstructions;
}

vector<string> dynamicArrayPositionToRegister(ArrayVariable* array,
                                              ValueVariable* index,
                                              Register reg) {
    vector<string> instructions;
    vector<string> genIndexMem = generateNumberInRegister(index->memoryId, reg);
    vector<string> genArrayMem =
        generateNumberInRegister(array->memoryId, Registers::E);
    vector<string> genArrayStart =
        generateNumberInRegister(array->startId, Registers::F);

    concatStringsVectors(&instructions, &genIndexMem);
    concatStringsVectors(&instructions, &genArrayMem);
    concatStringsVectors(&instructions, &genArrayStart);
    instructions.push_back(Instructions::LOAD(reg, reg));
    instructions.push_back(Instructions::ADD(reg, Registers::E));
    instructions.push_back(Instructions::SUB(reg, Registers::F));

    return instructions;
}

vector<string> tempConstToRegister(long value, Register reg) {
    vector<string> instructions;

    vector<string> genMemoryId =
        generateNumberInRegister(getSymbolTable()->getTempMemoryId(), reg);
    vector<string> genValue = generateNumberInRegister(value, Registers::F);

    concatStringsVectors(&instructions, &genMemoryId);
    concatStringsVectors(&instructions, &genValue);
    instructions.push_back(Instructions::STORE(Registers::F, reg));

    return instructions;
}

vector<string> valueAdressToRegister(Value* value, Register reg) {
    vector<string> instructions;
    switch (value->type) {
        case NUMBER: {
            auto numberValue = (NumberValue*)value;
            auto genInstructions =
                tempConstToRegister(numberValue->number, reg);

            concatStringsVectors(&instructions, &genInstructions);
            break;
        }
        case POINTER: {
            auto pointerValue = (PointerValue*)value;
            auto genInstructions =
                generateNumberInRegister(pointerValue->variable->memoryId, reg);

            concatStringsVectors(&instructions, &genInstructions);
            break;
        }
        case ARRAY_NUMBER: {
            auto arrayNumberValue = (ArrayNumberValue*)value;
            auto genInstructions = generateNumberInRegister(
                arrayNumberValue->array->getMemoryId(arrayNumberValue->index),
                reg);

            concatStringsVectors(&instructions, &genInstructions);
            break;
        }
        case ARRAY_POINTER: {
            auto arrayPointerValue = (ArrayPointerValue*)value;
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

void read(Value* value) {
    vector<string> readInstructions =
        valueAdressToRegister(value, Registers::A);
    readInstructions.push_back(Instructions::GET(Registers::A));

    value->initialize();
    // TODO its only for tests now
    concatStringsVectors(&commands, &readInstructions);
}

void write(Value* value) {
    if (value->isInitialized()) {
        vector<string> writeInstructions =
            valueAdressToRegister(value, Registers::A);
        writeInstructions.push_back(Instructions::PUT(Registers::A));

        // TODO its only for tests now
        concatStringsVectors(&commands, &writeInstructions);
    } else {
        error("Użycie niezainicializowanej zmiennej " + value->toString());
    }
}

vector<string> valueToRegister(Value* value, Register reg) {
    vector<string> instructions;
    switch (value->type) {
        case NUMBER: {
            auto numberValue = (NumberValue*)value;
            auto genInstructions =
                generateNumberInRegister(numberValue->number, reg);
            concatStringsVectors(&instructions, &genInstructions);
            break;
        }
        // case POINTER: {
        //     auto pointerValue = (PointerValue*)value;
        //     auto genInstructions = valueAdressToRegister(pointerValue, reg);
        //     concatStringsVectors(&instructions, &genInstructions);
        //     instructions.push_back(Instructions::LOAD(reg, reg));
        //     break;
        // }
        // case ARRAY_NUMBER: {
        //     auto arrayNumberValue = (ArrayNumberValue*)value;
        //     auto genInstructions = valueAdressToRegister(arrayNumberValue,
        //     reg); concatStringsVectors(&instructions, &genInstructions);
        //     instructions.push_back(Instructions::LOAD(reg, reg));
        //     break;
        // }
        // case ARRAY_POINTER: {
        //     auto arrayPointerValue = (ArrayPointerValue*)value;
        //     auto genInstructions = dynamicArrayPositionToRegister(
        //         arrayPointerValue->array, arrayPointerValue->indexVariable,
        //         reg);
        //     concatStringsVectors(&instructions, &genInstructions);
        //     instructions.push_back(Instructions::LOAD(reg, reg));
        //     break;
        // }
        default: {
            auto genInstructions = valueAdressToRegister(value, reg);
            concatStringsVectors(&instructions, &genInstructions);
            instructions.push_back(Instructions::LOAD(reg, reg));
            break;
        }
    }
    return instructions;
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
            auto rightInstructions =
                valueToRegister(expression->right, Registers::F);
            concatStringsVectors(&instructions, &leftInstructions);
            concatStringsVectors(&instructions, &rightInstructions);
            instructions.push_back(Instructions::ADD(reg, Registers::F));
            break;
        }
        case EX_SUB: {
            auto leftInstructions = valueToRegister(expression->left, reg);
            auto rightInstructions =
                valueToRegister(expression->right, Registers::F);
            concatStringsVectors(&instructions, &leftInstructions);
            concatStringsVectors(&instructions, &rightInstructions);
            instructions.push_back(Instructions::SUB(reg, Registers::F));
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
            instructions.push_back(
                Instructions::JZERO(A, 35 + loadRight.size() + 1));

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

            // LOOP START
            instructions.push_back(Instructions::JZERO(E, 12));
            instructions.push_back(Instructions::RESET(F));
            instructions.push_back(Instructions::INC(F));
            instructions.push_back(Instructions::ADD(F, A));
            instructions.push_back(Instructions::SUB(F, B));
            instructions.push_back(Instructions::SHL(Q));
            // if A < B JUMP
            instructions.push_back(Instructions::JZERO(F, 3));
            // A >= B
            instructions.push_back(Instructions::INC(Q));
            instructions.push_back(Instructions::SUB(A, B));
            // endif
            instructions.push_back(Instructions::SHR(B));
            instructions.push_back(Instructions::DEC(E));
            instructions.push_back(Instructions::JUMP(-11));

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
            instructions.push_back(
                Instructions::JZERO(A, 36 + loadRight.size() + 1));

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

            // LOOP START
            instructions.push_back(Instructions::JZERO(E, 13));
            instructions.push_back(Instructions::RESET(F));
            instructions.push_back(Instructions::INC(F));
            instructions.push_back(Instructions::ADD(F, A));
            instructions.push_back(Instructions::SUB(F, B));
            instructions.push_back(Instructions::SHL(Q));
            // if A < B JUMP
            instructions.push_back(Instructions::JZERO(F, 3));
            // A >= B
            instructions.push_back(Instructions::INC(Q));
            instructions.push_back(Instructions::SUB(A, B));
            // endif
            instructions.push_back(Instructions::SHR(B));
            instructions.push_back(Instructions::DEC(E));
            instructions.push_back(Instructions::JUMP(-11));

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

void assign(Value* identifierValue, Expression* expression) {
    string idName;
    switch (identifierValue->type) {
        case POINTER:
            idName = "POINTER";
            break;
        case ARRAY_NUMBER:
            idName = "ARRAY_NUMBER";
            break;
        case ARRAY_POINTER:
            idName = "ARRAY_POINTER";
            break;
        default:
            break;
    }
    cout << identifierValue->toString()
         << " := " << expression->left->toString() << " " << expression->type
         << " " << (expression->right ? expression->right->toString() : "")
         << endl;

    vector<string> assignInstructions;
    auto identifierInstructions =
        valueAdressToRegister(identifierValue, Registers::A);
    auto expressionInstructions =
        evalExpressionToRegister(expression, Registers::B);

    concatStringsVectors(&assignInstructions, &expressionInstructions);
    concatStringsVectors(&assignInstructions, &identifierInstructions);

    assignInstructions.push_back(
        Instructions::STORE(Registers::B, Registers::A));
    identifierValue->initialize();

    // TODO its only for tests now
    concatStringsVectors(&commands, &assignInstructions);
}

void printCommands() {
    if (debug) {
        for (int i = 0; i < commands.size(); i++) {
            cout << commands.at(i) << endl;
        }
    }
}

void writeCommands() {
    for (int i = 0; i < commands.size(); i++) {
        file << commands.at(i) << endl;
    }
    file << "HALT" << endl;
    file.close();
}