#include <iostream>
#include <bits/stdc++.h>
#include <windows.h>
#include "DataStruct.h"
#include <math.h>

int min;
int max;
double average;

//функция для создания потоков
bool createThread(DWORD WINAPI func(LPVOID v), DataStruct* array, DWORD& dword) {
    HANDLE hThread = CreateThread(NULL, 0, func, (void*)array, 0, &dword);
    
    if (hThread == NULL) {
        std::cout << "The stream did not start!" << std::endl;
        return false;
    }

    if (WaitForSingleObject(hThread, INFINITE)) {  //ожидание потока 
        std::cout << "Thread timeout exceeded!" << std::endl;
        return false;
    }
    CloseHandle(hThread);
    return true;
}

//вывод массива
void printArr(const DataStruct* array) {
    for (int i = 0; i < array->N; i++) {
        std::cout << array->arr[i] <<" ";
    }
    std::cout << std::endl;
}

//минимальный и максимальный элементы
DWORD WINAPI MaxMin(LPVOID v) {
    
    DataStruct* array = (DataStruct*) v;
    max = INT_MIN;
    min = INT_MAX;
    for (int i = 0; i < array->N; i++) {
        if (min > array->arr[i]) {
            min = array->arr[i];
            Sleep(7);
        }
        if (max < array->arr[i]) {
            max = array->arr[i];
            Sleep(7);
        }
    }
    std::cout << "Max: " << max << ", min: " << min << std::endl;
    return 0;
}

//средний по значению элемент
DWORD WINAPI Avg(LPVOID v) {
    DataStruct* array = (DataStruct*) v;
    average = 0;
    for (int i = 0; i < array->N; i++) {
        average += (double)array->arr[i];
        Sleep(12);
    }
    average /= (double)array->N;
    std::cout << "Average: " << average << " ~ "<< (int)round(average) << std::endl;
    return 0;
}

int N; //кол-во элементов

int main() {
    std::cout << "Enter the number of array elements: ";
    std::cin >> N;
    DataStruct* array = new DataStruct(N);
    DWORD dwThread;

    //заполнение массива
    std::cout << "Enter the array elements:\n"; 
    for (int i = 0; i < N; i++) {
        std::cout << i+1 << ") ";
        std::cin >> array->arr[i];
    }

    std::cout << "Array: ";
    printArr(array);
    createThread(MaxMin, array, dwThread);
    createThread(Avg, array, dwThread);

    //изменение максимальных и минимальных элементов массива на средний
    for (int i = 0; i < N; i++) {
        if (array->arr[i] == min || array->arr[i] == max)  {
            array->arr[i] = (int)round(average);
        }
    }
    std::cout << "Modified array: ";
    printArr(array);
    delete array;
    return 0;
}