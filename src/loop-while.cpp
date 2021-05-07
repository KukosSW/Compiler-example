#include <loop-while.hpp>

Loop_while::Loop_while(const std::string& start_label, const std::string& end_label) noexcept
: Loop(start_label, end_label, Loop::LOOP_TYPE_WHILE)
{

}