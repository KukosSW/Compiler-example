#include <bitset>
#include <bits/stdc++.h>

#include <assembler_generator.hpp>
#include <assembler.hpp>
#include <rvalue.hpp>
#include <lvalue.hpp>
#include <lvalue-var.hpp>
#include <lvalue-array.hpp>

Assembler_generator::Label_manager::Label_manager(Assembler_generator* asm_generator) noexcept
: asm_generator{asm_generator}, label_counter{1}
{

}

std::string Assembler_generator::Label_manager::create_label(const std::string& name) noexcept
{
    return name + "_" + std::to_string(label_counter++);
}

std::string Assembler_generator::Label_manager::create_label() noexcept
{
    return create_label("LABEL");
}

void Assembler_generator::Label_manager::insert_label(const std::string& label) noexcept
{
    uint64_t label_line = asm_generator->get_current_asm_line() + 1;

    auto lab = labels.find(label);
    // insert_label was called before jump_to_label, so there is no label. Add label
    if (lab == labels.end())
        labels[label] = std::make_pair(label_line, std::vector<uint64_t>());
    else // someone set line as a jump point from to label, so now set only jump point to as a label_line
        lab->second.first = label_line;
}

void Assembler_generator::Label_manager::jump_to_label(const std::string& jump_code, const std::string& label) noexcept
{
    // jump_code is incomplete as JUMP or JZERO x or JODD y, but this is fine. Add it to code and add jump from point to label
    asm_generator->add_code(jump_code);

    // jump code is here, so current line is our jump to point
    uint64_t jump_line = asm_generator->get_current_asm_line();
    auto lab = labels.find(label);

    // jump_to_label was called before insert_label, so there is no label.
    // Add label with invalid label line, but add jump from point to vector
    if (lab == labels.end())
        labels[label] = std::make_pair(0, std::vector<uint64_t>{jump_line});
    else // there is a label here, so add another jump from point to vector
        lab->second.second.push_back(jump_line);
}

void Assembler_generator::Label_manager::generate_code_from_labels() noexcept
{
    for (auto& label : labels)
    {
        uint64_t jump_to = label.second.first;
        auto& jump_from_vector = label.second.second;
        for (auto& jump_from : jump_from_vector)
        {
            int64_t jump_offset = static_cast<int64_t>(jump_to) - static_cast<int64_t>(jump_from);
            asm_generator->code[jump_from - 1] += " " + std::to_string(jump_offset) + "\n";
        }
        // we complete code based on this vector, we dont need values from this vector
        jump_from_vector.clear();
    }
}

Assembler_generator::Assembler_generator() noexcept
: label_manager{Assembler_generator::Label_manager(this)}
{

}

void Assembler_generator::add_code(const std::string& line) noexcept
{
    code.push_back(line);
}

size_t Assembler_generator::get_current_asm_line() const noexcept
{
    return code.size();
}

void Assembler_generator::asm_get(const Register& x) noexcept
{
    add_code(Assembler::instruction_get() + " " + x.get_name() + "\n");
}

void Assembler_generator::asm_put(const Register& x) noexcept
{
    add_code(Assembler::instruction_put() + " " + x.get_name() + "\n");
}

void Assembler_generator::asm_reset(const Register& x) noexcept
{
    add_code(Assembler::instruction_reset() + " " + x.get_name() + "\n");
}

void Assembler_generator::asm_inc(const Register& x) noexcept
{
    add_code(Assembler::instruction_inc() + " " + x.get_name() + "\n");
}

void Assembler_generator::asm_dec(const Register& x) noexcept
{
    add_code(Assembler::instruction_dec() + " " + x.get_name() + "\n");
}

void Assembler_generator::asm_shr(const Register& x) noexcept
{
    add_code(Assembler::instruction_shr() + " " + x.get_name() + "\n");
}

void Assembler_generator::asm_shl(const Register& x) noexcept
{
    add_code(Assembler::instruction_shl() + " " + x.get_name() + "\n");
}

