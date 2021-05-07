#ifndef LOOP_FOR_HPP
#define LOOP_FOR_HPP

#include <memory>

#include <loop.hpp>

class Loop_for : public Loop
{
private:
    std::shared_ptr<Lvalue> iterator;
    std::shared_ptr<Lvalue> counter;
public:
    Loop_for(const std::shared_ptr<Lvalue>& iterator, const std::shared_ptr<Lvalue>& counter, const std::string& start_label, const std::string& end_label, Loop::looptype_t type) noexcept;

    std::shared_ptr<Lvalue> get_iterator() const noexcept { return iterator; }
    std::shared_ptr<Lvalue> get_counter() const noexcept { return counter; }
};

#endif