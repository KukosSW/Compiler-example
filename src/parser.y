%{
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdarg>
#include <cinttypes>

#include <compiler.hpp>
#include <value.hpp>
#include <rvalue.hpp>
#include <lvalue.hpp>
#include <lvalue-var.hpp>
#include <lvalue-array.hpp>

// C like functions for YACC / FLEX
static void yyerror(const char *msg);
static int yyparse(void);
int yylex(void);

extern FILE *yyin;

#ifdef YAC_DEBUG
#define pr_dbg(...) fprintf(stderr, __VA_ARGS__)
#else
#define pr_dbg(...)
#endif

/* Global variables */
static Compiler compiler;

%}

%code requires
{
    #include <cstdint>
    #include <cstring>

    #include <compiler.hpp>
    #include <value.hpp>
    #include <rvalue.hpp>
    #include <lvalue.hpp>
    #include <lvalue-var.hpp>
    #include <lvalue-array.hpp>
    #include <loop.hpp>
    #include <loop-for.hpp>

    typedef struct Parser_token
    {
        uint64_t val;
        uint64_t line;
        std::string* str; // Yacc needs pointer instead of class
    } Parser_token;

}

%union
{
    Parser_token ptoken;
    Value* value; // abstract class
}

%token	YY_EQ YY_NE YY_LT YY_GT YY_LE YY_GE
%token 	YY_ADD YY_SUB YY_DIV YY_MOD YY_MUL
%token	YY_ASSIGN
%token	YY_DECLARE_VAR YY_BEGIN YY_END
%token	YY_READ YY_WRITE
%token	YY_FOR YY_FROM YY_TO YY_DOWNTO YY_ENDFOR
%token	YY_WHILE YY_DO YY_ENDWHILE
%token  YY_REPEAT YY_UNTIL
%token	YY_IF YY_THEN YY_ELSE YY_ENDIF
%token	YY_VARIABLE YY_NUM
%token	YY_L_BRACKET YY_R_BRACKET YY_ARRAY_RANGE
%token	YY_ERROR
%token	YY_SEMICOLON YY_COMMA


%type <ptoken>	YY_EQ YY_NE YY_LT YY_GT YY_LE YY_GE
%type <ptoken> 	YY_ADD YY_SUB YY_DIV YY_MOD YY_MUL
%type <ptoken>	YY_ASSIGN
%type <ptoken>	YY_DECLARE_VAR YY_BEGIN YY_END
%type <ptoken>	YY_READ YY_WRITE
%type <ptoken>	YY_FOR YY_FROM YY_TO YY_DOWNTO YY_ENDFOR
%type <ptoken>	YY_WHILE YY_DO YY_ENDWHILE
%type <ptoken>  YY_REPEAT YY_UNTIL
%type <ptoken>	YY_IF YY_THEN YY_ELSE YY_ENDIF
%type <ptoken>	YY_VARIABLE YY_NUM
%type <ptoken>	YY_L_BRACKET YY_R_BRACKET YY_ARRAY_RANGE
%type <ptoken>	YY_ERROR
%type <ptoken>	YY_SEMICOLON YY_COMMA

%type <value>  lvalue rvalue value

%%

program:
    YY_DECLARE_VAR vdeclar YY_BEGIN commands YY_END
    {
        // we go here when whole program had been compiled
        // so we can finish here
        compiler.get_asm_generator().finish_program();
    }
    | YY_BEGIN commands YY_END
    {
        // we go here when whole program had been compiled
        // so we can finish here
        compiler.get_asm_generator().finish_program();
    }
;

value:
    lvalue
    {

    }
    | rvalue
    {

    }
;

rvalue:
    YY_NUM
    {
        // push forward numeric value as Rvalue
        $$ = new Rvalue($1.val);
    }
;