void Assembler_generator::asm_load(const Register& x, const Register& y) noexcept
{
    add_code(Assembler::instruction_load() + " " + x.get_name() + " " + y.get_name() +  "\n");
}

void Assembler_generator::asm_store(const Register& x, const Register& y) noexcept
{
    add_code(Assembler::instruction_store() + " " + x.get_name() + " " + y.get_name() +  "\n");
}

void Assembler_generator::asm_add(const Register& x, const Register& y) noexcept
{
    add_code(Assembler::instruction_add() + " " + x.get_name() + " " + y.get_name() +  "\n");
}

void Assembler_generator::asm_sub(const Register& x, const Register& y) noexcept
{
    add_code(Assembler::instruction_sub() + " " + x.get_name() + " " + y.get_name() +  "\n");
}

void Assembler_generator::asm_jump(int64_t offset) noexcept
{
    add_code(Assembler::instruction_jump() + " " + std::to_string(offset) + "\n");
}

void Assembler_generator::asm_jzero(const Register& x, int64_t offset) noexcept
{
    add_code(Assembler::instruction_jzero() + " " + x.get_name() + " " + std::to_string(offset) + "\n");
}

void Assembler_generator::asm_jodd(const Register& x, int64_t offset) noexcept
{
    add_code(Assembler::instruction_jodd() + " " + x.get_name() + " " + std::to_string(offset) + "\n");
}

void Assembler_generator::asm_jump_label(const std::string& label) noexcept
{
    label_manager.jump_to_label(Assembler::instruction_jump(), label);
}

void Assembler_generator::asm_jzero_label(const Register& x, const std::string& label) noexcept
{
    label_manager.jump_to_label(Assembler::instruction_jzero() + " " + x.get_name(), label);
}

void Assembler_generator::asm_jodd_label(const Register& x, const std::string& label) noexcept
{
    label_manager.jump_to_label(Assembler::instruction_jodd() + " " + x.get_name(), label);
}

void Assembler_generator::asm_halt() noexcept
{
    add_code(Assembler::instruction_halt() + "\n");
}

void Assembler_generator::move_rvalue_to_reg(const Register& x, uint64_t val) noexcept
{
    std::bitset<sizeof(val) * CHAR_BIT> bits(val);

    size_t i;
    for (i = bits.size() - 1; i > 0; --i)
        if (bits[i])
            break;

    asm_reset(x);
    for (; i > 0; --i)
        if (bits[i])
        {
            asm_inc(x);
            asm_shl(x);
        }
        else
        {
            asm_shl(x);
        }

    if (bits[i])
        asm_inc(x);
}

void Assembler_generator::finish_code_generation() noexcept
{
    label_manager.generate_code_from_labels();
}

std::string Assembler_generator::get_generated_code() const noexcept
{
    std::string generated_code = "";

    for (auto const& line: code)
        generated_code += line;

    return generated_code;
}

void Assembler_generator::finish_program() noexcept
{
    asm_halt();
}

void Assembler_generator::move_addr_to_reg(const Register& x, const Lvalue& var) noexcept
{
    if (var.get_type() == Value::VALTYPE_LVALUE_VAR)
        move_rvalue_to_reg(x, dynamic_cast<const Lvalue_var&>(var).get_addr());
    else // Array, t[a] or t[10]
    {
        const Lvalue_array& arr = dynamic_cast<const Lvalue_array&>(var);
        move_rvalue_to_reg(x, arr.get_addr()); // x := &arr[0]

        Register& temp = Architecture::get_free_register();
        temp.lock();

        const Value* access_element = arr.get_access_element();
        if (access_element->get_type() == Value::VALTYPE_RVALUE)
            move_rvalue_to_reg(temp, dynamic_cast<const Rvalue*>(access_element)->get_value());
        else
        {
            move_rvalue_to_reg(temp, dynamic_cast<const Lvalue*>(access_element)->get_addr());
            asm_load(temp, temp); // temp := *(&access_element)
        }

        asm_add(x, temp); // x := &arr[access_element]

        move_rvalue_to_reg(temp, arr.get_access_offset()); // temp := offset

        asm_sub(x, temp); // x := &arr[access_element - array_offset]

        temp.unlock();
    }
}

