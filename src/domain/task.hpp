#ifndef DOMAIN_TASK_HPP
#define DOMAIN_TASK_HPP

#include <string>

enum class Status {
    PENDING,
    DOING,
    DONE
};

class Task {
public:
    Task(int id, std::string description);

    int id() const;
    const std::string& description() const;
    Status status() const;
    void set_status(Status status);
    void mark_done();
    void mark_pending();

private:
    int id_;
    std::string description_;
    Status status_;
};

#endif // DOMAIN_TASK_HPP
