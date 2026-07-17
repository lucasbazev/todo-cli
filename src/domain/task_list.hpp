#ifndef DOMAIN_TASK_LIST_HPP
#define DOMAIN_TASK_LIST_HPP

#include "domain/task.hpp"
#include <vector>
#include <string>

class TaskList {
public:
    void add(const std::string& description);
    void remove(int id);
    void mark_done(int id);
    void mark_pending(int id);
    void mark_doing(int id);
    void clear();
    
    const std::vector<Task>& tasks() const;

    // For storage/loading
    void set_tasks(std::vector<Task> tasks);
    void set_next_id(int id);

private:
    std::vector<Task> tasks_;
    int next_id_ = 1;
};

#endif // DOMAIN_TASK_LIST_HPP