void Assembler_generator::load(const Register& x, const Value& val) noexcept
{
    if (val.get_type() == Value::VALTYPE_RVALUE)
        move_rvalue_to_reg(x, dynamic_cast<const Rvalue&>(val).get_value());
    else
    {
        // x := &var or x := &arr[i]
        move_addr_to_reg(x, dynamic_cast<const Lvalue&>(val));

        // x := *(&var)
        asm_load(x, x);
    }
}

void Assembler_generator::store(const Lvalue& var, const Register& x) noexcept
{
    Register& temp = Architecture::get_free_register();
    temp.lock();

    // temp := &var
    move_addr_to_reg(temp, var);

    asm_store(x, temp);

    temp.unlock();
}

void Assembler_generator::read(const Lvalue& var) noexcept
{
    Register& ret = Architecture::get_retval_register();
    ret.lock();

    // ret := &var
    move_addr_to_reg(ret, var);

    // *ret := scan from stdin
    asm_get(ret);

    ret.unlock();
}

void Assembler_generator::write(const Value& val) noexcept
{
    Register& ret = Architecture::get_retval_register();
    ret.lock();

    if (val.get_type() == Value::VALTYPE_RVALUE)
    {
        // Alloc new memory for this value (we wont deallocate it), we have 2^64 memory
        // and this kind of leaks are only possible by writting some values into code like WRITE 10,
        // so this is safe
        Architecture::addr_t val_addr = Architecture::alloc(1);
        move_rvalue_to_reg(ret, val_addr); // x := &memory

        Register& temp = Architecture::get_free_register();
        temp.lock();

        // temp := val
        move_rvalue_to_reg(temp, dynamic_cast<const Rvalue&>(val).get_value());

        // *ret = val
        asm_store(temp, ret);

        temp.unlock();
    }
    else
        // ret := &var
        move_addr_to_reg(ret, dynamic_cast<const Lvalue&>(val));

    // here ret := &var or ret := &(int){5}, so we can print it
    asm_put(ret);

    ret.unlock();
}

void Assembler_generator::add(const Value& val1, const Value& val2) noexcept
{
    Register& retval = Architecture::get_retval_register();
    retval.lock();

    Register& temp = Architecture::get_free_register();
    temp.lock();

    load(retval, val1); // retval = val1
    load(temp, val2); // temp = val1

    asm_add(retval, temp); // retval = retval + temp

    temp.unlock();

    // retval will be used in assignment, so dont unlock it now
}

void Assembler_generator::sub(const Value& val1, const Value& val2) noexcept
{
    Register& retval = Architecture::get_retval_register();
    retval.lock();

    Register& temp = Architecture::get_free_register();
    temp.lock();

    load(retval, val1); // retval = val1
    load(temp, val2); // temp = val1

    asm_sub(retval, temp); // retval = retval - temp

    temp.unlock();

    // retval will be used in assignment, so dont unlock it now
}

