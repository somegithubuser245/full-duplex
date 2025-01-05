#include "headers/receiver.h"
#include "headers/FrameTypes.h"

extern std::mutex b15f_mutex;
extern std::condition_variable cv;
extern bool isSenderActive;
extern bool isReceiverActive;
extern std::array<bool, 16> lostPackages; 

extern bool isFirstPeer;

std::string fullFrame= "";

Receiver::Receiver(RPiDriver &drv) : drv(drv) {
    fileFullyReceived = false;
    frameCounter = 0;
    lostPackages.fill(false);
}

uint8_t Receiver::readBits() {
    std::unique_lock<std::mutex> lock(b15f_mutex);
    cv.wait(lock, [] { return isReceiverActive; });
    uint8_t bits;

    if (isFirstPeer) {
        bits = drv.getRegister(nullptr) >> 4;
    } else {
        bits = drv.getRegister(nullptr) & 0x0F;
    }

    std::cerr << "Read bits: " << std::bitset<4>(bits) << std::endl;  // keep for debugging

    isReceiverActive = false;
    isSenderActive = true;
    cv.notify_all();

    return bits;
}


void Receiver::monitor() {
    while (!fileFullyReceived) {
        try {
            handleFrame();
        } catch (const std::exception &e) {
            std::cerr << "Error in Receiver: " << e.what() << std::endl;
        }
    }
}

void Receiver::handleFrame() {
    uint8_t flag = readBits();

    if (flag == 0x0F) {
        readFrame();
    } else {
        // std::cerr << "Unexpected data\n";
    }

    
}

void Receiver::handleAckFrame() {
    std::cerr << "Acknowledgment frame incoming" << std::endl;
}

void Receiver::handleNackFrame() {
    std::cerr << "Nacknowledgment frame incoming" << std::endl;
}

void Receiver::readFrame()
{   
    const uint8_t type = readBits();

    switch(type) {
        case DATA_TYPE:
            std::cout << readDataFrame();
            break;
        case ACK_TYPE:
            handleAckFrame();
            break;
        case NACK_TYPE:
            handleNackFrame();
            break;
        default:
            // std::cerr << "Unknown type of frame!\n";
            break;
    }
}

std::string Receiver::readDataFrame() {
    std::string data;
    data.reserve(8);

    for(int i = 0; i < PACKAGE_SIZE; i++) {
        uint8_t upper = readBits();
        uint8_t lower = readBits();

        char c = (upper << 4) | lower;
        data.push_back(c);
    }

    uint16_t receivedChecksum = receiveChecksum();

    uint8_t packageNumber = readBits();

    uint16_t calculatedChecksum = crc16(data);

    if (calculatedChecksum != receivedChecksum) {
        std::cerr << "Checksum mismatch! Calculated: " << std::hex << calculatedChecksum
                  << ", Received: " << std::hex << receivedChecksum << std::endl; // Handle checksum error (e.g., request retransmission or discard the frame)
        lostPackages[packageNumber] = true;
    } else {
        std::cerr << "Checksum OK." << std::endl;
        completed_Frames[packageNumber]=data;
    }

    fullFrame += data;
    std::cerr<< "Receiver Package Number: " << static_cast<int>(packageNumber) << std::endl;
    std::cerr << "Received data: " << data << std::endl;
    std::cerr<< "Received Frame: "<< fullFrame <<std::endl;

    frameCounter++;

    if (lastChunk) {
        return checkZeros(data);
    }

    return data;
}

uint16_t Receiver::receiveChecksum() {
    uint8_t upperHigh = readBits();
    uint8_t upperLow = readBits();
    uint8_t checksumHigh = (upperHigh << 4) | upperLow;

    uint8_t lowerHigh = readBits();
    uint8_t lowerLow = readBits();
    uint8_t checksumLow = (lowerHigh << 4) | lowerLow;

    uint16_t receivedChecksum = (checksumHigh << 8) | checksumLow;

    return receivedChecksum;
}

uint16_t Receiver::crc16(const std::string& data) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < data.size(); ++i) {
        crc ^= static_cast<uint8_t>(data[i]) << 8;
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;  // Polynomial used in CRC16-CCITT
            else
                crc <<= 1;
        }
    }
    return crc;
}

std::string Receiver::crc16_to_string(uint16_t crc) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << crc;
    return ss.str();
}

std::string Receiver::checkZeros(std::string data) {
    // Remove all trailing zeros
    while (!data.empty() && data.back() == '0') {
        data.pop_back();
    }
    return data;
}

bool Receiver::checkFileReceived() {
    return false;
}

