#include <assembler.hpp>

const std::string Assembler::instructions_str[ASM_INSTRUCTION_NUMBER_OF_INSTRUCTIONS] =
{
    std::string("GET"),
    std::string("PUT"),
    std::string("LOAD"),
    std::string("STORE"),
    std::string("ADD"),
    std::string("SUB"),
    std::string("RESET"),
    std::string("INC"),
    std::string("DEC"),
    std::string("SHR"),
    std::string("SHL"),
    std::string("JUMP"),
    std::string("JZERO"),
    std::string("JODD"),
    std::string("HALT")
};