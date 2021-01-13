#include "compiler.hpp"

string outputFilename;

void setOutputFilename(string filename) { outputFilename = filename; }

vector<string>* read(Value* value) {
    checkIterator(value);
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

    checkIterator(identifierValue);
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
    instructions->push_back(Instructions::JZERO(Registers::A, 2));
    instructions->push_back(Instructions::JUMP(
        -(commands->size() + 1 + conditionInstructions.size())));

    return instructions;
}

void initFor(string identifier, Value* from, Value* to) {
    cout << "init for with " << identifier << endl;
    getSymbolTable()->addIteratorVariable(identifier);
    getSymbolTable()->addIteratorVariable("__" + identifier + "_to_value");
}

vector<string>* forTo(string identifier, Value* from, Value* to,
                      vector<string>* commands) {
    auto instructions = new vector<string>;
    cout << "FOR" << endl;

    // auto idVar = getSymbolTable()->getVariable(identifier);
    auto idValue = getValue(identifier);
    cout << "FOR 1" << endl;

    auto genIdAddr = valueAdressToRegister(idValue, Registers::A);
    cout << "FOR 2" << endl;

    auto genIdVal = valueToRegister(idValue, Registers::B);
    cout << "FOR 3" << endl;

    auto genFrom = valueToRegister(from, Registers::B);

    auto toValue = getValue("__" + identifier + "_to_value");
    auto genToValueAdrr = valueAdressToRegister(toValue, Registers::A);
    auto genTo = valueToRegister(to, Registers::B);
    cout << "FOR 4" << endl;

    auto condition = getLeqCondition(idValue, toValue);
    cout << "FOR 5" << endl;

    auto genCondition = evalConditionToRegister(condition, Registers::A);
    cout << "FOR 6" << endl;

    concatStringsVectors(instructions, &genIdAddr);
    concatStringsVectors(instructions, &genFrom);
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));

    concatStringsVectors(instructions, &genToValueAdrr);
    concatStringsVectors(instructions, &genTo);
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));

    concatStringsVectors(instructions, &genCondition);
    instructions->push_back(Instructions::JZERO(
        Registers::A,
        commands->size() + genIdAddr.size() + genIdVal.size() + 4));
    concatStringsVectors(instructions, commands);
    concatStringsVectors(instructions, &genIdAddr);
    concatStringsVectors(instructions, &genIdVal);
    instructions->push_back(Instructions::INC(Registers::B));
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));
    instructions->push_back(
        Instructions::JUMP(-(2 + genIdVal.size() + genIdAddr.size() +
                             commands->size() + 1 + genCondition.size())));

    cout << "FOR 7" << endl;
    idValue->variable->identifier = "";
    toValue->variable->identifier = "";
    return instructions;
}

vector<string>* forDownTo(string identifier, Value* from, Value* to,
                          vector<string>* commands) {
    auto instructions = new vector<string>;
    cout << "FOR" << endl;

    // auto idVar = getSymbolTable()->getVariable(identifier);
    auto idValue = getValue(identifier);
    cout << "FOR 1" << endl;

    auto genIdAddr = valueAdressToRegister(idValue, Registers::A);
    cout << "FOR 2" << endl;

    auto genIdVal = valueToRegister(idValue, Registers::B);
    cout << "FOR 3" << endl;

    auto genFrom = valueToRegister(from, Registers::B);

    auto toValue = getValue("__" + identifier + "_to_value");
    auto genToValueAdrr = valueAdressToRegister(toValue, Registers::A);
    auto genTo = valueToRegister(to, Registers::B);
    cout << "FOR 4" << endl;

    auto condition = getLeqCondition(toValue, toValue);
    cout << "FOR 5" << endl;

    auto genCondition = evalConditionToRegister(condition, Registers::A);
    cout << "FOR 6" << endl;

    concatStringsVectors(instructions, &genIdAddr);
    concatStringsVectors(instructions, &genFrom);
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));

    concatStringsVectors(instructions, &genToValueAdrr);
    concatStringsVectors(instructions, &genTo);
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));

    concatStringsVectors(instructions, &genCondition);
    instructions->push_back(Instructions::JZERO(
        Registers::A,
        commands->size() + genIdAddr.size() + genIdVal.size() + 4));
    concatStringsVectors(instructions, commands);
    concatStringsVectors(instructions, &genIdAddr);
    concatStringsVectors(instructions, &genIdVal);
    instructions->push_back(Instructions::DEC(Registers::B));
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));
    instructions->push_back(
        Instructions::JUMP(-(2 + genIdVal.size() + genIdAddr.size() +
                             commands->size() + 1 + genCondition.size())));

    cout << "FOR 7" << endl;
    idValue->variable->identifier = "";
    toValue->variable->identifier = "";
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