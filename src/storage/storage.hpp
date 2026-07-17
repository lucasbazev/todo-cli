#ifndef STORAGE_STORAGE_HPP
#define STORAGE_STORAGE_HPP

#include "domain/task_list.hpp"
#include <string>

class Storage {
public:
    explicit Storage(const std::string& db_path);
    ~Storage();

    void load_into(TaskList& list);
    void save(const TaskList& list);

private:
    std::string db_path_;
    void init_db();
};

#endif // STORAGE_STORAGE_HPP
