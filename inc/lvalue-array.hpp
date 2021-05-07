#ifndef LVALUE_ARRAY_HPP
#define LVALUE_ARRAY_HPP

#include <memory>

#include <value.hpp>
#include <lvalue.hpp>

class Lvalue_array : public Lvalue
{
private:
    Architecture::addr_t offset;
    Architecture::addr_t size;
    std::shared_ptr<Value> access_element;

public:
    Lvalue_array(const std::string &name, Architecture::addr_t offset, Architecture::addr_t size) noexcept;
    Lvalue_array(const Lvalue_array& arr) noexcept;

    Architecture::addr_t get_size() const noexcept override { return size; }

    Architecture::addr_t get_access_offset() const noexcept { return offset; }
    const Value* get_access_element() const noexcept { return access_element.get(); }
    void set_access_element(const std::shared_ptr<Value>& access) { access_element = access; }
};

#endif