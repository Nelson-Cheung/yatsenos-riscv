#ifndef DRIVER_H
#define DRIVER_H

#include "uart.h"
#include "clint.h"
#include "timer.h"

class Driver
{
public:
    UART uart;
    CLINT clint;
    TIMER timer;

public:
    void initialize();
};

#endif