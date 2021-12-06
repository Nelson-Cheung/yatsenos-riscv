#include "interrupt.h"
#include "rv64.h"

extern "C" long supervisor_interrupt_handler()
{
    unsigned long status = read_sstatus();
    unsigned long cause = read_scause();
}