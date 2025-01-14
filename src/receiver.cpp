#include "headers/receiver.h"
#include "headers/FrameTypes.h"
#include "headers/Checksum.h"

extern bool isFirstPeer;

std::string fullFrame= "";

Receiver::Receiver(GeneralDriver &gdrv) : gdrv(gdrv) {
    fileFullyReceived = false;
    frameCounter = 0;
}


void Receiver::monitor() {
    while (!checkFileReceived()) {
        try {
            handleFrame();
        } catch (const std::exception &e) {
            std::cerr << "Error in Receiver: " << e.what() << std::endl;
        }
    }
}

void Receiver::handleFrame() {
    uint8_t flag = gdrv.readWithLock();

    if (flag == 0x0F) {
        std::cerr << "FLAG HIT\n";
        readFrame();
    } else {
        std::cerr << "Unexpected data\n";
    }
}

void Receiver::readFrame()
{   
    const uint8_t type = gdrv.readWithLock();

    switch(type) {
        case DATA:
            std::cerr << "DATA TYPE HIT\n";
            std::cout << readDataFrame();
            break;
        default:
            // std::cerr << "Unknown type of frame!\n";
            break;
    }
}

std::string Receiver::readDataFrame() {
    std::cerr << "trying to read data!\n";
    std::string data;
    data.reserve(PACKAGE_SIZE);

    for(int i = 0; i < PACKAGE_SIZE; i++) {
        uint8_t upper = gdrv.readWithLock();
        uint8_t lower = gdrv.readWithLock();

        char c = (upper << 4) | lower;
        data.push_back(c);
    }

    uint16_t receivedChecksum = receiveChecksum();

    uint8_t packageNumber = gdrv.readWithLock();
    

    uint16_t calculatedChecksum = Checksum::crc16(data);

    if (calculatedChecksum != receivedChecksum) {
        std::cerr << "Checksum mismatch! Calculated: " << std::hex << calculatedChecksum
                  << ", Received: " << std::hex << receivedChecksum << std::endl;
    } else {
        std::cerr << "Checksum OK." << std::endl;
    }

    // fullFrame += data;
    std::cerr<< "Receiver Package Number: " << static_cast<int>(packageNumber) << std::endl;
    std::cerr << "Received data: " << data << std::endl;
    // std::cerr<< "Received Frame: "<< fullFrame <<std::endl;

    frameCounter++;

    if (lastChunk) {
        return checkZeros(data);
    }

    return data;
}

uint16_t Receiver::receiveChecksum() {
    uint8_t upperHigh = gdrv.readWithLock();
    uint8_t upperLow = gdrv.readWithLock();
    uint8_t checksumHigh = (upperHigh << 4) | upperLow;

    uint8_t lowerHigh = gdrv.readWithLock();
    uint8_t lowerLow = gdrv.readWithLock();
    uint8_t checksumLow = (lowerHigh << 4) | lowerLow;

    uint16_t receivedChecksum = (checksumHigh << 8) | checksumLow;

    return receivedChecksum;
}

std::string Receiver::checkZeros(std::string data) {
    // Remove all trailing zeros
    while (!data.empty() && data.back() == '0') {
        data.pop_back();
    }
    return data;
}

bool Receiver::checkFileReceived() {
    
    if (gdrv.readWithLock() == EOT) {
        if (gdrv.readWithLock() == EOT) {
            if (gdrv.readWithLock() == EOT) {
                gdrv.setEOT(true);
                std::cerr << "EOT FLAG HIT BRUH!!!\n";
                return true; 
            }
        }
        return false;
    };
    return false;
}

