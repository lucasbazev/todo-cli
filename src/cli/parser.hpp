#ifndef CLI_PARSER_HPP
#define CLI_PARSER_HPP

#include <string>
#include <vector>

enum class CommandType {
    ADD,
    LIST,
    DONE,
    UNDONE,
    START,
    REMOVE,
    CLEAR,
    HELP,
    UNKNOWN
};

struct Command {
    CommandType type;
    std::vector<std::string> args;
};

class Parser {
public:
    static Command parse(const std::vector<std::string>& tokens);
};

#endif // CLI_PARSER_HPP