void Assembler_generator::mul(const Value& val1, const Value& val2) noexcept
{
    Register& retval = Architecture::get_retval_register();
    retval.lock();

    Register& temp1 = Architecture::get_free_register();
    temp1.lock();
    load(temp1, val1);

    Register& temp2 = Architecture::get_free_register();
    temp2.lock();
    load(temp2, val2);

    const std::string label_end = label_manager.create_label("MUL_END");
    const std::string label_loop = label_manager.create_label("MUL_LOOP");
    const std::string label_inside_loop = label_manager.create_label("MUL_INSIDE_LOOP");
    const std::string label_odd = label_manager.create_label("MUL_ODD");

    // Peasant Multiplication
    asm_reset(retval);
    asm_jzero_label(temp1, label_end);

    // we reuse this jzero as initial check and loop check
    label_manager.insert_label(label_loop);
    asm_jzero_label(temp2, label_end);
    asm_jodd_label(temp2, label_odd);
    label_manager.insert_label(label_inside_loop);
    asm_shl(temp1);
    asm_shr(temp2);
    asm_jump_label(label_loop);

    label_manager.insert_label(label_odd);
    asm_add(retval, temp1);
    asm_jump_label(label_inside_loop);

    temp1.unlock();
    temp2.unlock();

    label_manager.insert_label(label_end);
    // retval will be used in assignment, so dont unlock it now
}

// Algo adopted from this presentation: https://www.cs.utah.edu/~rajeev/cs3810/slides/3810-08.pdf?fbclid=IwAR3QdWK4L9jG6xY_MkhvlxdzxYalvqHXrP5zP63OoNQ0XkTHGzcQUOkijg0
void Assembler_generator::div(const Value& val1, const Value& val2) noexcept
{
    // PREPARATION PHASE
    // Alloc 2 variables (clone of val1 and val2) to make reload easier (1 reg instead of 2 in case of array)
    Lvalue* clone_val1 = new Lvalue_var("clone1", true);
    clone_val1->set_addr(Architecture::alloc(1));

    Lvalue* clone_val2 = new Lvalue_var("clone2", true);
    clone_val2->set_addr(Architecture::alloc(1));

    // Copy val1 to clone_val1 and val2_to clone_val2
    Register& temp1 = Architecture::get_free_register();
    temp1.lock();

    load(temp1, val1);
    store(*clone_val1, temp1);

    Register& temp2 = Architecture::get_free_register();
    temp2.lock();

    load(temp2, val2);
    store(*clone_val2, temp2);

    // get temp3 as temp1 (val1) and temp4 as temp1 (val2)
    Register& temp3 = Architecture::get_free_register();
    temp3.lock();

    load(temp3, *clone_val1);

    Register& temp4 = Architecture::get_free_register();
    temp4.lock();

    load(temp4, *clone_val2);
    // Now: temp1 = temp3 = val1, temp2 = temp4 = val2

    Register& retval = Architecture::get_retval_register();
    retval.lock();

    //CHECK PHASE
    // 0 / 0 -> 0, a / 0 -> 0, 0 / b -> 0
    // a / b -> 1
    // a / b -> 0 if a < b

    const std::string label_end = label_manager.create_label("DIV_END");
    const std::string label_ret0 = label_manager.create_label("DIV_RETURN_0");
    const std::string label_ret1 = label_manager.create_label("DIV_RETURN_1");
    const std::string label_start_algo = label_manager.create_label("DIV_START_ALGO");

    // return 0
    asm_jzero_label(temp1, label_ret0);
    asm_jzero_label(temp2, label_ret0);

    // max{a - b, 0} + max{b - a, 0} == 0 iff a == b. But we know that a != 0 && b & 0 so return 1
    asm_sub(temp1, temp2);
    asm_sub(temp2, temp3);
    asm_add(temp1, temp2);
    asm_jzero_label(temp1, label_ret1);

    // a < b -> a != b && a - b == 0. We know that a != 0, so a < b. Return 0
    asm_sub(temp3, temp4);
    asm_jzero_label(temp3, label_ret0);
    asm_jump_label(label_start_algo);

    label_manager.insert_label(label_ret0);
    asm_reset(retval);
    asm_jump_label(label_end);
    label_manager.insert_label(label_ret1);
    asm_reset(retval);
    asm_inc(retval);
    asm_jump_label(label_end);

    // we dont need temp4 anymore
    temp4.unlock();

    // MAIN ALGO PHASE
    // Check Phase destroys a and b in registers we need to reload this
    const std::string label_count_0 = label_manager.create_label("DIV_COUNT_0");
    const std::string label_reload_b = label_manager.create_label("DIV_RELOAD_B");
    const std::string label_add_0 = label_manager.create_label("DIV_ADD_0");
    const std::string label_finish_add_0 = label_manager.create_label("DIV_FINISH_ADD_0");
    const std::string label_loop = label_manager.create_label("DIV_COUNT_LOOP");
    const std::string label_b_gt = label_manager.create_label("DIV_B_IS_GREATER");

    label_manager.insert_label(label_start_algo);
    load(temp1, *clone_val1);
    load(temp2, *clone_val2);

    move_rvalue_to_reg(retval, 0);
    move_rvalue_to_reg(temp3, 1);

    label_manager.insert_label(label_count_0);
    asm_jzero_label(temp2, label_reload_b);
    asm_shr(temp2);
    asm_shr(temp1);
    asm_jump_label(label_count_0);

    label_manager.insert_label(label_reload_b);
    load(temp2, *clone_val2); // we have broken "b" in temp2 but we need fresh "b" -> reload

    label_manager.insert_label(label_add_0);
    asm_jzero_label(temp1, label_finish_add_0);
    asm_shr(temp1);
    asm_shl(temp2);
    asm_inc(temp3);
    asm_jump_label(label_add_0);

    label_manager.insert_label(label_finish_add_0);
    load(temp1, *clone_val1); // as above here we used "a" to calculate temp3 (counter). Reload fresh "a"

    // MAIN DIV LOOP
    label_manager.insert_label(label_loop);
    asm_jzero_label(temp3, label_end);
    asm_inc(temp1);
    asm_sub(temp1, temp2);
    asm_jzero_label(temp1, label_b_gt);
    asm_dec(temp1);
    asm_dec(temp3);
    asm_shl(retval);
    asm_inc(retval);
    asm_shr(temp2);

    // store current value in memory, we need current value in reg and prev value in memory
    store(*clone_val1, temp1);
    asm_jump_label(label_loop);

    label_manager.insert_label(label_b_gt);
    load(temp1, *clone_val1); // get prev value of "a"
    asm_shl(retval);
    asm_dec(temp3);
    asm_shr(temp2);
    asm_jump_label(label_loop);

    label_manager.insert_label(label_end);

    temp1.unlock();
    temp2.unlock();
    temp3.unlock();

    delete clone_val1;
    delete clone_val2;
}

