#ifndef LOOP_WHILE_HPP
#define LOOP_WHILE_HPP

#include <loop.hpp>

class Loop_while : public Loop
{
public:
    Loop_while(const std::string& start_label, const std::string& end_label) noexcept;
};

#endif