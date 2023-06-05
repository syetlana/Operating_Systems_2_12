#include <windows.h>

struct ThreadInfo {
    //handle
    HANDLE h;

    //Marker number
    int num;

    //stopped
    bool flag = false;

    int marked_num = 0;
};