// Algo adopted from this presentation: https://www.cs.utah.edu/~rajeev/cs3810/slides/3810-08.pdf?fbclid=IwAR3QdWK4L9jG6xY_MkhvlxdzxYalvqHXrP5zP63OoNQ0XkTHGzcQUOkijg0
void Assembler_generator::mod(const Value& val1, const Value& val2) noexcept
{
    // PREPARATION PHASE
    // Alloc 2 variables (clone of val1 and val2) to make reload easier (1 reg instead of 2 in case of array)
    Lvalue* clone_val1 = new Lvalue_var("clone1", true);
    clone_val1->set_addr(Architecture::alloc(1));

    Lvalue* clone_val2 = new Lvalue_var("clone2", true);
    clone_val2->set_addr(Architecture::alloc(1));

    // Copy val1 to clone_val1 and val2_to clone_val2
    Register& temp1 = Architecture::get_free_register();
    temp1.lock();

    load(temp1, val1);
    store(*clone_val1, temp1);

    Register& temp2 = Architecture::get_free_register();
    temp2.lock();

    load(temp2, val2);
    store(*clone_val2, temp2);

    // get temp3 as temp1 (val1) and temp4 as temp1 (val2)
    Register& temp3 = Architecture::get_free_register();
    temp3.lock();

    load(temp3, *clone_val1);

    Register& temp4 = Architecture::get_free_register();
    temp4.lock();

    load(temp4, *clone_val2);
    // Now: temp1 = temp3 = val1, temp2 = temp4 = val2

    Register& retval = Architecture::get_retval_register();
    retval.lock();

    // CHECK PHASE
    // 0 % 0 -> 0, a % 0 -> 0, 0 % b -> 0
    // a % b -> 0
    // a % b -> a if a < b

    const std::string label_end = label_manager.create_label("MOD_END");
    const std::string label_ret0 = label_manager.create_label("MOD_RETURN_0");
    const std::string label_reta = label_manager.create_label("MOD_RETURN_a");
    const std::string label_start_algo = label_manager.create_label("MOD_START_ALGO");

    // return 0
    asm_jzero_label(temp1, label_ret0);
    asm_jzero_label(temp2, label_ret0);

    // max{a - b, 0} + max{b - a, 0} == 0 iff a == b. But we know that a != 0 && b & 0 so return 0
    asm_sub(temp1, temp2);
    asm_sub(temp2, temp3);
    asm_add(temp1, temp2);
    asm_jzero_label(temp1, label_ret0);

    // a < b -> a != b && a - b == 0. We know that a != 0, so a < b. Return a
    asm_sub(temp3, temp4);
    asm_jzero_label(temp3, label_reta);
    asm_jump_label(label_start_algo);

    label_manager.insert_label(label_ret0);
    asm_reset(retval);
    asm_jump_label(label_end);
    label_manager.insert_label(label_reta);
    load(retval, *clone_val1);
    asm_jump_label(label_end);

    // we dont need temp4 anymore
    temp4.unlock();

    // MAIN ALGO PHASE
    // Check Phase destroys a and b in registers we need to reload this
    const std::string label_count_0 = label_manager.create_label("MOD_COUNT_0");
    const std::string label_reload_b = label_manager.create_label("MOD_RELOAD_B");
    const std::string label_add_0 = label_manager.create_label("MOD_ADD_0");
    const std::string label_finish_add_0 = label_manager.create_label("MOD_FINISH_ADD_0");
    const std::string label_loop = label_manager.create_label("MOD_COUNT_LOOP");
    const std::string label_b_gt = label_manager.create_label("MOD_B_IS_GREATER");

    label_manager.insert_label(label_start_algo);
    load(retval, *clone_val1);
    load(temp2, *clone_val2);

    move_rvalue_to_reg(temp1, 0);
    move_rvalue_to_reg(temp3, 1);

    label_manager.insert_label(label_count_0);
    asm_jzero_label(temp2, label_reload_b);
    asm_shr(temp2);
    asm_shr(retval);
    asm_jump_label(label_count_0);

    label_manager.insert_label(label_reload_b);
    load(temp2, *clone_val2); // we have broken "b" in temp2 but we need fresh "b" -> reload

    label_manager.insert_label(label_add_0);
    asm_jzero_label(retval, label_finish_add_0);
    asm_shr(retval);
    asm_shl(temp2);
    asm_inc(temp3);
    asm_jump_label(label_add_0);

    label_manager.insert_label(label_finish_add_0);
    load(retval, *clone_val1); // as above here we used "a" to calculate temp3 (counter). Reload fresh "a"

    // MAIN DIV LOOP
    label_manager.insert_label(label_loop);
    asm_jzero_label(temp3, label_end);
    asm_inc(retval);
    asm_sub(retval, temp2);
    asm_jzero_label(retval, label_b_gt);
    asm_dec(retval);
    asm_dec(temp3);
    asm_shl(temp1);
    asm_inc(temp1);
    asm_shr(temp2);

    // store current value in memory, we need current value in reg and prev value in memory
    store(*clone_val1, retval);
    asm_jump_label(label_loop);

    label_manager.insert_label(label_b_gt);
    load(retval, *clone_val1); // get prev value of "a"
    asm_shl(temp1);
    asm_dec(temp3);
    asm_shr(temp2);
    asm_jump_label(label_loop);

    label_manager.insert_label(label_end);

    temp1.unlock();
    temp2.unlock();
    temp3.unlock();

    delete clone_val1;
    delete clone_val2;
}