lvalue:
    YY_VARIABLE
    {
        compiler.assert_declaration(*($1.str), $1.line);
        compiler.assert_usage(*($1.str), Value::VALTYPE_LVALUE_VAR, $1.line);

        Lvalue_var* var = dynamic_cast<Lvalue_var*>(compiler.get_var_manager().get_variable(*($1.str)).get());
        $$ = new Lvalue_var(*var);

        delete $1.str;
    }
    | YY_VARIABLE YY_L_BRACKET YY_VARIABLE YY_R_BRACKET
    {
        compiler.assert_declaration(*($1.str), $1.line);
        compiler.assert_declaration(*($3.str), $3.line);

        compiler.assert_usage(*($1.str), Value::VALTYPE_LVALUE_ARRAY, $1.line);
        compiler.assert_usage(*($3.str), Value::VALTYPE_LVALUE_VAR, $3.line);

        // token t(a) is created after a. So a should always be init here
        compiler.assert_initalization(compiler.get_var_manager().get_variable(*($3.str)).get(), $1.line);

        Lvalue_array* array = dynamic_cast<Lvalue_array*>(compiler.get_var_manager().get_variable(*($1.str)).get());
        auto var = compiler.get_var_manager().get_variable(*($3.str));

        Lvalue_array* forward = new Lvalue_array(*array);
        forward->set_access_element(var);
        $$ = forward;

        delete $1.str;
        delete $3.str;
    }
    | YY_VARIABLE YY_L_BRACKET YY_NUM YY_R_BRACKET
    {
        compiler.assert_declaration(*($1.str), $1.line);
        compiler.assert_usage(*($1.str), Value::VALTYPE_LVALUE_ARRAY, $1.line);

        Lvalue_array* array = dynamic_cast<Lvalue_array*>(compiler.get_var_manager().get_variable(*($1.str)).get());
        auto val = std::shared_ptr<Value>(new Rvalue($3.val));

        Lvalue_array* forward =  new Lvalue_array(*array);
        forward->set_access_element(val);
        $$ = forward;

        delete $1.str;
    }
;

vdeclar:
    vdeclar YY_COMMA YY_VARIABLE
    {
        pr_dbg("Next Variable declaration: %s\n", $3.str->c_str());

        compiler.assert_redeclaration(*($3.str), $3.line);

        Lvalue* var = new Lvalue_var(*($3.str), true);
        compiler.get_var_manager().declare_variable(var);

        delete $3.str;
    }
    | vdeclar YY_COMMA YY_VARIABLE YY_L_BRACKET YY_NUM YY_ARRAY_RANGE YY_NUM YY_R_BRACKET
    {
        pr_dbg("Next Array declaration: %s\n", $3.str->c_str());

        compiler.assert_redeclaration(*($3.str), $3.line);
        if ($5.val > $7.val)
            compiler.error("BLAD: Zly zakres tablicy %s w linii %" PRIu64 "\n", $3.str->c_str(), $3.line);

        Lvalue* var = new Lvalue_array(*($3.str), $5.val, $7.val - $5.val + 1);
        compiler.get_var_manager().declare_variable(var);

        delete $3.str;
    }
    | YY_VARIABLE
    {
        pr_dbg("First Variable declaration: %s\n", $1.str->c_str());

        compiler.assert_redeclaration(*($1.str), $1.line);

        Lvalue* var = new Lvalue_var(*($1.str), true);
        compiler.get_var_manager().declare_variable(var);

        delete $1.str;
    }
    | YY_VARIABLE YY_L_BRACKET YY_NUM YY_ARRAY_RANGE YY_NUM YY_R_BRACKET
    {
        pr_dbg("First Array declaration: %s\n", $1.str->c_str());

        compiler.assert_redeclaration(*($1.str), $1.line);

        if ($3.val > $5.val)
            compiler.error("BLAD: Zly zakres tablicy %s w linii %" PRIu64 "\n", $1.str->c_str(), $1.line);

        Lvalue* var = new Lvalue_array(*($1.str), $3.val, $5.val - $3.val + 1);
        compiler.get_var_manager().declare_variable(var);

        delete $1.str;
    }
;

commands:
    commands command
    {

    }
    | command
    {

    }

;

