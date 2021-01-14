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
    if (DEBUG_MODE) {
        cout << "------------ ASSIGN ------------" << endl;
        string operand = "";
        switch (expression->type) {
            case EX_ADD:
                operand = "+";
                break;
            case EX_SUB:
                operand = "-";
                break;
            case EX_MUL:
                operand = "*";
                break;
            case EX_DIV:
                operand = "/";
                break;
            case EX_MOD:
                operand = "%";
                break;
            default:
                break;
        }
        cout << "  " << identifierValue->toString() << " := " << expression->left->toString() << " " << operand << " "
             << (expression->right ? expression->right->toString() : "") << endl;
        cout << "--------------------------------" << endl << endl << endl;
    }

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
    instructions->push_back(Instructions::JZERO(Registers::A, -(commands->size() + conditionInstructions.size())));

    return instructions;
}

void initFor(string identifier) {
    getSymbolTable()->addIteratorVariable(identifier);
    getSymbolTable()->addIteratorVariable("__" + identifier + "_to_copy");
}

vector<string>* forTo(string identifier, Value* from, Value* to, vector<string>* commands) {
    auto instructions = new vector<string>;

    auto iterator = getValue(identifier);
    auto iteratorAddressToA = valueAdressToRegister(iterator, Registers::A);
    auto iteratorValueToB = valueToRegister(iterator, Registers::B);

    auto copy = getValue("__" + identifier + "_to_copy");
    auto copyAddressToA = valueAdressToRegister(copy, Registers::A);
    auto copyValueToA = valueToRegister(copy, ::Registers::A);

    auto toValueToB = valueToRegister(to, Registers::B);
    auto fromValueToB = valueToRegister(from, Registers::B);

    concatStringsVectors(instructions, &iteratorAddressToA);                   // reg a = i addr
    concatStringsVectors(instructions, &fromValueToB);                         // reg b = from
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // i = from
    concatStringsVectors(instructions,                                         //
                         &copyAddressToA);                                     // reg a = copy addr
    concatStringsVectors(instructions, &toValueToB);                           // reg b = to
    instructions->push_back(Instructions::INC(Registers::B));                  // reg b = to + 1
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // copy = to + 1

    // loop start ----------------------------
    concatStringsVectors(instructions, &copyValueToA);              // reg a = copy = to + 1
    concatStringsVectors(instructions, &iteratorValueToB);          // reg b = i
    instructions->push_back(Instructions::INC(Registers::B));       // reg b = i + 1
    instructions->push_back(Instructions::SUB(Registers::B,         //
                                              Registers::A));       // reg b = (i + 1) - (to + 1)
    instructions->push_back(Instructions::JZERO(Registers::B, 2));  // if (i + 1) - (to + 1) == 0 stay in loop
    instructions->push_back(Instructions::JUMP(commands->size() + iteratorAddressToA.size() + 4 + 1));  // else out

    concatStringsVectors(instructions, commands);  // loop body

    concatStringsVectors(instructions, &iteratorAddressToA);                   // reg a = i addr
    instructions->push_back(Instructions::LOAD(Registers::B, Registers::A));   // reg b = i
    instructions->push_back(Instructions::INC(Registers::B));                  // reg b = i + 1
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // i = i + 1
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

    auto copy = getValue("__" + identifier + "_to_copy");
    auto copyAddressToA = valueAdressToRegister(copy, Registers::A);
    auto copyValueToA = valueToRegister(copy, ::Registers::A);

    auto downtoValueToB = valueToRegister(downto, Registers::B);
    auto fromValueToB = valueToRegister(from, Registers::B);

    concatStringsVectors(instructions, &iteratorAddressToA);                   // reg a = i addr
    concatStringsVectors(instructions, &fromValueToB);                         // reg b = from
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // i = from
    concatStringsVectors(instructions,                                         //
                         &copyAddressToA);                                     // reg a = copy addr
    concatStringsVectors(instructions, &downtoValueToB);                       // reg b = to
    instructions->push_back(Instructions::INC(Registers::B));                  // reg b = to + 1
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // copy = to + 1

    // loop start ----------------------------
    concatStringsVectors(instructions, &copyValueToA);              // reg a = copy
    concatStringsVectors(instructions, &iteratorValueToB);          // reg b = i
    instructions->push_back(Instructions::INC(Registers::B));       // reg b = i + 1
    instructions->push_back(Instructions::SUB(Registers::A,         //
                                              Registers::B));       // reg a = (to + 1) - (i + 1)
    instructions->push_back(Instructions::JZERO(Registers::A, 2));  // if (to + 1) - (i + 1) = 0 stay in loop
    instructions->push_back(Instructions::JUMP(commands->size() + iteratorAddressToA.size() + 5 + 1));  // else out

    concatStringsVectors(instructions, commands);  // loop body

    concatStringsVectors(instructions, &iteratorAddressToA);                   // reg a = i addr
    instructions->push_back(Instructions::LOAD(Registers::B, Registers::A));   // reg b = i
    instructions->push_back(Instructions::JZERO(Registers::B, 4));             // check if i is 0, since we can't go -1
    instructions->push_back(Instructions::DEC(Registers::B));                  // reg b = i - 1
    instructions->push_back(Instructions::STORE(Registers::B, Registers::A));  // id = reg b = i + 1
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
    for (vector<string>::size_type i = 0; i < commands->size(); i++) {
        out << commands->at(i) << endl;
    }
    out << "HALT" << endl;
    out.close();
}