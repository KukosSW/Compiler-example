#include<lvalue.hpp>

Lvalue::Lvalue() noexcept
: Value(VALTYPE_LVALUE), name{std::string("LVALUE")}, init{false}, mutuable{true}, addr{0}
{

}

Lvalue::Lvalue(const std::string& name, bool mutuable, Value::valtype_t type) noexcept
: Value(type), name{name}, init{false}, mutuable{mutuable}, addr{0}
{

}

Lvalue::Lvalue(const std::string& name, bool mutuable, Value::valtype_t type, Architecture::addr_t addr) noexcept
: Value(type), name{name}, init{false}, mutuable{mutuable}, addr{addr}
{

}