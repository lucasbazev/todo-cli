#include "cli/parser.hpp"

Command Parser::parse(const std::vector<std::string>& tokens) {
    if (tokens.empty()) {
        return {CommandType::HELP, {}};
    }

    std::string cmd = tokens[0];
    std::vector<std::string> args;
    for (size_t i = 1; i < tokens.size(); ++i) {
        args.push_back(tokens[i]);
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