Conditional_branch Assembler_generator::branch_eq(const Value& val1, const Value& val2) noexcept
{
    const std::string label_true = label_manager.create_label("COND_EQ_TRUE");
    const std::string label_false = label_manager.create_label("COND_EQ_FALSE");
    const std::string label_double_check = label_manager.create_label("COND_EQ_DOUBLE_CHECK");

    Register& temp1 = Architecture::get_free_register();
    temp1.lock();

    Register& temp2 = Architecture::get_free_register();
    temp2.lock();

    load(temp1, val1);
    load(temp2, val2);

    // temp1 = max{temp1 - temp2, 0}. So 0 -> temp1 == temp2 or temp2 > temp1
    // so in case of zero we need to check it again
    // in case of != 0 it is for sure false
    asm_sub(temp1, temp2);
    asm_jzero_label(temp1, label_double_check);
    asm_jump_label(label_false);

    label_manager.insert_label(label_double_check);
    // temp1 == temp2 or temp2 > temp1. Reorder sub to check if temp1 > temp2 or temp1 == temp2.
    // we know that temp1 > temp2 == false.
    load(temp1, val1);
    asm_sub(temp2, temp1);
    asm_jzero_label(temp2, label_true);
    asm_jump_label(label_false);

    label_manager.insert_label(label_true);

    temp1.unlock();
    temp2.unlock();

    // Create Branch
    return Conditional_branch(label_false);
}

