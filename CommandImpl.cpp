#include "CommandImpl.h"

Command PietCommand::Noop()
{
    return Command( "noop", "" );
}


Command PietCommand::Push()
{
    return Command( "push", "Pushes the value of the colour block just exited on to the stack. Note that values of colour blocks are not automatically pushed on to the stack - this push operation must be explicitly carried out." );
}

Command PietCommand::Pop()
{
    return Command( "pop", "Pops the top value off the stack and discards it." );
}


Command PietCommand::Add()
{
    return Command( "add", "Pops the top two values off the stack, adds them, and pushes the result back on the stack." );
}

Command PietCommand::Subtract()
{
    return Command( "subtract", "Pops the top two values off the stack, subtracts the top value from the second top value, and pushes the result back on the stack." );
}

Command PietCommand::Multiply()
{
    return Command( "multiply", "Pops the top two values off the stack, multiplies them, and pushes the result back on the stack." );
}

Command PietCommand::Divide()
{
    return Command( "divide", "Pops the top two values off the stack, calculates the integer division of the second top value by the top value, and pushes the result back on the stack." );
}

Command PietCommand::Mod()
{
    return Command( "mod", "Pops the top two values off the stack, calculates the second top value modulo the top value, and pushes the result back on the stack." );
}

Command PietCommand::Not()
{
    return Command( "not", "Replaces the top value of the stack with 0 if it is non-zero, and 1 if it is zero." );
}

Command PietCommand::Greater()
{
    return Command( "greater", "Pops the top two values off the stack, and pushes 1 on to the stack if the second top value is greater than the top value, and pushes 0 if it is not greater." );
}

Command PietCommand::Pointer()
{
    return Command( "pointer", "Pops the top value off the stack and rotates the DP clockwise that many steps (anticlockwise if negative)." );
}

Command PietCommand::Switch()
{
    return Command( "switch", "Pops the top value off the stack and toggles the CC that many times." );
}

Command PietCommand::Duplicate()
{
    return Command( "duplicate", "Pushes a copy of the top value on the stack on to the stack." );
}

Command PietCommand::Roll()
{
    return Command( "roll", "Pops the top two values off the stack and \"rolls\" the remaining stack entries to a depth equal to the second value popped, by a number of rolls equal to the first value popped. A single roll to depth n is defined as burying the top value on the stack n deep and bringing all values above it up by 1 place. A negative number of rolls rolls in the opposite direction. A negative depth is an error and the command is ignored." );
}

Command PietCommand::In()
{
    return Command( "in", "Reads a value from STDIN as either a number or character, depending on the particular incarnation of this command and pushes it on to the stack." );
}

Command PietCommand::Out()
{
    return Command( "out", "Pops the top value off the stack and prints it to STDOUT as either a number or character, depending on the particular incarnation of this command." );
}

