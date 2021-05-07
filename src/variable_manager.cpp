#include <cstdlib>
#include <iostream>

#include <variable_manager.hpp>

void Variable_manager::declare_variable(Lvalue* val) noexcept
{
    // Alloc memory
    val->set_addr(Architecture::alloc(val->get_size()));

    // Declare variable
    variables[val->get_name()] = std::shared_ptr<Lvalue>(val);
}

void Variable_manager::undeclare_variable(Lvalue* val) noexcept
{
    variables.erase(val->get_name());
}

std::shared_ptr<Lvalue>& Variable_manager::get_variable(const std::string& name) noexcept
{
    if (!is_variable_declared(name))
    {
        std::cerr << "Critical issue, getting undeclare variable" << std::endl;
        exit(EXIT_FAILURE);
    }

    return variables[name];
}

bool Variable_manager::is_variable_declared(const std::string& name) const noexcept
{
    return variables.find(name) != variables.end();
}