command:
    lvalue YY_ASSIGN expr YY_SEMICOLON
    {
        pr_dbg("Assigment\n");

        compiler.assert_mutuable($1, $2.line);

        // For sure we have Lvalue (var or array)
        Lvalue* lval = dynamic_cast<Lvalue*>($1);

        // in RetVal Register we have value from expression, so just store it
        Register& retval = Architecture::get_retval_register();
        compiler.get_asm_generator().store(*lval, retval);

        retval.unlock();

        // after assigment set init flag
        compiler.init_variable($1);

        delete $1;
    }
    | YY_READ lvalue YY_SEMICOLON
    {
        pr_dbg("Read\n");

        compiler.assert_mutuable($2, $1.line);

        // For sure we have Lvalue (var or array)
        Lvalue* lval = dynamic_cast<Lvalue*>($2);

        compiler.get_asm_generator().read(*lval);

        // after assigment set init flag
        compiler.init_variable($2);

        delete $2;
    }
    | YY_WRITE value YY_SEMICOLON
    {
        pr_dbg("Write\n");

       compiler.assert_initalization($2, $1.line);

       compiler.get_asm_generator().write(*$2);

        // Pointer to temporary Value is no needed anymore
        delete $2;
    }
    | fordeclar forend
    {
        pr_dbg("For loop\n");
    }
    | whiledeclar whileend
    {
        pr_dbg("While loop\n");
    }
    | repeatdeclar repeatend
    {
        pr_dbg("Repeat until loop\n");
    }
    | ifcond ifelse ifend_after_else
    {
        pr_dbg("If then else end\n");
    }
    | ifcond ifend
    {
        pr_dbg("If then end\n");
    }
;

fordeclar:
    YY_FOR YY_VARIABLE YY_FROM value YY_TO value YY_DO
    {
        pr_dbg("For DO\n");

        compiler.assert_initalization($4, $1.line);
        compiler.assert_initalization($6, $1.line);

        compiler.assert_redeclaration(*($2.str), $1.line);

        // Alloc iterator as a const variable
        Lvalue* iterator = new Lvalue_var(*($2.str), false);
        compiler.get_var_manager().declare_variable(iterator);

        // iterator is always inited
        compiler.init_variable(iterator);

        // Alloc loop counter as const variable. we add _ to the name so this name will be incorrect according to gramma
        // which is safe, because nobody declare this variable as a normal variable in code

        Lvalue* counter = new Lvalue_var("_" + *($2.str), false);
        compiler.get_var_manager().declare_variable(counter);

        // Create label to jump from and jump to
        const std::string label_start = compiler.get_asm_generator().get_label_manager().create_label("FOR_START");
        const std::string label_end = compiler.get_asm_generator().get_label_manager().create_label("FOR_END");

        Loop* loop = new Loop_for(compiler.get_var_manager().get_variable(iterator->get_name()),
                                  compiler.get_var_manager().get_variable(counter->get_name()),
                                  label_start,
                                  label_end,
                                  Loop::LOOP_TYPE_FOR_DO);

        compiler.get_loop_manager().add_loop_to_stack(loop);

        // Calculate number of iterations. Counter := (TO + 1) - FROM
        compiler.get_asm_generator().calculate_for_iterations(*$4, *$6);

        Register& retval = Architecture::get_retval_register();
        compiler.get_asm_generator().store(*counter, retval);

        // Set iterator
        compiler.get_asm_generator().load(retval, *$4);
        compiler.get_asm_generator().store(*iterator, retval);

        retval.unlock();

        compiler.get_asm_generator().start_for_loop(*dynamic_cast<Loop_for*>(loop));

        delete $2.str;

        // Pointer to temporary Value is no needed anymore
        delete $4;
        delete $6;
    }
    | YY_FOR YY_VARIABLE YY_FROM value YY_DOWNTO value YY_DO
    {
        compiler.assert_initalization($4, $1.line);
        compiler.assert_initalization($6, $1.line);

        compiler.assert_redeclaration(*($2.str), $1.line);

        // Alloc iterator as a const variable
        Lvalue* iterator = new Lvalue_var(*($2.str), false);
        compiler.get_var_manager().declare_variable(iterator);

        // iterator is always inited
        compiler.init_variable(iterator);

        // Alloc loop counter as const variable. we add _ to the name so this name will be incorrect according to gramma
        // which is safe, because nobody declare this variable as a normal variable in code

        Lvalue* counter = new Lvalue_var("_" + *($2.str), false);
        compiler.get_var_manager().declare_variable(counter);

        // Create label to jump from and jump to
        const std::string label_start = compiler.get_asm_generator().get_label_manager().create_label("FOR_START");
        const std::string label_end = compiler.get_asm_generator().get_label_manager().create_label("FOR_END");

        Loop* loop = new Loop_for(compiler.get_var_manager().get_variable(iterator->get_name()),
                                  compiler.get_var_manager().get_variable(counter->get_name()),
                                  label_start,
                                  label_end,
                                  Loop::LOOP_TYPE_FOR_DOWNTO);

        compiler.get_loop_manager().add_loop_to_stack(loop);

        // Calculate number of iterations. Counter := (FROM + 1) - TO
        compiler.get_asm_generator().calculate_for_iterations(*$6, *$4);

        Register& retval = Architecture::get_retval_register();
        compiler.get_asm_generator().store(*counter, retval);

        // Set iterator
        compiler.get_asm_generator().load(retval, *$4);
        compiler.get_asm_generator().store(*iterator, retval);

        retval.unlock();

        compiler.get_asm_generator().start_for_loop(*dynamic_cast<Loop_for*>(loop));

        delete $2.str;

        // Pointer to temporary Value is no needed anymore
        delete $4;
        delete $6;
    }
