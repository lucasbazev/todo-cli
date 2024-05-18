#include <iostream>
#include <string>
using namespace std;

#define ANSI_GREEN   "\x1b[32m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_RESET   "\x1b[0m"

struct Task {
  int id;
  string name;
  bool done = false;
};

bool validateCommand(string command) {
  string validCommands[] = {"add", "remove", "done", "undone", "list", "clear", "help", "end"};

  for (string validCommand : validCommands) {
    if (command == validCommand) {
      return true;
    }
  }

  return false;
}

int main() {
  string commands[] = {
    "'add <task_name>'   -> adds task to list",
    "'remove <task_id>'  -> removes task from list",
    "'done <task_id>'    -> marks task as done",
    "'undone <task_id>   -> marks task as undone",
    "'list'              -> shows all items from list",
    "'clear'             -> removes all items from list",
    "'help'              -> lists available commands",
    "'end'               -> exits program",   
  };

  Task todos[20];
  int taskId, currIndex = 0;

  cout << ANSI_BLUE << "Welcome to TODO CLI - Developed by Lucas Azevedo" << endl;
  cout << ANSI_RESET << "Enter 'help' for list of available commands.\n\n";

  string command;
  Task task;

  while (command != "end") {
    cout << ANSI_RESET << endl << "Enter a command: ";
    getline(cin, command);

    if (!validateCommand(command)) {
      cout << ANSI_RED << "Invalid command. Enter 'help' to see a list of available commands." << endl;
      continue;
    }

    if (currIndex == 0) {
      if (command == "remove" || command == "done" || command == "undone" || command == "clear") {
        cout << ANSI_YELLOW << "Cannot remove/clear or mark as done/undone since your list is empty.\nTry adding a new task." << endl;
        continue;
      }
    }

    if (command == "add") {
      cout << "Enter task: ";
      getline(cin, task.name);

      task.id = currIndex + 1;
      todos[currIndex] = task;
      currIndex++;
      
      printf("%s'%s' added to your list.\n", ANSI_BLUE, task.name.c_str());
    }

    if (command == "done") {
      cout << "Enter task id: ";
      cin >> taskId;
      cin.ignore();

      // could simply do todos[taskId - 1].done = true, but I want to implement a real search algorithm
      for (int i = 0; i < currIndex; i++) {
        if (todos[i].id == taskId) {
          todos[i].done = true;

          cout << ANSI_GREEN;
          printf("Task '%i. %s' marked as done.\n", todos[i].id, todos[i].name.c_str());
          cout << "Well done!" << endl;
        }
      }
    }

    if (command == "undone") {
      cout << "Enter task id: ";
      cin >> taskId;
      cin.ignore();
      
      for (int i = 0; i < currIndex; i++) {
        if (todos[i].id == taskId) {
          todos[i].done = false;

          cout << ANSI_YELLOW;
          printf("Task '%i, %s' marked as undone.\n", todos[i].id, todos[i].name.c_str());
        }
      }
    }

    if (command == "remove") {
      cout << "Enter task id: ";
      cin >> taskId;
      cin.ignore();

      for (int i = 0; i < currIndex; i++) {
        if (todos[i].id == taskId) {
          char confirm;
          printf("%sYou're removing '%i. %s' from the list. Confirm (y/n)? ", ANSI_YELLOW, todos[i].id, todos[i].name.c_str());
          cin >> confirm;
          cin.ignore();

          if (confirm == 'n') break;

          cout << ANSI_RED << endl;
          printf("Task '%i. %s' removed from list.\n", todos[i].id, todos[i].name.c_str());
          
          for (int j = i; j < currIndex; j++) {
            todos[j] = todos[j + 1];
            todos[j].id--;
          }

          currIndex--;
          break;
        }
      }
    }

    if (command == "list") {
      if (currIndex == 0) {
        cout << endl << "Your TODO list is empty..." << endl;
      } else {
        cout << endl << "TODO: " << endl;

        for (int i = 0; i < currIndex; i++) {
          if (todos[i].done) {
            cout << ANSI_GREEN;
            printf("%i. [DONE] %s\n", todos[i].id, todos[i].name.c_str());
            continue;
          }

          cout << ANSI_RESET;
          printf("%i. %s\n", i + 1, todos[i].name.c_str()); 
        }
      }
    }

    if (command == "clear") {
      char confirm;
      cout << ANSI_YELLOW << "You're removing all items from your list. Confirm (y/n)? ";
      cin >> confirm;
      cin.ignore();

      if (confirm == 'n') continue;

      Task empty;
      empty.name = "";
      empty.id = 0;
      todos[0] = empty;
      currIndex = 0;

      cout << ANSI_RED << "Your list has been cleared." << endl;
    }

    if (command == "help") {
      cout << endl << "Available commands:" << endl;
      
      for (int i = 0; i < (sizeof(commands) / sizeof(commands[0])); i++) {
        cout << commands[i] << endl;
      }

      cout << endl;
    }

    if (command == "end") {
      char confirm;
      cout << ANSI_YELLOW << "You're going to lose your list. Confirm (y/n)? ";
      cin >> confirm;
      cin.ignore();

      if (confirm == 'y') continue;
      else command = "";
    }
  }

  cout << endl << ANSI_RED << "Program exited." << endl;

	return 0;
}
