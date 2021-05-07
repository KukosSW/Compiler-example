#include <rvalue.hpp>

Rvalue::Rvalue() noexcept
: Value(VALTYPE_RVALUE)
{

}

Rvalue::Rvalue(uint64_t value) noexcept
: Value(VALTYPE_RVALUE), value{value}
{

}