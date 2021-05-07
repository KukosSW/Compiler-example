#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <string>

class Register
{
private:
    std::string name;
    bool free;

public:
    Register(const std::string& name) noexcept;
    const std::string& get_name() const noexcept { return name; }
    bool is_free() const noexcept { return free; }
    void lock() noexcept;
    void unlock() noexcept;
};

#endif