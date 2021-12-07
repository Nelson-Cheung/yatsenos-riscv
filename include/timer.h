#ifndef TIMER_H
#define TIMER_H

#define MTIMECMP_BASE 0x4000
#define MTIME_BASE 0xbff8

class TIMER
{
public:
    unsigned long read_mtimecmp();
    unsigned long read_mtime();
    void write_mtimecmp(unsigned long val);
    void write_mtime(unsigned long val);
};
#endif