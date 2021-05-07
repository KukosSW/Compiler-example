#ifndef LVALUE_HPP
#define LVALUE_HPP

#include <value.hpp>

class Lvalue : public Value
{
protected:
    std::string               name;
    bool                      init;
    bool                      mutuable;
    Architecture::addr_t      addr;
public:

    /* I need a virtual default desctructor, I need also apply rule of 5 */
    Lvalue() noexcept;
    Lvalue(const std::string& name, bool mutuable, Value::valtype_t type) noexcept;
    Lvalue(const std::string& name, bool mutuable, Value::valtype_t type, Architecture::addr_t addr) noexcept;
    virtual ~Lvalue() = default;
    Lvalue(const Lvalue& from) = default;
    Lvalue& operator=(const Lvalue& from) = default;
    Lvalue(Lvalue &&) = default;
    Lvalue& operator=(Lvalue &&) = default;

    /* Interface for all Lvalues */
    virtual const std::string& get_name() const noexcept { return name; }
    virtual bool is_init() const noexcept { return init; }
    virtual bool is_mutuable() const noexcept  { return mutuable; }
    virtual Architecture::addr_t get_addr() const noexcept { return addr; }
    virtual Architecture::addr_t get_size() const noexcept = 0;

    virtual void set_addr(Architecture::addr_t addr) noexcept { this->addr = addr; }
    virtual void set_init() noexcept { init = true; }
};


#endif