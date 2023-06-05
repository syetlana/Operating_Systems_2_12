#include <iostream>
#include <fstream>
#include <string>
#include "Windows.h"


int main(int argc, char* argv[]) {
    std::ofstream   binOutputStream;
    std::string     binFileName{ argv[1] };
    std::string     message;
    char            messageCharset[20]{    };
    const int       MESSAGE_LENGTH{ 20 };
    int             option{ 0 };
    int             proccessID{ std::stoi(argv[2]) };
    HANDLE          hReadAccessSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"Read Access Semaphore");
    HANDLE          hWriteAccessSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"Write Access Semaphore");
    HANDLE          hCurrentProcess = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"Current Process Owner2");
    HANDLE          senderStartEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, (LPCWSTR)("START PROCESS" + proccessID));

    SetEvent(senderStartEvent);
    std::cout << "You are currently in Sender.exe number " << proccessID << " process \n" << std::endl;

    while (option != 2) {
        WaitForSingleObject(hWriteAccessSemaphore, INFINITE);
        WaitForSingleObject(hCurrentProcess, INFINITE);
        std::cout << "Options: \n" <<
            "1. Write messages " + binFileName + " (max. message length is 20 characters) \n" <<
            "2. Exit program \n";
        std::cin >> option;
        switch (option) {
        case 1:
            option = 0;
            std::cout << "Input message: "; std::cin >> message;
            for (std::size_t i = 0; i < MESSAGE_LENGTH; i++)
                messageCharset[i] = '_';
            message.copy(messageCharset, message.length());
            binOutputStream.open(binFileName, std::ios::binary | std::ios::app);
            binOutputStream.write((char*)&messageCharset, sizeof(messageCharset));
            binOutputStream.close();
            ReleaseSemaphore(hReadAccessSemaphore, 1, NULL);
            break;
        case 2:
            std::cout << "Exiting proccess..." << std::endl;
            ReleaseMutex(hCurrentProcess);
            Sleep(500);
            break;
        }
    }
    return 0;
}