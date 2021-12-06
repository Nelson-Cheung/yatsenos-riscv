#include "clint.h"
#include "rv64.h"

void CLINT::set_interrupt_handler(unsigned long address)
{
    write_stvec(address);
}