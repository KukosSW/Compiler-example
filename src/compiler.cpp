#include <cstdarg>
#include <cstdio>
#include <cinttypes>

#include <compiler.hpp>
#include <lvalue.hpp>

void Compiler::error(const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);

    vfprintf(stderr, fmt, args);

    va_end(args);

    exit(1);
}

void Compiler::assert_declaration(const std::string& name, uint64_t line)
{
    if (!get_var_manager().is_variable_declared(name))
        this->error("BLAD: Niezadeklarowana zmienna %s w linii %" PRIu64 "\n", name.c_str(), line);
}

void Compiler::assert_redeclaration(const std::string& name, uint64_t line)
{
    if (get_var_manager().is_variable_declared(name))
        this->error("BLAD: Redeklaracja zmiennej %s w linii %" PRIu64 "\n", name.c_str(), line);
}

void Compiler::assert_usage(const std::string& name, Value::valtype_t type, uint64_t line)
{
    Lvalue* lval = get_var_manager().get_variable(name).get();
    if (lval->get_type() != type)
        this->error("BLAD: Nieprawidlowe uzycie zmiennej %s w linii %" PRIu64 "\n", name.c_str(), line);
}

void Compiler::assert_initalization(Value *val, uint64_t line)
{
    // check if variable has been inited, Rval and Arrays are always inited
    if (val->get_type() == Value::VALTYPE_LVALUE_VAR)
    {
        Lvalue* lval = get_var_manager().get_variable(dynamic_cast<Lvalue*>(val)->get_name()).get();
        if (!lval->is_init())
            this->error("BLAD: Uzycie zmiennej %s bez inicjalizacji w linii %" PRIu64 "\n", lval->get_name().c_str(), line);
    }
}

void Compiler::assert_mutuable(Value *val, uint64_t line)
{
    // check if variable can be modyfied, Rval and Arrays cannot be by gramma rules, so there is no need to check it here
    if (val->get_type() == Value::VALTYPE_LVALUE_VAR)
    {
        Lvalue* lval = get_var_manager().get_variable(dynamic_cast<Lvalue*>(val)->get_name()).get();
        if (!lval->is_mutuable())
            this->error("BLAD: Proba nadpisania zmiennej %s typu const w linii %" PRIu64 "\n", lval->get_name().c_str(), line);
    }
}

void Compiler::init_variable(Value* val)
{
    if (val->get_type() == Value::VALTYPE_LVALUE_VAR)
    {
        Lvalue* lval = get_var_manager().get_variable(dynamic_cast<Lvalue*>(val)->get_name()).get();
        lval->set_init();
    }
}
