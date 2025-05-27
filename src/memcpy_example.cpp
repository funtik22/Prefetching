#include <iostream>
#include <cstdlib>
#include <ctime>    
#include <cstring> 

#define SIZE 1000 // размер массива

int main() {
    int* source = new int[SIZE];
    int* destination = new int[SIZE];

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    for (size_t i = 0; i < SIZE; ++i) {
        source[i] = std::rand();
    }

    std::cout<<"MEMCPY_START"<<std::flush;

    std::memcpy(destination, source, SIZE * sizeof(int));

    std::cout<<"MEMCPY_END"<<std::flush;

    delete[] source;
    delete[] destination;

    return 0;
}
