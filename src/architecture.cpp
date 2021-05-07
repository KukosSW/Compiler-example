#include <iostream>
#include <cstdlib>

#include <architecture.hpp>

// 0 is a NULL
Architecture::addr_t Architecture::first_free_addr = 1;

Register Architecture::registers[] = {Register("a"), Register("b"), Register("c"), Register("d"), Register("e"), Register("f")};

Architecture::addr_t Architecture::alloc(Architecture::addr_t bytes) noexcept
{
    addr_t alloc_addr = first_free_addr;
    first_free_addr += bytes;

    return alloc_addr;
}

Register& Architecture::get_free_register() noexcept
{
    // Register a is like eax in x86 is used to return value so it is a special register
    // cannot return Register a as normal free register
    for (size_t i = 1; i < registers_num; ++i)
        if (registers[i].is_free())
            return registers[i];

    // if we reached here we have critical issue
    std::cerr << "Critical issue, no free register" << std::endl;
    exit(EXIT_FAILURE);
}

Register& Architecture::get_retval_register() noexcept
{
    return registers[0];
}