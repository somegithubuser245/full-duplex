//
// Created by Nanik on 12/17/2024.
//

#ifndef V7_FRAMETYPES_H
#define V7_FRAMETYPES_H

enum class FrameType {
    DATA = 0x0,
    ACK = 0x1,
    NACK = 0x2,
    LAST = 0xE,
};

#endif //V7_FRAMETYPES_H
