#ifndef COMMANDIMPL_H
#define COMMANDIMPL_H
#include "Command.h"

namespace PietCommand
{
    Command Noop();
    Command Push();
    Command Pop();
    Command Add();
    Command Subtract();
    Command Multiply();
    Command Divide();
    Command Mod();
    Command Not();
    Command Greater();
    Command Pointer();
    Command Switch();
    Command Duplicate();
    Command Roll();
    Command In();
    Command Out();
}
#endif