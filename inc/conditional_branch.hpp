#ifndef CONDITIONAL_BRANCH_HPP
#define CONDITIONAL_BRANCH_HPP

#include <string>

class Conditional_branch
{
private:
    std::string label_false;

public:
    Conditional_branch(const std::string& label_false) noexcept;

    const std::string& get_label_false() const noexcept { return label_false; }
};

#endif