Conditional_branch Assembler_generator::branch_neq(const Value& val1, const Value& val2) noexcept
{
    const std::string label_true = label_manager.create_label("COND_NEQ_TRUE");
    const std::string label_false = label_manager.create_label("COND_NEQ_FALSE");
    const std::string label_double_check = label_manager.create_label("COND_NEQ_DOUBLE_CHECK");

    Register& temp1 = Architecture::get_free_register();
    temp1.lock();

    Register& temp2 = Architecture::get_free_register();
    temp2.lock();

    load(temp1, val1);
    load(temp2, val2);

    // temp1 = max{temp1 - temp2, 0}. So 0 -> temp1 == temp2 or temp2 > temp1
    // so in case of zero we need to check it again
    // in case of != 0 it is for sure true
    asm_sub(temp1, temp2);
    asm_jzero_label(temp1, label_double_check);
    asm_jump_label(label_true);

    label_manager.insert_label(label_double_check);
    // temp1 == temp2 or temp2 > temp1. Reorder sub to check if temp1 > temp2 or temp1 == temp2.
    // we know that temp1 > temp2 == false.
    load(temp1, val1);
    asm_sub(temp2, temp1);
    asm_jzero_label(temp2, label_false);

    label_manager.insert_label(label_true);

    temp1.unlock();
    temp2.unlock();

    // Create Branch
    return Conditional_branch(label_false);
}

Conditional_branch Assembler_generator::branch_gt(const Value& val1, const Value& val2) noexcept
{
    const std::string label_true = label_manager.create_label("COND_GT_TRUE");
    const std::string label_false = label_manager.create_label("COND_GT_FALSE");

    Register& temp1 = Architecture::get_free_register();
    temp1.lock();

    Register& temp2 = Architecture::get_free_register();
    temp2.lock();

    load(temp1, val1);
    load(temp2, val2);

    // temp2 = temp2 - temp1. temp2 == 0 <-> temp2 == temp1 or temp1 > temp2.
    // But Let do this trick. temp2 = (temp2 + 1) - temp1.
    // temp2 == 0 <-> temp2 == temp1 - 1 or temp2 < temp1 - 1
    // from 1. case temp1 > temp1 from 2. case temp1 > temp2 + 1 -> temp1 > temp2
    asm_inc(temp2);
    asm_sub(temp2, temp1);
    asm_jzero_label(temp2, label_true);
    asm_jump_label(label_false);

    label_manager.insert_label(label_true);

    temp1.unlock();
    temp2.unlock();

    // Create Branch
    return Conditional_branch(label_false);
}

