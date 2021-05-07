#ifndef RVALUE_HPP
#define RVALUE_HPP

#include <value.hpp>

class Rvalue : public Value
{
private:
    uint64_t value;
public:
    Rvalue() noexcept;
    Rvalue(uint64_t value) noexcept;

    uint64_t get_value() const noexcept { return value; }
};

#endif