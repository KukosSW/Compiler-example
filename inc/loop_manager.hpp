#ifndef LOOP_MANAGER_HPP
#define LOOP_MANAGER_HPP

#include <stack>

#include <loop.hpp>

class Loop_manager
{
private:
    std::stack<Loop*> stack_loop;

public:
    void add_loop_to_stack(Loop* loop) noexcept;
    Loop* get_loop_from_stack() noexcept;
    Loop* get_loop_top_stack() noexcept;
};


#endif