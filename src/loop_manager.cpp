#include <loop_manager.hpp>

void Loop_manager::add_loop_to_stack(Loop* loop) noexcept
{
    stack_loop.push(loop);
}

Loop* Loop_manager::get_loop_from_stack() noexcept
{
    Loop* loop = stack_loop.top();
    stack_loop.pop();

    return loop;
}

Loop* Loop_manager::get_loop_top_stack() noexcept
{
    return stack_loop.top();
}