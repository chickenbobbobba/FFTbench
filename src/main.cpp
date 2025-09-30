#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>
#include <complex>
#include <vector>
#include <fft.h>
#include <chrono>

enum arg_type {
    INT,
    STR,
    FLOAT,
    BOOL,
    NIL,
};

std::unordered_map<std::string, arg_type> accepted_args {
    {"file",STR},
    {"out",STR},
};

struct config {
    std::string arg;
    std::string data;
    bool operator==(const config& other) const {
        return arg == other.arg;
    }
};

auto parse_args(int argc, char* argv[]) {
    auto hash = [](const config& a) -> std::size_t {
        return std::hash<std::string>{}(a.arg);  // must return std::size_t
    };

    std::unordered_set<config, decltype(hash)> args(0, hash);
    std::unordered_map<std::string, arg_type> short_args {};

    int i = 1;
    while (i < argc) {
        std::string arg = argv[i];
        if (arg.starts_with("--")) {
            std::string argval = argv[i+1];
            config conf;
            conf.arg = arg.erase(0, 2);
            conf.data = argval;
            args.insert(conf);
            i += 2;
        } else {
            i++;
        }
    }

    return args;
}

int main(int argc, char* argv[]) {
    auto args = parse_args(argc, argv);
    for (auto i : args) {
        std::cout << "got arg: \"" << i.arg << "\" with value: \"" << i.data << "\"\n";
    }

    using namespace std::chrono;

    std::cout << revbits(0b110, 3) << "\n";

    size_t N = 1 << 16;
    size_t trials = 1 << 8;

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
