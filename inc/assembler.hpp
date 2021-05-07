#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <string>

class Assembler
{
public:
    typedef enum
    {
        ASM_INSTRUCTION_GET,
        ASM_INSTRUCTION_PUT,
        ASM_INSTRUCTION_LOAD,
        ASM_INSTRUCTION_STORE,
        ASM_INSTRUCTION_ADD,
        ASM_INSTRUCTION_SUB,
        ASM_INSTRUCTION_RESET,
        ASM_INSTRUCTION_INC,
        ASM_INSTRUCTION_DEC,
        ASM_INSTRUCTION_SHR,
        ASM_INSTRUCTION_SHL,
        ASM_INSTRUCTION_JUMP,
        ASM_INSTRUCTION_JZERO,
        ASM_INSTRUCTION_JODD,
        ASM_INSTRUCTION_HALT,
        ASM_INSTRUCTION_NUMBER_OF_INSTRUCTIONS
    } asm_instruction_type_t;

    static const std::string instructions_str[ASM_INSTRUCTION_NUMBER_OF_INSTRUCTIONS];

    static const std::string& instruction_get() { return instructions_str[ASM_INSTRUCTION_GET]; }
    static const std::string& instruction_put() { return instructions_str[ASM_INSTRUCTION_PUT]; }
    static const std::string& instruction_load() { return instructions_str[ASM_INSTRUCTION_LOAD]; }
    static const std::string& instruction_store() { return instructions_str[ASM_INSTRUCTION_STORE]; }
    static const std::string& instruction_add() { return instructions_str[ASM_INSTRUCTION_ADD]; }
    static const std::string& instruction_sub() { return instructions_str[ASM_INSTRUCTION_SUB]; }
    static const std::string& instruction_reset() { return instructions_str[ASM_INSTRUCTION_RESET]; }
    static const std::string& instruction_inc() { return instructions_str[ASM_INSTRUCTION_INC]; }
    static const std::string& instruction_dec() { return instructions_str[ASM_INSTRUCTION_DEC]; }
    static const std::string& instruction_shr() { return instructions_str[ASM_INSTRUCTION_SHR]; }
    static const std::string& instruction_shl() { return instructions_str[ASM_INSTRUCTION_SHL]; }
    static const std::string& instruction_jump() { return instructions_str[ASM_INSTRUCTION_JUMP]; }
    static const std::string& instruction_jzero() { return instructions_str[ASM_INSTRUCTION_JZERO]; }
    static const std::string& instruction_jodd() { return instructions_str[ASM_INSTRUCTION_JODD]; }
    static const std::string& instruction_halt() { return instructions_str[ASM_INSTRUCTION_HALT]; }
};

#endif