#include "task.hpp"
#include <utility>

Task::Task(int id, std::string description) 
    : id_(id), description_(std::move(description)), status_(Status::PENDING) {}

int Task::id() const { return id_; }

const std::string& Task::description() const { return description_; }

Status Task::status() const { return status_; }

void Task::set_status(Status status) { status_ = status; }

void Task::mark_done() { status_ = Status::DONE; }

void Task::mark_pending() { status_ = Status::PENDING; }
