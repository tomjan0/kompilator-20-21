#include "compiler.hpp"

string outputFilename;

void setOutputFilename(string filename) { outputFilename = filename; }

vector<string>* read(Value* value) {
    auto readInstructions = new vector<string>;
    auto genInstructions = valueAdressToRegister(value, Registers::A);
    concatStringsVectors(readInstructions, &genInstructions);
    readInstructions->push_back(Instructions::GET(Registers::A));

    value->initialize();
    return readInstructions;
}

vector<string>* write(Value* value) {
    if (value->isInitialized()) {
        auto writeInstructions = new vector<string>;
        auto genInstructions = valueAdressToRegister(value, Registers::A);
        concatStringsVectors(writeInstructions, &genInstructions);
        writeInstructions->push_back(Instructions::PUT(Registers::A));

        return writeInstructions;
    } else {
        error("Użycie niezainicializowanej zmiennej " + value->toString());
    }
    return nullptr;
}

vector<string>* assign(Value* identifierValue, Expression* expression) {
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

    vector<string>* assignInstructions = new vector<string>;
    auto expressionInstructions =
        evalExpressionToRegister(expression, Registers::B);
    auto identifierInstructions =
        valueAdressToRegister(identifierValue, Registers::A);

    // load expression first to avoid overwriting registers
    concatStringsVectors(assignInstructions, &expressionInstructions);
    concatStringsVectors(assignInstructions, &identifierInstructions);

    assignInstructions->push_back(
        Instructions::STORE(Registers::B, Registers::A));
    identifierValue->initialize();

    return assignInstructions;
}

vector<string>* ifThen(Condition* condition, vector<string>* commands) {
    auto instructions = new vector<string>;

    auto conditionInstructions =
        evalConditionToRegister(condition, Registers::A);

    concatStringsVectors(instructions, &conditionInstructions);
    instructions->push_back(
        Instructions::JZERO(Registers::A, commands->size() + 1));
    concatStringsVectors(instructions, commands);

    return instructions;
}

vector<string>* ifThenElse(Condition* condition, vector<string>* thenCommands,
                           vector<string>* elseCommands) {
    auto instructions = new vector<string>;

    auto conditionInstructions =
        evalConditionToRegister(condition, Registers::A);

    concatStringsVectors(instructions, &conditionInstructions);
    instructions->push_back(
        Instructions::JZERO(Registers::A, thenCommands->size() + 2));
    concatStringsVectors(instructions, thenCommands);
    instructions->push_back(Instructions::JUMP(elseCommands->size() + 1));
    concatStringsVectors(instructions, elseCommands);

    return instructions;
}

vector<string>* whileDo(Condition* condition, vector<string>* commands) {
    auto instructions = new vector<string>;

    auto conditionInstructions =
        evalConditionToRegister(condition, Registers::A);

    concatStringsVectors(instructions, &conditionInstructions);
    instructions->push_back(
        Instructions::JZERO(Registers::A, commands->size() + 2));
    concatStringsVectors(instructions, commands);
    instructions->push_back(Instructions::JUMP(
        -(commands->size() + 1 + conditionInstructions.size())));

    return instructions;
}

vector<string>* repeatUntil(vector<string>* commands, Condition* condition) {
    auto instructions = new vector<string>;

    auto conditionInstructions =
        evalConditionToRegister(condition, Registers::A);

    concatStringsVectors(instructions, commands);
    concatStringsVectors(instructions, &conditionInstructions);
    instructions->push_back(
        Instructions::JZERO(Registers::A, commands->size() + 2));
    instructions->push_back(Instructions::JUMP(
        -(commands->size() + 1 + conditionInstructions.size())));

    return instructions;
}

vector<string>* mergeInstructions(vector<string>* commands,
                                  vector<string>* command) {
    concatStringsVectors(commands, command);
    return commands;
}

void writeCommands(vector<string>* commands) {
    ofstream out;
    out.open(outputFilename);
    for (int i = 0; i < commands->size(); i++) {
        out << commands->at(i) << endl;
    }
    out << "HALT" << endl;
    out.close();
}