;

forend:
    commands YY_ENDFOR
    {
        Loop_for* loop = dynamic_cast<Loop_for*>(compiler.get_loop_manager().get_loop_from_stack());

        compiler.get_asm_generator().do_for_loop(*loop);

        compiler.get_var_manager().undeclare_variable(loop->get_iterator().get());
        compiler.get_var_manager().undeclare_variable(loop->get_counter().get());

        delete loop;
    }
;

whiledeclar:
    prewhile cond YY_DO
    {
        // now we can get false label name. False label from cond will be our end label
        Conditional_branch branch = compiler.get_branch_manager().get_branch_from_stack();
        compiler.get_loop_manager().get_loop_top_stack()->set_end_label(branch.get_label_false());
    }
;

prewhile:
    YY_WHILE
    {
        const std::string label_start = compiler.get_asm_generator().get_label_manager().create_label("WHILE_START");

        Loop* loop = new Loop_while(label_start, std::string("to_be_defined_later"));
        compiler.get_loop_manager().add_loop_to_stack(loop);

        compiler.get_asm_generator().start_while_loop(*dynamic_cast<Loop_while*>(loop));
    }
;

whileend:
    commands YY_ENDWHILE
    {
        Loop* loop = compiler.get_loop_manager().get_loop_from_stack();

        compiler.get_asm_generator().do_while_loop(*dynamic_cast<Loop_while*>(loop));

        delete loop;
    }
;

repeatdeclar:
    YY_REPEAT
    {
        const std::string label_start = compiler.get_asm_generator().get_label_manager().create_label("REPEAT_START");

        Loop* loop = new Loop_until(label_start, std::string("to_be_defined_later"));
        compiler.get_loop_manager().add_loop_to_stack(loop);

        compiler.get_asm_generator().start_until_loop(*dynamic_cast<Loop_until*>(loop));
    }
;

repeatend:
    commands YY_UNTIL cond YY_SEMICOLON
    {
        Loop* loop = compiler.get_loop_manager().get_loop_from_stack();

        // now we can get false label name. False label from cond will be our end label
        Conditional_branch branch = compiler.get_branch_manager().get_branch_from_stack();
        loop->set_end_label(branch.get_label_false());

        compiler.get_asm_generator().do_until_loop(*dynamic_cast<Loop_until*>(loop));

        delete loop;
    }
;

ifcond:
    YY_IF cond YY_THEN
    {

    }
;

ifend:
    commands YY_ENDIF
    {
        Conditional_branch branch = compiler.get_branch_manager().get_branch_from_stack();

        compiler.get_asm_generator().if_insert_jump_point(branch);
    }
