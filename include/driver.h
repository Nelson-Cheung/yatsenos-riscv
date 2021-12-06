#ifndef DRIVER_H
#define DRIVER_H

#include "uart.h"

class Driver
{
public:
    UART uart;
    void initialize();
};

#endif