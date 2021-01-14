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
    cout << identifierValue->toString() << " := " << expression->left->toString() << " " << expression->type << " "
         << (expression->right ? expression->right->toString() : "") << endl;

    checkIterator(identifierValue);
    vector<string>* assignInstructions = new vector<string>;
    auto expressionInstructions = evalExpressionToRegister(expression, Registers::B);
    auto identifierInstructions = valueAdressToRegister(identifierValue, Registers::A);

    // load expression first to avoid overwriting registers
    concatStringsVectors(assignInstructions, &expressionInstructions);
    concatStringsVectors(assignInstructions, &identifierInstructions);

    assignInstructions->push_back(Instructions::STORE(Registers::B, Registers::A));
    identifierValue->initialize();

    return assignInstructions;
}

vector<string>* ifThen(Condition* condition, vector<string>* commands) {
    auto instructions = new vector<string>;

    auto conditionInstructions = evalConditionToRegister(condition, Registers::A);

    concatStringsVectors(instructions, &conditionInstructions);
    instructions->push_back(Instructions::JZERO(Registers::A, commands->size() + 1));
    concatStringsVectors(instructions, commands);

    return instructions;
}

vector<string>* ifThenElse(Condition* condition, vector<string>* thenCommands, vector<string>* elseCommands) {
    auto instructions = new vector<string>;

    auto conditionInstructions = evalConditionToRegister(condition, Registers::A);

    concatStringsVectors(instructions, &conditionInstructions);
    instructions->push_back(Instructions::JZERO(Registers::A, thenCommands->size() + 2));
    concatStringsVectors(instructions, thenCommands);
    instructions->push_back(Instructions::JUMP(elseCommands->size() + 1));
    concatStringsVectors(instructions, elseCommands);

    return instructions;
}

vector<string>* whileDo(Condition* condition, vector<string>* commands) {
    auto instructions = new vector<string>;

    auto conditionInstructions = evalConditionToRegister(condition, Registers::A);

    concatStringsVectors(instructions, &conditionInstructions);
    instructions->push_back(Instructions::JZERO(Registers::A, commands->size() + 2));
    concatStringsVectors(instructions, commands);
    instructions->push_back(Instructions::JUMP(-(commands->size() + 1 + conditionInstructions.size())));

    return instructions;
}

vector<string>* repeatUntil(vector<string>* commands, Condition* condition) {
    auto instructions = new vector<string>;

    auto conditionInstructions = evalConditionToRegister(condition, Registers::A);

    concatStringsVectors(instructions, commands);
    concatStringsVectors(instructions, &conditionInstructions);
    instructions->push_back(Instructions::JZERO(Registers::A, 2));
    instructions->push_back(Instructions::JUMP(-(commands->size() + 1 + conditionInstructions.size())));

    return instructions;
}

void initFor(string identifier, Value* from, Value* to) {
    cout << "init for with " << identifier << endl;
    getSymbolTable()->addIteratorVariable(identifier);
    getSymbolTable()->addIteratorVariable("__" + identifier + "_to_value");
}

