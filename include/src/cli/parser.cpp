#include "cli/parser.hpp"

Command Parser::parse(int argc, char* argv[]) {
    if (argc < 2) {
        return {CommandType::HELP, {}};
    }

    std::string cmd = argv[1];
    std::vector<std::string> args;
    for (int i = 2; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    if (cmd == "add") return {CommandType::ADD, args};
    if (cmd == "list") return {CommandType::LIST, args};
    if (cmd == "done") return {CommandType::DONE, args};
    if (cmd == "undone") return {CommandType::UNDONE, args};
    if (cmd == "start") return {CommandType::START, args};
    if (cmd == "remove") return {CommandType::REMOVE, args};
    if (cmd == "clear") return {CommandType::CLEAR, args};
    if (cmd == "help") return {CommandType::HELP, args};
    
    return {CommandType::UNKNOWN, args};
}
