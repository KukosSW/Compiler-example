#ifndef LOOP_HPP
#define LOOP_HPP

#include <memory>

#include <lvalue.hpp>

class Loop
{
public:
    typedef enum
    {
        LOOP_TYPE_FOR_DO,
        LOOP_TYPE_FOR_DOWNTO,
        LOOP_TYPE_WHILE,
        LOOP_TYPE_UNTIL
    } looptype_t;

private:
    std::string start_label;
    std::string end_label;
    looptype_t type;

public:
    Loop(const std::string& start_label, const std::string& end_label, looptype_t type) noexcept;

    // Rule of 5
    virtual ~Loop() = default;
    Loop(const Loop& from) = default;
    Loop& operator=(const Loop& from) = default;
    Loop(Loop &&) = default;
    Loop& operator=(Loop &&) = default;

    const std::string& get_start_label() const noexcept { return start_label; }
    const std::string& get_end_label() const noexcept { return end_label; }
    void set_start_label(const std::string& label) noexcept { start_label = label; }
    void set_end_label(const std::string& label) noexcept { end_label = label; }
    looptype_t get_type() const noexcept { return type; }
};

#endif


