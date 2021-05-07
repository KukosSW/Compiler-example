#include <loop.hpp>

Loop::Loop(const std::string& start_label, const std::string& end_label,  looptype_t type) noexcept
: start_label{start_label}, end_label{end_label}, type{type}
{

}