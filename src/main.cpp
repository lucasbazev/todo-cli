#include "domain/task_list.hpp"
#include "storage/storage.hpp"
#include "cli/parser.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// ANSI Color Definitions
#define ANSI_GREEN "\x1b[32m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_RESET "\x1b[0m"

void print_help() {
    std::cout << ANSI_BLUE << "\nAvailable commands:" << ANSI_RESET << std::endl;
    std::cout << "  add <description>  - Add a new task" << std::endl;
    std::cout << "  list               - List all tasks" << std::endl;
    std::cout << "  done <id>          - Mark task as done" << std::endl;
    std::cout << "  undone <id>        - Mark task as pending" << std::endl;
    std::cout << "  start <id>         - Mark task as doing" << std::endl;
    std::cout << "  remove <id>        - Remove a task" << std::endl;
    std::cout << "  clear              - Clear all tasks" << std::endl;
    std::cout << "  help               - Show this menu" << std::endl;
    std::cout << "  exit               - Exit application" << std::endl;
}

void print_tasks(const TaskList& list) {
    if (list.tasks().empty()) {
        std::cout << ANSI_YELLOW << "Your TODO list is empty..." << ANSI_RESET << std::endl;
        return;
    }

    std::cout << ANSI_BLUE << "\nTODO: " << ANSI_RESET << std::endl;
    for (const auto& t : list.tasks()) {
        if (t.status() == Status::DONE) {
            std::cout << ANSI_GREEN << t.id() << ". [DONE] " << t.description() << ANSI_RESET << std::endl;
        } else if (t.status() == Status::DOING) {
            std::cout << ANSI_YELLOW << t.id() << ". [DOING] " << t.description() << ANSI_RESET << std::endl;
        } else {
            std::cout << t.id() << ". " << t.description() << ANSI_RESET << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        Storage storage("todo_database.db");
        TaskList list;
        storage.load_into(list);

        // Branding
        std::cout << ANSI_BLUE << "\n\nWelcome to TODO CLI - Developed by Lucas Azevedo" << ANSI_RESET << std::endl;
        std::cout << "Enter 'help' for list of available commands.\n" << std::endl;

        // If arguments are provided (non-interactive mode)
        if (argc > 1) {
            std::vector<std::string> tokens;
            for (int i = 1; i < argc; ++i) {
                tokens.push_back(argv[i]);
            }
            
            Command cmd = Parser::parse(tokens);
            
            if (cmd.type == CommandType::ADD) {
                std::string desc;
                for (size_t i = 0; i < cmd.args.size(); ++i) {
                    desc += cmd.args[i] + (i == cmd.args.size() - 1 ? "" : " ");
                }
                if (!desc.empty()) {
                    list.add(desc);
                    std::cout << ANSI_BLUE << "'" << desc << "' added to your list." << ANSI_RESET << std::endl;
                }
            } else if (cmd.type == CommandType::LIST) {
                print_tasks(list);
            } else if (cmd.type == CommandType::DONE) {
                if (!cmd.args.empty()) {
                    list.mark_done(std::stoi(cmd.args[0]));
                    std::cout << ANSI_GREEN << "Task marked as done. Well done!" << ANSI_RESET << std::endl;
                }
            } else if (cmd.type == CommandType::REMOVE) {
                if (!cmd.args.empty()) {
                    list.remove(std::stoi(cmd.args[0]));
                    std::cout << ANSI_RED << "Task removed from list." << ANSI_RESET << std::endl;
                }
            } else if (cmd.type == CommandType::CLEAR) {
                list.clear();
                std::cout << ANSI_RED << "Your list has been cleared." << ANSI_RESET << std::endl;
            } else if (cmd.type == CommandType::START) {
                if (!cmd.args.empty()) {
                    list.mark_doing(std::stoi(cmd.args[0]));
                    std::cout << ANSI_YELLOW << "Task marked as doing." << ANSI_RESET << std::endl;
                }
            } else if (cmd.type == CommandType::UNDONE) {
                if (!cmd.args.empty()) {
                    list.mark_pending(std::stoi(cmd.args[0]));
                    std::cout << ANSI_YELLOW << "Task marked as undone." << ANSI_RESET << std::endl;
                }
            } else if (cmd.type == CommandType::HELP) {
                print_help();
            } else {
                std::cout << ANSI_RED << "Invalid command. Enter 'help' to see a list of available commands." << ANSI_RESET << std::endl;
            }
            
            storage.save(list);
            return 0;
        }

        // Interactive mode
        std::string line;
        while (true) {
            std::cout << ANSI_RESET << "\nEnter a command: ";
            if (!std::getline(std::cin, line)) break;
            if (line == "exit" || line == "end") {
                // Exit logic
                std::cout << ANSI_YELLOW << "You're ending this program. Confirm (y/n)? ";
                std::string confirm;
                std::getline(std::cin, confirm);
                if (confirm == "y" || confirm == "Y") {
                    if (!list.tasks().empty()) {
                        std::cout << ANSI_YELLOW << "Do you want to save this list into the database? (y/n): ";
                        std::getline(std::cin, confirm);
                        if (confirm == "y" || confirm == "Y") {
                            storage.save(list);
                            std::cout << ANSI_GREEN << "List successfully saved to database." << ANSI_RESET << std::endl;
                        }
                    }
                    std::cout << ANSI_RED << "Program exited." << ANSI_RESET << std::endl;
                    break;
                } else {
                    // If they say no to exit, we just continue the loop
                    // But we need to re-prompt for the command
                    std::cout << "Exiting cancelled. Type 'exit' again to quit." << std::endl;
                    continue;
                }
            }

            std::vector<std::string> tokens;
            std::stringstream ss(line);
            std::string token;
            while (ss >> token) {
                tokens.push_back(token);
            }

            Command cmd = Parser::parse(tokens);

            if (cmd.type == CommandType::UNKNOWN) {
                std::cout << ANSI_RED << "Invalid command. Enter 'help' to see a list of available commands." << ANSI_RESET << std::endl;
                continue;
            }

            if (cmd.type == CommandType::ADD) {
                std::string desc;
                for (size_t i = 0; i < cmd.args.size(); ++i) {
                    desc += cmd.args[i] + (i == cmd.args.size() - 1 ? "" : " ");
                }
                if (!desc.empty()) {
                    list.add(desc);
                    std::cout << ANSI_BLUE << "'" << desc << "' added to your list." << ANSI_RESET << std::endl;
                }
            } else if (cmd.type == CommandType::DONE) {
                if (!cmd.args.empty()) {
                    list.mark_done(std::stoi(cmd.args[0]));
                    std::cout << ANSI_GREEN << "Task marked as done. Well done!" << ANSI_RESET << std::endl;
                }
            } else if (cmd.type == CommandType::REMOVE) {
                if (!cmd.args.empty()) {
                    // Confirmation for removal
                    std::cout << ANSI_YELLOW << "Are you sure you want to remove this task? (y/n): ";
                    std::string confirm;
                    std::getline(std::cin, confirm);
                    if (confirm == "y" || confirm == "Y") {
                        list.remove(std::stoi(cmd.args[0]));
                        std::cout << ANSI_RED << "Task removed from list." << ANSI_RESET << std::endl;
                    }
                }
            } else if (cmd.type == CommandType::CLEAR) {
                std::cout << ANSI_YELLOW << "You're removing all items from your list. Confirm (y/n)? ";
                std::string confirm;
                std::getline(std::cin, confirm);
                if (confirm == "y" || confirm == "Y") {
                    list.clear();
                    std::cout << ANSI_RED << "Your list has been cleared." << ANSI_RESET << std::endl;
                }
            } else if (cmd.type == CommandType::START) {
                if (!cmd.args.empty()) {
                    list.mark_doing(std::stoi(cmd.args[0]));
                    std::cout << ANSI_YELLOW << "Task marked as doing." << ANSI_RESET << std::endl;
                }
            } else if (cmd.type == CommandType::UNDONE) {
                if (!cmd.args.empty()) {
                    list.mark_pending(std::stoi(cmd.args[0]));
                    std::cout << ANSI_YELLOW << "Task marked as undone." << ANSI_RESET << std::endl;
                }
            } else if (cmd.type == CommandType::LIST) {
                print_tasks(list);
            } else if (cmd.type == CommandType::HELP) {
                print_help();
            }

            storage.save(list);
        }
        
        storage.save(list);

    } catch (const std::exception& e) {
        std::cerr << ANSI_RED << "Error: " << e.what() << ANSI_RESET << std::endl;
        return 1;
    }
    return 0;
}
