#include "timer.h"
#include "clint.h"

unsigned long TIMER::read_mtimecmp()
{
    return *(unsigned long *)(CLINT_BASE + MTIMECMP_BASE);
}

unsigned long TIMER::read_mtime()
{
    return *(unsigned long *)(CLINT_BASE + MTIME_BASE);
}

void TIMER::write_mtimecmp(unsigned long val)
{
    unsigned long *mtimecmp = (unsigned long *)(CLINT_BASE + MTIMECMP_BASE);
    *mtimecmp = val;
}

void TIMER::write_mtime(unsigned long val)
{
    unsigned long *mtime = (unsigned long *)(CLINT_BASE + MTIME_BASE);
    *mtime = val;
}