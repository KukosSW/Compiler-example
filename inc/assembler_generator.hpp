#ifndef ASSEMBLER_GENERATOR_HPP
#define ASSEMBLER_GENERATOR_HPP

#include <vector>
#include <string>
#include <map>
#include <cstdint>

#include <value.hpp>
#include <lvalue.hpp>
#include <register.hpp>
#include <architecture.hpp>
#include <loop.hpp>
#include <loop-for.hpp>
#include <loop-while.hpp>
#include <loop-until.hpp>
#include <conditional_branch.hpp>

class Assembler_generator
{
public:
    // nested class to manage labels in jumps
    class Label_manager
    {
    private:
        Assembler_generator* asm_generator;
        std::map<std::string, std::pair<uint64_t, std::vector<uint64_t>>> labels;
        uint64_t label_counter;
    public:
        Label_manager(Assembler_generator* asm_generator) noexcept;

        // All labels are tracked using counter, so conflicts are impossible
        std::string create_label(const std::string& name) noexcept;
        std::string create_label() noexcept; // default name will be used

        void insert_label(const std::string& label) noexcept;
        void jump_to_label(const std::string& jump_code, const std::string& label) noexcept;
        void generate_code_from_labels() noexcept;
    };

    friend class Assembler_generator::Label_manager;
private:
    Assembler_generator::Label_manager label_manager;

    // manage asm code
    std::vector<std::string> code;
    void add_code(const std::string& line) noexcept;
    size_t get_current_asm_line() const noexcept;

    // Instructions defined by VM. Methods generate and add code into code vector
    void asm_get(const Register& x) noexcept;
    void asm_put(const Register& x) noexcept;
    void asm_load(const Register& x, const Register& y) noexcept;
    void asm_store(const Register& x, const Register& y) noexcept;
    void asm_add(const Register& x, const Register& y) noexcept;
    void asm_sub(const Register& x, const Register& y) noexcept;
    void asm_reset(const Register& x) noexcept;
    void asm_inc(const Register& x) noexcept;
    void asm_dec(const Register& x) noexcept;
    void asm_shr(const Register& x) noexcept;
    void asm_shl(const Register& x) noexcept;
    void asm_jump(int64_t offset) noexcept;
    void asm_jzero(const Register& x, int64_t offset) noexcept;
    void asm_jodd(const Register& x, int64_t offset) noexcept;
    void asm_jump_label(const std::string& label) noexcept;
    void asm_jzero_label(const Register& x, const std::string& label) noexcept;
    void asm_jodd_label(const Register& x, const std::string& label) noexcept;
    void asm_halt() noexcept;

    // private algorithm to pump rvalue and addr to register. Methods generate and add code into code vector
    void move_rvalue_to_reg(const Register& x, uint64_t val) noexcept;
    void move_addr_to_reg(const Register& x, const Lvalue& var) noexcept;

public:
    // rule of 5
    Assembler_generator() noexcept;
    virtual ~Assembler_generator() = default;
    Assembler_generator(const Assembler_generator& from) = default;
    Assembler_generator& operator=(const Assembler_generator& from) = default;
    Assembler_generator(Assembler_generator &&) = default;
    Assembler_generator& operator=(Assembler_generator &&) = default;

    // Methods defined by Language. Generator produce code and write into code vector
    void load(const Register& x, const Value& val) noexcept;
    void store(const Lvalue& var, const Register& x) noexcept;

    void read(const Lvalue& var) noexcept;
    void write(const Value& val) noexcept;

    void add(const Value& val1, const Value& val2) noexcept;
    void sub(const Value& val1, const Value& val2) noexcept;
    void mul(const Value& val1, const Value& val2) noexcept;
    void div(const Value& val1, const Value& val2) noexcept;
    void mod(const Value& val1, const Value& val2) noexcept;

    // Create cond branch
    Conditional_branch branch_eq(const Value& val1, const Value& val2) noexcept;
    Conditional_branch branch_neq(const Value& val1, const Value& val2) noexcept;
    Conditional_branch branch_gt(const Value& val1, const Value& val2) noexcept;
    Conditional_branch branch_lt(const Value& val1, const Value& val2) noexcept;
    Conditional_branch branch_geq(const Value& val1, const Value& val2) noexcept;
    Conditional_branch branch_leq(const Value& val1, const Value& val2) noexcept;

    // Iteration = (to + 1) - from Result will be in retval register
    void calculate_for_iterations(const Value& from, const Value& to) noexcept;

    void start_for_loop(const Loop_for& loop) noexcept;
    void do_for_loop(const Loop_for& loop) noexcept;

    void start_while_loop(const Loop_while& loop) noexcept;
    void do_while_loop(const Loop_while& loop) noexcept;

    void start_until_loop(const Loop_until& loop) noexcept;
    void do_until_loop(const Loop_until& loop) noexcept;

    void if_insert_jump_point(const Conditional_branch& branch) noexcept;
    void if_skip_else(const Conditional_branch& branch) noexcept;

    void finish_program() noexcept;

    // finish generation, create final code based on some metadata like data from label_manager
    void finish_code_generation() noexcept;

    // get code in 1 string
    std::string get_generated_code() const noexcept;

    // get Label manager to create lables from another class like parser
    Assembler_generator::Label_manager& get_label_manager() noexcept { return label_manager; };
};

#endif