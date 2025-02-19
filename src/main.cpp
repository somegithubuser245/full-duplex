#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <cstring>
#include "headers/Sender.h"
#include "headers/receiver.h"
#include "headers/Generaldriver.h"
using namespace std;

bool isFirstPeer = false;  // Default value

int main(int argc, char* argv[]) {
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            isFirstPeer = true;
        } else if (strcmp(argv[i], "-s") == 0) {
            isFirstPeer = false;
        }
    }

    // RPiDriver drv(isFirstPeer); // Singleton instance
    B15F &drv = B15F::getInstance();
    
    std::string text;
    if (argc > 1 && strcmp(argv[1], "-f") != 0 && strcmp(argv[1], "-s") != 0) {
        // If a filename is provided, read from the file
        std::string filename = argv[1];
        std::ifstream inputFile(filename, std::ios::binary);
        if (!inputFile) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return 1;
        }
        // Read the entire file into 'text'
        std::ostringstream oss;
        oss << inputFile.rdbuf();
        text = oss.str();
        std::cerr << "Read " << text.size() << " bytes from file: " << filename << std::endl;
    } else {
        // No filename provided, read from stdin
        std::ostringstream oss;
        oss << std::cin.rdbuf();
        text = oss.str();
        std::cerr << "Read " << text.size() << " bytes from stdin." << std::endl;
    }

    // Create Receiver and Sender objects with peer type
    GeneralDriver gdrv(isFirstPeer, drv);
    Receiver receiver(gdrv);
    Sender sender(gdrv, text);

    // Launch sender and receiver in separate threads
    thread receiverThread([&]() {
        receiver.monitor();
    });
    thread senderThread([&]() {
        sender.Package_sendung();
    });

    // Wait for both threads to finish
    senderThread.join();
    receiverThread.join();
    cerr << "Full-duplex communication complete!" << endl;
    return 0;
}