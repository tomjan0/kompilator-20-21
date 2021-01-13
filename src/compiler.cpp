#include "compiler.hpp"

vector<string> commands;
string output_filename;
ofstream file;
bool debug = false;

void test() {}

void set_output_filename(char* filename) { output_filename = filename; }

void open_file() { file.open(output_filename); }

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

    auto conditionInstructions = evalConditionToRegister(condition, Registers::A);

    concatStringsVectors(instructions, &conditionInstructions);
    instructions->push_back(Instructions::JZERO(Registers::A, commands->size() + 1));
}

vector<string>* mergeInstructions(vector<string>* commands,
                                  vector<string>* command) {
    concatStringsVectors(commands, command);
    return commands;
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

void writeCommands(vector<string>* cmds) {
    for (int i = 0; i < cmds->size(); i++) {
        file << cmds->at(i) << endl;
    }
    file << "HALT" << endl;
    file.close();
}