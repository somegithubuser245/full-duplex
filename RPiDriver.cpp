#include "RPiDriver.h"
#include <iostream>
#include <bitset>

RPiDriver::RPiDriver(bool firstPeer) {
    if (gpioInitialise() < 0) {
        throw std::runtime_error("Failed to initialize pigpio");
    }
    std::cerr << "Initializing GPIO pins..." << std::endl;
    firstPeer = firstPeer;
    // For first peer mode:
    // GPIO20-23 are inputs
    // GPIO24-27 are outputs
    int pinStartRead, pinStartWrite;

    if (firstPeer) {
        shiftRead = 16;
        shiftWrite = 24;
        pinStartRead = 20;
        pinStartWrite = 24;

    } else {
        shiftRead = 20;
        shiftWrite = 24;
        pinStartRead = 24;
        pinStartWrite = 20;
    }

    for(int i = pinStartRead; i < pinStartRead + 4; i++) {
        gpioSetMode(i, PI_INPUT);
        gpioSetPullUpDown(i, PI_PUD_DOWN);
    }

    for(int i = pinStartWrite; i < pinStartWrite + 4; i++) {
        gpioSetMode(i, PI_OUTPUT);
        gpioWrite(i, 0);
    }
}

    

uint8_t RPiDriver::getRegister(void*) {
    uint32_t bank = gpioRead_Bits_0_31();
    uint8_t value = ((bank >> shiftRead));
    
    
    std::cerr << "Raw bank read: 0b" << std::bitset<32>(bank) << std::endl;
    std::cerr << "After mask and shift: 0b" << std::bitset<8>(value) << std::endl;
    
    return value;
}

void RPiDriver::setRegister(void*, uint8_t value) {
    // Clear output pins (GPIO24-27)
    gpioWrite_Bits_0_31_Clear(0xF << shiftWrite);
    
    // Write lower 4 bits of value to GPIO24-27

    uint32_t shifted_value = ((uint32_t)(value)) << shiftWrite;  // Shift to position 24-27

    
    std::cerr << "Writing value: 0b" << std::bitset<8>(value) << std::endl;
    std::cerr << "Output to pins: 0b" << std::bitset<32>(shifted_value) << std::endl;
    
    gpioWrite_Bits_0_31_Set(shifted_value);
    // gpioWrite_Bits_0_31_Clear(0xF << shiftWrite);
}

RPiDriver::~RPiDriver() {
    for(int i = 20; i <= 27; i++) {
        gpioSetMode(i, PI_INPUT);
        gpioSetPullUpDown(i, PI_PUD_DOWN);
    }
    gpioTerminate();
}