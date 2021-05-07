#ifndef ARCHITECTURE_HPP
#define ARCHITECTURE_HPP

#include <cstdint>

#include <register.hpp>

class Architecture
{
public:
    typedef uint64_t addr_t;
    static Architecture::addr_t alloc(Architecture::addr_t bytes) noexcept;
    static Register& get_free_register() noexcept;
    static Register& get_retval_register() noexcept;
    static constexpr size_t registers_num = 6;
private:
    static Architecture::addr_t first_free_addr;
    static Register registers[registers_num];
};

#endif