#ifndef COMPILER_HPP
#define COMPILER_HPP

#include <string>
#include <cstdint>

#include <variable_manager.hpp>
#include <loop_manager.hpp>
#include <assembler_generator.hpp>
#include <value.hpp>
#include <conditional_branch_manager.hpp>

class Compiler
{
private:
    Variable_manager var_manager;
    Loop_manager loop_manager;
    Assembler_generator asm_generator;
    Conditional_branch_manager branch_manager;
public:
    Variable_manager& get_var_manager() noexcept { return var_manager; }
    Loop_manager& get_loop_manager() noexcept { return loop_manager; }
    Assembler_generator& get_asm_generator() noexcept { return asm_generator; }
    Conditional_branch_manager& get_branch_manager() noexcept { return branch_manager; }

    // C like function. Print error using fprintf and va_arg and exit program
    void error(const char *fmt, ...);

    // Assertions
    void assert_declaration(const std::string& name, uint64_t line);
    void assert_redeclaration(const std::string& name, uint64_t line);
    void assert_usage(const std::string& name, Value::valtype_t type, uint64_t line);
    void assert_initalization(Value *val, uint64_t line);
    void assert_mutuable(Value *val, uint64_t line);

    void init_variable(Value* val);
};


#endif