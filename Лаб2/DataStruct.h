struct DataStruct
{
    int N = 0;
    int* arr = nullptr;
    
    DataStruct(int N) {
        this->N = N;
        arr = new int[N];
    }
    ~DataStruct() {
        delete[] arr;
        arr = nullptr;
    }
};
