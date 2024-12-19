#ifndef RPI_DRIVER_H
#define RPI_DRIVER_H

#include <pigpio.h>
#include <cstdint>  // for uint8_t, uint32_t

class RPiDriver {
private:
    static const uint32_t INPUT_MASK = 0xF00000;
    static const int INPUT_SHIFT = 20;
    static const uint32_t OUTPUT_MASK = 0xF000000;
    static const int OUTPUT_SHIFT = 20;
    int shiftRead, shiftWrite;
    bool firstPeer;

public:
    RPiDriver(bool firstPeer);
    uint8_t getRegister(void*);
    void setRegister(void*, uint8_t value);
    void setGPIO(bool firstPeer, bool input);
    ~RPiDriver();
};

#endif