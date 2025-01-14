#include "headers/Sender.h"
#include "headers/Checksum.h"
#include "headers/Config.h"
#include "headers/FrameTypes.h"

extern bool isFirstPeer;

Sender::Sender(GeneralDriver &gdrv, std::string &Datei) : gdrv(gdrv), Datei(Datei) {
    Package_counter = 0;
    Packege_teilung();
}

void Sender::bitssendung(std::string Package) {
    // uint8_t outputMask = isFirstPeer ? 0x0F : 0xF0;
    // sendWithLock(outputMask, false); this is useless and leads to bugs when using RPi!
    gdrv.sendWithLock(FLAG, true);
    gdrv.sendWithLock(DATA, true);

    for (char zeichen : Package) {
        gdrv.sendWithLock(zeichen >> 4, true);
        gdrv.sendWithLock(zeichen, true);
    }

    gdrv.sendWithLock(static_cast<uint8_t>(Package_counter), true);
    gdrv.sendWithLock(0x0, true); //clear last bits
    // std::cerr << "Reset PORTA after sending." << std::endl;
    if (Package_counter == 15) {
        Package_counter = 0;
    } else {
        Package_counter++;
    }
}

void Sender::Packege_teilung() {
    const size_t chunkSize = PACKAGE_SIZE;
    Packages.reserve((Datei.size() + chunkSize - 1) / chunkSize);
    static const std::string nullPadding(chunkSize, '0');  // Pre-create padding

    for (size_t i = 0; i < Datei.size(); i += chunkSize) {
        std::string chunk = Datei.substr(i, chunkSize);
        if (chunk.size() < chunkSize) {
            chunk.append(nullPadding, 0, chunkSize - chunk.size());
        }
        // std::move reallocates memory (не надо копировать и вставлять чанк)
        Packages.push_back(std::move(chunk));
        // std::cerr << chunk << std::endl;
    }
}

void Sender::Package_sendung() {
    sync();
    gdrv.sendWithLock(0x00, true);

    for (int i = 0; i < Packages.size() - 1; i++) {
        bitssendung(completePackage(Packages[i]));
    }

    last_package();
    gdrv.setEOT(true);
    gdrv.setEOT(true);
    gdrv.setEOT(true);
}

void Sender::sync() {
    int sync_counter = 0;
    const int REQUIRED_SYNCS = 4;
    const int MAX_ATTEMPTS = 10000;  // Prevent infinite loops
    int attempts = 0;
    
    // First phase: Send SYNC until we get enough SYNC responses
    while(sync_counter < REQUIRED_SYNCS && attempts < MAX_ATTEMPTS) {
        gdrv.sendWithLock(SYNC, true);
        uint8_t answer = gdrv.readWithLock();
        
        if (answer == SYNC) {
            sync_counter++;
            std::cerr << "SYNC ACK RECEIVED! (" << sync_counter << "/" << REQUIRED_SYNCS << ")\n";
        } else {
            // Reset counter if we get an unexpected response
            sync_counter = 0;
            std::cerr << "Unexpected response, resetting sync\n";
        }
        attempts++;
    }
    
    if (attempts >= MAX_ATTEMPTS) {
        throw std::runtime_error("Failed to synchronize after maximum attempts");
    }
    
    bool ready = false;
    attempts = 0;
    
    while (!ready && attempts < MAX_ATTEMPTS) {
        gdrv.sendWithLock(READY, true);
        uint8_t answer = gdrv.readWithLock();
        
        if (answer == READY) {
            ready = true;
            std::cerr << "READY signal confirmed!\n";
        } 
        attempts++;
    }

    std::cerr << "Synced succesfully!";
    
    if (!ready) {
        throw std::runtime_error("Failed to confirm ready state");
    }
    
    // Small delay to ensure both sides are ready
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}


void Sender::last_package() {
    bitssendung(completePackage(Packages[Packages.size() - 1]));
    gdrv.sendWithLock(EOT, true);
}

std::string Sender::completePackage(std::string package) {
    uint16_t checksum = Checksum::crc16(package);
    // Convert checksum to two bytes
    char highByte = (checksum >> 8);
    char lowByte = checksum;
    // Append checksum bytes to package
    package += highByte;
    package += lowByte;
    return package;
}