Conditional_branch Assembler_generator::branch_lt(const Value& val1, const Value& val2) noexcept
{
    // a < b --> b > a
    return branch_gt(val2, val1);
}

Conditional_branch Assembler_generator::branch_geq(const Value& val1, const Value& val2) noexcept
{
    const std::string label_true = label_manager.create_label("COND_GEQ_TRUE");
    const std::string label_false = label_manager.create_label("COND_GEQ_FALSE");

    Register& temp1 = Architecture::get_free_register();
    temp1.lock();

    Register& temp2 = Architecture::get_free_register();
    temp2.lock();

    load(temp1, val1);
    load(temp2, val2);

    // temp2 = temp2 - temp1. temp2 == 0 <-> temp1 == temp2 or temp1 > temp1 -> temp1 >= temp2
    asm_sub(temp2, temp1);
    asm_jzero_label(temp2, label_true);
    asm_jump_label(label_false);

    label_manager.insert_label(label_true);

    temp1.unlock();
    temp2.unlock();

    // Create Branch
    return Conditional_branch(label_false);
}

Conditional_branch Assembler_generator::branch_leq(const Value& val1, const Value& val2) noexcept
{
    // a <= b --> b >= a
    return branch_geq(val2, val1);
}

void Assembler_generator::calculate_for_iterations(const Value& from, const Value& to) noexcept
{
    Register& retval = Architecture::get_retval_register();
    retval.lock();

    Register& temp = Architecture::get_free_register();
    temp.lock();

    load(retval, to);
    load(temp, from);

    asm_inc(retval);

    asm_sub(retval, temp);

    temp.unlock();

    // retval will be used in assignment, so dont unlock it now
}

void Assembler_generator::start_for_loop(const Loop_for& loop) noexcept
{
    label_manager.insert_label(loop.get_start_label());

    Register& temp = Architecture::get_free_register();
    temp.lock();

    load(temp, *loop.get_counter().get());
    asm_jzero_label(temp, loop.get_end_label());

    temp.unlock();
}

void Assembler_generator::do_for_loop(const Loop_for& loop) noexcept
{
    Register& temp = Architecture::get_free_register();
    temp.lock();

    // counter--
    load(temp, *loop.get_counter().get());
    asm_dec(temp);
    store(*loop.get_counter().get(), temp);

    // iterator++ or --
    load(temp, *loop.get_iterator().get());
    if (loop.get_type() == Loop::LOOP_TYPE_FOR_DO)
        asm_inc(temp);
    else
        asm_dec(temp);

    store(*loop.get_iterator().get(), temp);

    asm_jump_label(loop.get_start_label());

    label_manager.insert_label(loop.get_end_label());

    temp.unlock();
}

void Assembler_generator::start_while_loop(const Loop_while& loop) noexcept
{
    label_manager.insert_label(loop.get_start_label());
}

void Assembler_generator::do_while_loop(const Loop_while& loop) noexcept
{
    asm_jump_label(loop.get_start_label());
    label_manager.insert_label(loop.get_end_label());
}

void Assembler_generator::start_until_loop(const Loop_until& loop) noexcept
{
    label_manager.insert_label(loop.get_start_label());
}

void Assembler_generator::do_until_loop(const Loop_until& loop) noexcept
{
    const std::string label_end = label_manager.create_label("UNTIL_END");
    asm_jump_label(label_end);

    label_manager.insert_label(loop.get_end_label());
    asm_jump_label(loop.get_start_label());

    label_manager.insert_label(label_end);
}

void Assembler_generator::if_insert_jump_point(const Conditional_branch& branch) noexcept
{
    label_manager.insert_label(branch.get_label_false());
}

void Assembler_generator::if_skip_else(const Conditional_branch& branch) noexcept
{
    asm_jump_label(branch.get_label_false());
}