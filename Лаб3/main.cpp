#include <iostream>
#include <windows.h>
#include "ThreadInfo.h"


HANDLE* fromMain;
HANDLE* fromThread;
int* array;
int arrsize;
CRITICAL_SECTION cs;



void printArray() {
    for (int i = 0; i < arrsize; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}


DWORD WINAPI Marker(LPVOID v) {
    auto cur = (ThreadInfo*)v; //current ThreadInfo object
    EnterCriticalSection(&cs);
    srand(cur->num);
    
    bool* marked = new bool[arrsize]; //array to track marked elements
    for (int i = 0; i < arrsize; i++) {
        marked[i] = false;
    }
    LeaveCriticalSection(&cs);

    while(true) {
        int num = rand();
        EnterCriticalSection(&cs);
        LeaveCriticalSection(&cs);
        num %= (arrsize); //random number
        EnterCriticalSection(&cs);
        if (array[num] == 0) { //if the element is equal 0
            Sleep(5);
            array[num] = cur->num;
            marked[num] = true;
            ++cur->marked_num;
            LeaveCriticalSection(&cs);
            Sleep(5);
        }
        else if (array[num] != 0) { //if the element isn't equal 0, print info about thread
            std::cout << "\nMarker N" << cur->num 
                      << "\nCan't modify the element with index: " << num 
                      << "\nNumber of marked elements: " << cur->marked_num << std::endl << std::endl;
            LeaveCriticalSection(&cs);

            SetEvent(fromThread[cur->num - 1]);
            WaitForSingleObject(fromMain[cur->num - 1], INFINITE);
            ResetEvent(fromThread[cur->num - 1]);
            ResetEvent(fromMain[cur->num - 1]);
			WaitForSingleObject(fromMain[cur->num - 1], INFINITE);

            //got signal to finish thread 
            if (cur->flag == true) {
                EnterCriticalSection(&cs);
                for (int i = 0; i < arrsize; i++) {
                    if (marked[i]) {
                        array[i] = 0;
                    }
                }
                cur->marked_num = 0;
                LeaveCriticalSection(&cs);
                break;

            }
        }
    }
    delete[] marked;

    SetEvent(fromThread[cur->num - 1]);
    
    EnterCriticalSection(&cs);
    std::cout << "Thread N" << cur->num << " is finished\n";
    LeaveCriticalSection(&cs);
    return 0;
}


int main() {

    //initialization array
    std::cout << "Array size: " << std::endl;
    std::cin >> arrsize;

    array = new int[arrsize];
    for (int i = 0; i < arrsize; i++) {
        array[i] = 0;
    }

    //initialization Marker array
    std::cout << "Thread number: " << std::endl;
    int thnum;
    std::cin >> thnum;
    ThreadInfo** tharr = new ThreadInfo*[thnum];
    DWORD dword;
    InitializeCriticalSection(&cs);

    //initization of handle arrays
    fromMain = new HANDLE[thnum];
    fromThread = new HANDLE[thnum];
    for (int i = 0; i < thnum; i++) {
        tharr[i] = new ThreadInfo; 
        fromMain[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        fromThread[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        (*tharr[i]).num = i + 1;
        (*tharr[i]).h = CreateThread(NULL, 0, Marker, (LPVOID)tharr[i], 0, &dword);

    }

    int n;
    for (int i = 0; i < thnum; i++) {

        //waiting 'till all threads stop
        WaitForMultipleObjects(thnum, fromThread, TRUE, INFINITE);

        //resume threads execution

        for (int j = 0; j < thnum; j++)
			SetEvent(fromMain[j]);

        //input thread number to finish
	    EnterCriticalSection(&cs);
        printArray();
        do {
            std::cout << "Enter the num of thread to stop: ";
            std::cin >> n;
            if (tharr[n-1]->flag) {
                std::cout << "This thread is already finished\n";
            }
        } while (tharr[n-1]->flag);

        (*tharr[n-1]).flag = true;
        LeaveCriticalSection(&cs);

        //signal to finish current thread
        SetEvent(fromMain[n-1]);
        WaitForSingleObject(fromThread[n-1], INFINITE);

        //resume threads execution
        for (int j = 0; j < thnum; j++) {
            SetEvent(fromMain[j]);
            ResetEvent(fromMain[j]);
        }
    }

    std::cout << "All threads are finished";
    //closing handles
    for (int i = 0; i < thnum; i++) {
        CloseHandle(tharr[i]->h);
        CloseHandle(fromThread[i]);
        CloseHandle(fromMain[i]);
    }

    delete[] tharr;
    delete[] fromThread;
    delete[] fromMain;
    DeleteCriticalSection(&cs);
    delete[] array;
    return 0;

}