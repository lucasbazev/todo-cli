#include "domain/task_list.hpp"
#include <algorithm>

void TaskList::add(const std::string& description) {
    tasks_.emplace_back(next_id_++, description);
}

void TaskList::remove(int id) {
    tasks_.erase(std::remove_if(tasks_.begin(), tasks_.end(), [id](const Task& t) {
        return t.id() == id;
    }), tasks_.end());
}

void TaskList::mark_done(int id) {
    for (auto& task : tasks_) {
        if (task.id() == id) {
            task.set_status(Status::DONE);
            break;
        }
    }
}

void TaskList::mark_pending(int id) {
    for (auto& task : tasks_) {
        if (task.id() == id) {
            task.set_status(Status::PENDING);
            break;
        }
    }
}

void TaskList::mark_doing(int id) {
    for (auto& task : tasks_) {
        if (task.id() == id) {
            task.set_status(Status::DOING);
            break;
        }
    }
}

void TaskList::clear() {
    tasks_.clear();
}

const std::vector<Task>& TaskList::tasks() const {
    return tasks_;
}

void TaskList::set_tasks(std::vector<Task> tasks) {
    tasks_ = std::move(tasks);
}

void TaskList::set_next_id(int id) {
    next_id_ = id;
}
