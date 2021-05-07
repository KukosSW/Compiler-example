#ifndef LOOP_UNTIL_HPP
#define LOOP_UNTIL_HPP

#include <loop.hpp>

class Loop_until : public Loop
{
public:
    Loop_until(const std::string& start_label, const std::string& end_label) noexcept;
};

#endif