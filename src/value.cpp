#include <value.hpp>

Value::Value() noexcept
: type{VALTYPE_GENERAL}
{

}

Value::Value(valtype_t type) noexcept
: type{type}
{

}