#include <iostream>

#include <register.hpp>

Register::Register(const std::string& name) noexcept
: name{name}, free{true}
{

}

void Register::lock() noexcept
{
    if (!free)
    {
        std::cerr << "Register " << name << " is already locked\n";
        exit(EXIT_FAILURE);
    }

    free = false;
}

void Register::unlock() noexcept
{
    if (free)
    {
        std::cerr << "Register " << name << " is already unlocked\n";
        exit(EXIT_FAILURE);
    }

    free = true;
}