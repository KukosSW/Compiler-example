#include <loop-for.hpp>

Loop_for::Loop_for(const std::shared_ptr<Lvalue>& iterator, const std::shared_ptr<Lvalue>& counter, const std::string& start_label, const std::string& end_label, Loop::looptype_t type) noexcept
: Loop(start_label, end_label, type), iterator{iterator}, counter{counter}
{

}