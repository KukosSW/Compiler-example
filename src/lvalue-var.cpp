#include <lvalue-var.hpp>

Lvalue_var::Lvalue_var(const std::string& name, bool is_mutuable) noexcept
: Lvalue(name, is_mutuable, VALTYPE_LVALUE_VAR)
{

}

Lvalue_var::Lvalue_var(const Lvalue_var& var) noexcept
: Lvalue(var.get_name(), var.is_mutuable(), var.get_type(), var.get_addr())
{

}