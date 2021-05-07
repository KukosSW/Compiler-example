#include <loop-until.hpp>

Loop_until::Loop_until(const std::string& start_label, const std::string& end_label) noexcept
: Loop(start_label, end_label, Loop::LOOP_TYPE_UNTIL)
{

}