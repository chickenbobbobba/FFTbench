#include <cassert>
#include <iostream>
#include <complex>
#include <vector>
#include <fft.h>
#include <chrono>

int main(int argc, char* argv[]) {
    using namespace std::chrono;

    size_t N = 1 << 24;
    size_t trials = 1 << 2;

    long t1;
    long t2;

    {
        std::vector<std::complex<double>> data(N);

        long ms = 0;
        
        for (size_t t = 0; t < trials; t++) {
            srand(t);
            for (size_t i = 0; i < N; i++)
                data[i] = { (double)rand() / RAND_MAX * 2 - 1, (double)rand() / RAND_MAX * 2 - 1 };
            
            auto start = high_resolution_clock::now();
            auto a = FFT(data);
            auto end = high_resolution_clock::now();
            ms += duration_cast<milliseconds>(end - start).count();
        }

        std::cout << "Ran " << trials << " naiive FFTs of size " << N
                << " in " << ms << " ms\n";
        
        t1 = ms;
    }
    {
        std::vector<std::complex<double>> data(N);

        long ms = 0;
        
        for (size_t t = 0; t < trials; t++) {
            srand(t);
            for (size_t i = 0; i < N; i++)
                data[i] = { (double)rand() / RAND_MAX * 2 - 1, (double)rand() / RAND_MAX * 2 - 1 };

            auto start = high_resolution_clock::now();
            auto a = FFT_ip(data);
            auto end = high_resolution_clock::now();
            ms += duration_cast<milliseconds>(end - start).count();
        }

        std::cout << "Ran " << trials << " in place FFTs of size " << N
                << " in " << ms << " ms\n";

        t2 = ms;
    }

    std::cout << "Thats a " << (1-(float)t2/t1)*100 << "\% improvement!\n";
    
}
