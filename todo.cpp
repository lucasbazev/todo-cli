#include <iostream>
#include <sqlite3.h>
#include <string>

using namespace std;

#define ANSI_GREEN "\x1b[32m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_RESET "\x1b[0m"

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

int loadPreviousList(void *data, int columns, char **rowValues, char **azColName) {
  Task task;
  Task *previousList = static_cast<Task *>(data);

  for (int i = 0; i < columns; i++) {
    if (string(azColName[i]) == "done") {
      task.done = (bool)(*rowValues[i] - '0');
    } else if (string(azColName[i]) == "task_name") {
      task.name = rowValues[i];
    } else if (string(azColName[i]) == "rowid") {
      task.id = *rowValues[i] - '0';
    }
  }

  previousList[task.id - 1] = task;
  return 0;
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

  cout << ANSI_BLUE << "\n\nWelcome to TODO CLI - Developed by Lucas Azevedo" << endl;
  cout << ANSI_RESET << "Enter 'help' for list of available commands.\n\n";

  // init/connect to database and init vars
  sqlite3 *db;
  char *sqliteErrMsg;
  string sql;
  int sqliteResponseCode;

  if (sqlite3_open("todo_database", &db)) {
    cerr << sqlite3_errmsg(db);
    sqlite3_close(db);
  }

  // check if there are any previously saved list in the database
  Task previousList[20], todos[20];
  int currIndex = 0;

  sqliteResponseCode = sqlite3_exec(db, "select rowid, * from todos;", loadPreviousList, &previousList, &sqliteErrMsg);

  if (sqliteResponseCode) {
    cout << ANSI_YELLOW << "You do not have a previously saved list.";
  } else if (previousList[0].id) {
    char confirm;
    cout << ANSI_YELLOW << "You have a previously saved list in the database:\n\n";

    for (Task item : previousList) {
      if (!item.id)
        break;

      if (item.done) {
        cout << ANSI_GREEN;
        printf("%i. [DONE] %s\n", item.id, item.name.c_str());
        continue;
      }

      cout << ANSI_RESET;
      printf("%i. %s\n", item.id, item.name.c_str());
    }

    cout << ANSI_YELLOW << "\nDo you want to retrieve it (y/n)? ";
    cin >> confirm;
    cin.ignore();

    if (confirm == 'y') {
      for (int i = 0; i < 20; i++) {
        if (!previousList[i].id)
          break;

        todos[i] = previousList[i];
        currIndex++;
      }

      cout << ANSI_GREEN << "Your list has been succesfully retrieved!" << endl;
    }
  }

  int taskId;
  string command;
  Task task;

  while (command != "end") {
    cout << ANSI_RESET << endl
         << "Enter a command: ";
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

          if (confirm == 'n')
            break;

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
        cout << endl
             << "Your TODO list is empty..." << endl;
      } else {
        cout << endl
             << "TODO: " << endl;

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

      if (confirm == 'n')
        continue;

      Task empty;
      empty.name = "";
      empty.id = 0;
      todos[0] = empty;
      currIndex = 0;

      cout << ANSI_RED << "Your list has been cleared." << endl;
    }

    if (command == "help") {
      cout << endl
           << "Available commands:" << endl;

      for (int i = 0; i < (sizeof(commands) / sizeof(commands[0])); i++) {
        cout << commands[i] << endl;
      }

      cout << endl;
    }

    if (command == "end") {
      char confirm;
      cout << ANSI_YELLOW << "You're ending this program. Confirm (y/n)? ";
      cin >> confirm;
      cin.ignore();

      if (confirm == 'y') {
        if (currIndex != 0) {
          cout << endl
               << "Do you want to save this list into the database?\nThis action will override the previously saved list. (y/n): ";
          cin >> confirm;
          cin.ignore();

          if (confirm == 'y') {
            sqliteResponseCode = sqlite3_exec(db, "drop table if exists todos;", 0, 0, &sqliteErrMsg);

            if (sqliteResponseCode != SQLITE_OK) {
              cerr << ANSI_RED << "Failed to save to database.";
              sqlite3_close(db);
              continue;
            }

            sqliteResponseCode = sqlite3_exec(db, "create table todos(task_name text not null unique, done int);", 0, 0, &sqliteErrMsg);

            if (sqliteResponseCode != SQLITE_OK) {
              cerr << ANSI_RED << "Failed to save to database.";
              sqlite3_close(db);
              continue;
            }

            for (int i = 0; i < currIndex; i++) {
              sql = "insert into todos values ('" + todos[i].name + "'," + to_string(todos[i].done ? 1 : 0) + ");";
              sqliteResponseCode = sqlite3_exec(db, sql.c_str(), 0, 0, &sqliteErrMsg);

              if (sqliteResponseCode != SQLITE_OK) {
                cerr << ANSI_RED << "Failed to save to database.";
                break;
              }
            }

            cout << ANSI_GREEN << "List successfully saved to database." << endl;
          } else
            continue;
        }

        sqlite3_close(db);
      } else
        command = "";
    }
  }

  cout << endl
       << ANSI_RED << "Program exited." << endl;

  return 0;
}
