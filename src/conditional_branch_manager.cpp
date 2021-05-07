#include <conditional_branch_manager.hpp>

void Conditional_branch_manager::add_branch(const Conditional_branch& branch) noexcept
{
    stack_branch.push(branch);
}

Conditional_branch Conditional_branch_manager::get_branch_from_stack() noexcept
{
    Conditional_branch branch = stack_branch.top();
    stack_branch.pop();

    return branch;
}