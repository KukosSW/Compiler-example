#ifndef CONDITIONAL_BRANCH_MANAGER
#define CONDITIONAL_BRANCH_MANAGER

#include <stack>

#include <conditional_branch.hpp>

class Conditional_branch_manager
{
private:
    std::stack<Conditional_branch> stack_branch;

public:
    void add_branch(const Conditional_branch& branch) noexcept;
    Conditional_branch get_branch_from_stack() noexcept;
};

#endif