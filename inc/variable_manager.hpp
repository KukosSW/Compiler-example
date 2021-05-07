#ifndef VARIABLE_MANAGER_HPP
#define VARIABLE_MANAGER_HPP

#include <map>
#include <string>
#include <memory>

#include <lvalue.hpp>

class Variable_manager
{
private:
    std::map<std::string, std::shared_ptr<Lvalue>> variables;

public:
    void declare_variable(Lvalue* val) noexcept;
    void undeclare_variable(Lvalue* val) noexcept;
    bool is_variable_declared(const std::string& name) const noexcept;
    std::shared_ptr<Lvalue>& get_variable(const std::string& name) noexcept;
};


#endif