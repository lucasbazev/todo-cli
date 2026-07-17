#include "storage/storage.hpp"
#include "domain/task_list.hpp"
#include <sqlite3.h>
#include <stdexcept>
#include <vector>

Storage::Storage(const std::string& db_path) : db_path_(db_path) {
    init_db();
}

Storage::~Storage() {}

void Storage::init_db() {
    sqlite3* db;
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Could not open database for initialization.");
    }

    const char* sql = "CREATE TABLE IF NOT EXISTS todos ("
                      "id INTEGER PRIMARY KEY,"
                      "task_name TEXT NOT NULL,"
                      "status INTEGER NOT NULL);";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        throw std::runtime_error("Failed to create table: " + error);
    }

    sqlite3_close(db);
}

void Storage::load_into(TaskList& list) {
    sqlite3* db;
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Could not open database for loading.");
    }

    const char* sql = "SELECT id, task_name, status FROM todos;";
    sqlite3_stmt* stmt;

    std::vector<Task> loaded_tasks;
    int max_id = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char* name_ptr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string name = name_ptr ? name_ptr : "";
            int status_int = sqlite3_column_int(stmt, 2);

            Task task(id, name);
            task.set_status(static_cast<Status>(status_int));
            loaded_tasks.push_back(std::move(task));
            
            if (id > max_id) max_id = id;
        }
        sqlite3_finalize(stmt);
    } else {
        sqlite3_close(db);
        throw std::runtime_error("Failed to prepare load statement.");
    }

    sqlite3_close(db);
    list.set_tasks(std::move(loaded_tasks));
    list.set_next_id(max_id + 1);
}

void Storage::save(const TaskList& list) {
    sqlite3* db;
    if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error("Could not open database for saving.");
    }

    char* errMsg = nullptr;
    if (sqlite3_exec(db, "DELETE FROM todos;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        throw std::runtime_error("Failed to clear table: " + error);
    }

    const char* sql = "INSERT INTO todos (id, task_name, status) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        for (const auto& task : list.tasks()) {
            sqlite3_reset(stmt);
            sqlite3_bind_int(stmt, 1, task.id());
            sqlite3_bind_text(stmt, 2, task.description().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 3, static_cast<int>(task.status()));
            
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                // Log error but continue
            }
        }
        sqlite3_finalize(stmt);
    }
    sqlite3_close(db);
}
