#include "compiler.hpp"

vector<string> commands;
string output_filename;
ofstream file;
bool debug = true;

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

void read(Value* value) {
    vector<string> readInstructions;
    Register mainReg = Registers::A;
    switch (value->type) {
        case POINTER: {
            auto pointerValue = (PointerValue*)value;
            pointerValue->variable->initialized = true;
            auto genInstructions = generateNumberInRegister(
                pointerValue->variable->memoryId, mainReg);

            concatStringsVectors(&readInstructions, &genInstructions);
            break;
        }
        case ARRAY_NUMBER: {
            auto arrayNumberValue = (ArrayNumberValue*)value;
            arrayNumberValue->array->initalizeId(arrayNumberValue->index);
            auto genInstructions = generateNumberInRegister(
                arrayNumberValue->array->getMemoryId(arrayNumberValue->index),
                mainReg);

            concatStringsVectors(&readInstructions, &genInstructions);
            break;
        }
        case ARRAY_POINTER: {
            auto arrayPointerValue = (ArrayPointerValue*)value;
            auto genInstructions = dynamicArrayPositionToRegister(
                arrayPointerValue->array, arrayPointerValue->indexVariable,
                mainReg);
            concatStringsVectors(&readInstructions, &genInstructions);
            break;
        }
        default:
            break;
    }

    readInstructions.push_back(Instructions::GET(mainReg));
    // TODO its only for tests now
    concatStringsVectors(&commands, &readInstructions);
}

void write(Value* value) {
    vector<string> readInstructions;
    Register mainReg = Registers::A;
    switch (value->type) {
        case NUMBER: {
            auto numberValue = (NumberValue*)value;
            auto genInstructions =
                tempConstToRegister(numberValue->number, mainReg);

            concatStringsVectors(&readInstructions, &genInstructions);
            break;
        }
        case POINTER: {
            auto pointerValue = (PointerValue*)value;
            pointerValue->variable->initialized = true;
            auto genInstructions = generateNumberInRegister(
                pointerValue->variable->memoryId, mainReg);

            concatStringsVectors(&readInstructions, &genInstructions);
            break;
        }
        case ARRAY_NUMBER: {
            auto arrayNumberValue = (ArrayNumberValue*)value;
            arrayNumberValue->array->initalizeId(arrayNumberValue->index);
            auto genInstructions = generateNumberInRegister(
                arrayNumberValue->array->getMemoryId(arrayNumberValue->index),
                mainReg);

            concatStringsVectors(&readInstructions, &genInstructions);
            break;
        }
        case ARRAY_POINTER: {
            auto arrayPointerValue = (ArrayPointerValue*)value;
            auto genInstructions = dynamicArrayPositionToRegister(
                arrayPointerValue->array, arrayPointerValue->indexVariable,
                mainReg);
            concatStringsVectors(&readInstructions, &genInstructions);
            break;
        }
        default:
            break;
    }

    readInstructions.push_back(Instructions::PUT(mainReg));
    // TODO its only for tests now
    concatStringsVectors(&commands, &readInstructions);
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