;

ifelse:
    commands YY_ELSE
    {
        // we need to set point to jump from cond false label,
        // also we need to create jump branch to ommit else code after if
        Conditional_branch jump_from_if = Conditional_branch(compiler.get_asm_generator().get_label_manager().create_label("jump_after_if"));
        compiler.get_asm_generator().if_skip_else(jump_from_if);

        Conditional_branch branch = compiler.get_branch_manager().get_branch_from_stack();

        compiler.get_asm_generator().if_insert_jump_point(branch);

        // now we need to add jump_from_if to stack, we wil pop it after else commands to create jump to point
        compiler.get_branch_manager().add_branch(jump_from_if);
    }
;


ifend_after_else:
    commands YY_ENDIF
    {
        //after else branch jump is a point to jump from if code to skip else code
        Conditional_branch branch = compiler.get_branch_manager().get_branch_from_stack();

        compiler.get_asm_generator().if_insert_jump_point(branch);
    }
;

expr:
    value
    {
        pr_dbg("Value expression\n");

        compiler.assert_initalization($1, 0);

        // load value into retVal registe. Assign will read expr from retVal register
        Register& retval = Architecture::get_retval_register();
        retval.lock();

        compiler.get_asm_generator().load(retval, *$1);

        // Pointer to temporary Value is no needed anymore
        delete $1;
    }
    | value YY_ADD value
    {
        pr_dbg("ADD\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        compiler.get_asm_generator().add(*$1, *$3);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
    | value YY_SUB value
    {
        pr_dbg("SUB\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        compiler.get_asm_generator().sub(*$1, *$3);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
    | value YY_MUL value
    {
        pr_dbg("MUL\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        compiler.get_asm_generator().mul(*$1, *$3);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
    | value YY_DIV value
    {
        pr_dbg("DIV\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        compiler.get_asm_generator().div(*$1, *$3);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
    | value YY_MOD value
    {
        pr_dbg("MOD\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        compiler.get_asm_generator().mod(*$1, *$3);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
;

cond:
    value YY_EQ value
    {
        pr_dbg("EQ\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        Conditional_branch branch = compiler.get_asm_generator().branch_eq(*$1, *$3);
        compiler.get_branch_manager().add_branch(branch);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
    | value YY_NE value
    {
        pr_dbg("NE\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        Conditional_branch branch = compiler.get_asm_generator().branch_neq(*$1, *$3);
        compiler.get_branch_manager().add_branch(branch);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
    | value YY_LT value
    {
        pr_dbg("LT\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        Conditional_branch branch = compiler.get_asm_generator().branch_lt(*$1, *$3);
        compiler.get_branch_manager().add_branch(branch);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
    | value YY_LE value
    {
        pr_dbg("LE\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        Conditional_branch branch = compiler.get_asm_generator().branch_leq(*$1, *$3);
        compiler.get_branch_manager().add_branch(branch);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
    | value YY_GT value
    {
        pr_dbg("GT\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        Conditional_branch branch = compiler.get_asm_generator().branch_gt(*$1, *$3);
        compiler.get_branch_manager().add_branch(branch);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
    | value YY_GE value
    {
        pr_dbg("GE\n");

        compiler.assert_initalization($1, $2.line);
        compiler.assert_initalization($3, $2.line);

        Conditional_branch branch = compiler.get_asm_generator().branch_geq(*$1, *$3);
        compiler.get_branch_manager().add_branch(branch);

        // Pointer to temporary Value is no needed anymore
        delete $1;
        delete $3;
    }
;

%%

static void yyerror(const char* msg)
{
    std::cerr << "BLAD: " << msg << " w linii " << yylval.ptoken.line << std::endl;
}

int compile(const char* in_file, const char* out_file)
{
    yyin = fopen(in_file, "r");
    const int ret = yyparse();
    fclose(yyin);

    std::ofstream outstream;
    outstream.open(out_file);

    compiler.get_asm_generator().finish_code_generation();
    const std::string code = compiler.get_asm_generator().get_generated_code();

    outstream << code << std::endl;

    return ret;
}