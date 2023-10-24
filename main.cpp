#include "lib/Allocator.h"
#include <chrono>
#include <fstream>

int main() {
    std::vector <std::pair<int, int>> PoolsParametres;

    for(int i = 10000; i <= 4000000; i*=2){
        PoolsParametres.emplace_back(i,10);
    }

    Allocator<int> A(PoolsParametres);
    std::vector<int, Allocator<int>> vec1(A);

    std::vector <int> vec2;
    auto start = std::chrono::system_clock::now();
    for(int i = 10; i < 300000; i++){
        vec1.reserve(i);
    }
    auto stop = std::chrono::system_clock::now();
    auto dur1 = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    start = std::chrono::system_clock::now();

    for(int i = 10; i < 300000; i++){
        vec2.reserve(i);
    }
    stop = std::chrono::system_clock::now();
    auto dur2 = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout <<"Duration with custom allocator: " << dur1.count() << std::endl;
    std::cout << "Duration with default allocator: " << dur2.count() << std::endl;
    std::cout << (float)dur2.count()/(float)dur1.count() << " times profit";


}
