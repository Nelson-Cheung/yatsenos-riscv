#ifndef DRIVER_H
#define DRIVER_H

#include "uart.h"
#include "clint.h"

class Driver
{
public:
    UART uart;
    CLINT clint;

public:
    void initialize();
};

#endif