vector<string>* forTo(string identifier, Value* from, Value* to, vector<string>* commands) {
    auto instructions = new vector<string>;

    auto iterator = getValue(identifier);
    auto iteratorAddressToA = valueAdressToRegister(iterator, Registers::A);
    auto iteratorValueToB = valueToRegister(iterator, Registers::B);

    auto copy = getValue("__" + identifier + "_to_value");
    auto copyAddressToA = valueAdressToRegister(copy, Registers::A);
    auto copyValueToA = valueToRegister(copy, ::Registers::A);

    auto toValueToB = valueToRegister(to, Registers::B);
    auto fromValueToB = valueToRegister(from, Registers::B);

    concatStringsVectors(instructions, &iteratorAddressToA);                   // reg a = id addr
    concatStringsVectors(instructions, &fromValueToB);                         // reg b = from
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // id = from

    concatStringsVectors(instructions,
                         &copyAddressToA);            // reg a = copy addr
    concatStringsVectors(instructions, &toValueToB);  // reg b = to
    instructions->push_back(Instructions::INC(Registers::B));
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // to_copy = to + 1

    // loop start ----------------------------
    concatStringsVectors(instructions, &copyValueToA);  // reg a = to_copy = to + 1

    concatStringsVectors(instructions, &iteratorValueToB);  // reg b = id

    instructions->push_back(Instructions::INC(Registers::B));  // reg b = id + 1 DOLICZĆ!!!!

    instructions->push_back(Instructions::SUB(Registers::B,
                                              Registers::A));  // reg b = reg b - reg a = (id + 1) - (to + 1)

    instructions->push_back(
        Instructions::JZERO(Registers::B, 2));  // if (id + 1) - (to + 1) = 0 then its good, so skip jump DOLICZYĆ

    instructions->push_back(Instructions::JUMP(commands->size() + iteratorAddressToA.size() + 4 + 1));

    concatStringsVectors(instructions, commands);  // loop body

    concatStringsVectors(instructions, &iteratorAddressToA);  // reg a = id addr

    instructions->push_back(Instructions::LOAD(Registers::B, Registers::A));  // reg b = id

    instructions->push_back(Instructions::INC(Registers::B));  // reg b = id + 1

    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // id = reg b = id + 1

    instructions->push_back(Instructions::JUMP(-3 - iteratorAddressToA.size() - commands->size() - 4 -
                                               iteratorValueToB.size() - copyValueToA.size()));  // jump to loop start
    // loop end --------------------------

    iterator->variable->identifier = "";
    copy->variable->identifier = "";
    return instructions;
}

vector<string>* forDownTo(string identifier, Value* from, Value* downto, vector<string>* commands) {
    auto instructions = new vector<string>;

    auto iterator = getValue(identifier);
    auto iteratorAddressToA = valueAdressToRegister(iterator, Registers::A);
    auto iteratorValueToB = valueToRegister(iterator, Registers::B);

    auto copy = getValue("__" + identifier + "_to_value");
    auto copyAddressToA = valueAdressToRegister(copy, Registers::A);
    auto copyValueToA = valueToRegister(copy, ::Registers::A);

    auto downtoValueToB = valueToRegister(downto, Registers::B);
    auto fromValueToB = valueToRegister(from, Registers::B);

    concatStringsVectors(instructions, &iteratorAddressToA);                   // reg a = id addr
    concatStringsVectors(instructions, &fromValueToB);                         // reg b = from
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // id = from

    concatStringsVectors(instructions,
                         &copyAddressToA);                                     // reg a = to_copy addr
    concatStringsVectors(instructions, &downtoValueToB);                       // reg b = to
    instructions->push_back(Instructions::INC(Registers::B));                  // reg b++
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // to_copy = to + 1

    // loop start ----------------------------
    concatStringsVectors(instructions, &copyValueToA);  // reg a = to_copy

    concatStringsVectors(instructions, &iteratorValueToB);  // reg b = id

    instructions->push_back(Instructions::INC(Registers::B));  // reg b = id + 1 DOLICZĆ!!!!

    instructions->push_back(Instructions::SUB(Registers::A,
                                              Registers::B));  // reg a = reg a - reg b = to_copy - (id + 1)

    instructions->push_back(
        Instructions::JZERO(Registers::A, 2));  // if to_copy - (id + 1) = 0 then its good, so skip jump

    instructions->push_back(Instructions::JUMP(commands->size() + iteratorAddressToA.size() + 4 + 2));

    concatStringsVectors(instructions, commands);  // loop body

    concatStringsVectors(instructions, &iteratorAddressToA);  // reg a = id addr

    instructions->push_back(Instructions::LOAD(Registers::B, Registers::A));  // reg b = id

    instructions->push_back(Instructions::JZERO(Registers::B, 4));  // check if id is 0, since we can't go -1

    instructions->push_back(Instructions::DEC(Registers::B));  // reg b = id - 1

    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // id = reg b = id + 1

    instructions->push_back(Instructions::JUMP(-4 - iteratorAddressToA.size() - commands->size() - 4 -
                                               iteratorValueToB.size() - copyValueToA.size()));  // jump to loop start
    // loop end --------------------------

    iterator->variable->identifier = "";
    copy->variable->identifier = "";
    return instructions;
}

vector<string>* mergeInstructions(vector<string>* commands, vector<string>* command) {
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