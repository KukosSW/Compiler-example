#ifndef VALUE_HPP
#define VALUE_HPP

#include <string>
#include <cstdint>

#include <architecture.hpp>

class Value
{
public:
    typedef enum
    {
        VALTYPE_GENERAL,
        VALTYPE_RVALUE,
        VALTYPE_LVALUE,
        VALTYPE_LVALUE_VAR,
        VALTYPE_LVALUE_ARRAY
    } valtype_t;

protected:
    valtype_t type;

public:
    /* I need a virtual default desctructor, I need also apply rule of 5 */
    Value() noexcept;
    Value(valtype_t type) noexcept;
    virtual ~Value() = default;
    Value(const Value& from) = default;
    Value& operator=(const Value& from) = default;
    Value(Value &&) = default;
    Value& operator=(Value &&) = default;

    /* Interface for all Values */
    virtual valtype_t get_type() const noexcept { return type; }
};

#endif