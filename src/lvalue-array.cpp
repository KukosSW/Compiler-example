#include <lvalue-array.hpp>

Lvalue_array::Lvalue_array(const std::string& name, Architecture::addr_t offset, Architecture::addr_t size) noexcept
: Lvalue(name, true, VALTYPE_LVALUE_ARRAY), offset{offset}, size{size}
{

}

Lvalue_array::Lvalue_array(const Lvalue_array& arr) noexcept
: Lvalue(arr.get_name(), arr.is_mutuable(), arr.get_type(), arr.get_addr()), offset{arr.get_access_offset()}, size{arr.get_size()}
{

}