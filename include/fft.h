#include <vector>
#include <cassert>
#include <complex>
#include <bit>

bool is_pow_two(int a) {
    while (a > 1) {
        if (a % 2 == 1) return false; 
        a /= 2;
    }
    return true;
}

std::vector<std::complex<double>> FFT(const std::vector<std::complex<double>>& data) {
    /*
    * standard/naiive cooley-tukey implementation. 
    * uses heap allocation for recursive subarrays.
    * potential optimisations can be made, though 
    * works well enough to illustrate the demo.
    */
    assert(is_pow_two(data.size()) == true);
    
    size_t n = data.size();
    if (n == 1) return data;

    std::vector<std::complex<double>> ODD(n/2);
    std::vector<std::complex<double>> EVEN(n/2);

    for (int i = 0; i < n; i+= 2)
        EVEN[i/2] = data[i];
    for (int i = 1; i < n; i+= 2)
        ODD[i/2] = data[i];

    EVEN = FFT(EVEN);
    ODD = FFT(ODD);

    std::vector<std::complex<double>> result(n);

    for (auto i = 0; i < n/2; i++) {
        result[i] = EVEN[i] + std::polar(1.0, 2.0 * M_PI / n * (double)i) * ODD[i];
        result[i + n/2] = EVEN[i] - std::polar(1.0, 2.0 * M_PI / n * (double)i) * ODD[i];
    }

    return result;
}

long revbits(const long a, const long befbit) {
    long b = 0; 
    for (int i = 0; i < befbit; i++) {
        b <<= 1;
        b += (a & (1<<i)) != 0;
    }
    return b;
}

std::vector<std::complex<double>> FFT_ip(const std::vector<std::complex<double>>& data) {
    /*
    * EXPLANATION:
    * In the examples provided, we are using an array of 
    * length 8 for demonstration purposes. Using the operator
    * "|" to represent combinations of arrays, the standard FFT
    * combinations can be represented as such:
    *     0|4
    *     2|6
    *     0 4|2 6
    *     1|5
    *     3|7
    *     1 5|3 7
    *     0 4 2 6|1 5 3 7
    * (source: trust me bro)
    * 
    * You may notice this can be written in an alternative 
    * order without any mathematical changes. You will see 
    * how this becomes important later:
    *     0|4
    *     2|6
    *     1|5
    *     3|7
    *     0 4|2 6 <- (this line has been moved)
    *     1 5|3 7
    *     0 4 2 6|1 5 3 7
    *
    * The logic behind this implementation comes down to an 
    * interesting property of bit reversals:
    *     0 1 2 3 4 5 6 7 -> 000 001 010 011 100 101 110 111
    *     0 4 2 6 1 5 3 7 -> 000 100 010 110 001 101 011 111
    *
    * By swapping each index in the original vector to its 
    * reversed bit value as shown above, the order of 
    * combinations becomes trivial to generate, by simply 
    * counting linearly in structured blocks:
    *     0|1
    *     2|3
    *     4|5
    *     6|7
    *     0 1|2 3
    *     4 5|6 7
    *     0 1 2 3|4 5 6 7
    * 
    * This leads to the final implementation of the algorithm,
    * which swaps data around after calculating the index, then
    * processes in blocks that look like this:
    *     1)  
    *         {0 1 2 3 4 5 6 7} -> {0 4 2 6 1 5 3 7}
    *     2)
    *         0|1 | 
    *         2|3 | 
    *         4|5 | j
    *         6|7 | 
    *         ---
    *          i
    *     3)
    *         0 1|2 3 |
    *         4 5|6 7 | j
    *         -------
    *            i
    *     4)
    *         0 1 2 3|4 5 6 7 | j
    *         ---------------
    *                i
    */

    assert(is_pow_two(data.size()) == true);

    size_t n = data.size();
    int rotpoint = std::countr_zero(n);
    
    std::vector<std::complex<double>> result = data;

    for (size_t i = 0; i < n; i++) {
        result[revbits(i, rotpoint)] = data[i];
    }

    /*
    * Step 2 .. 4 in example.
    */ 
    
    for (int len = 2; len <= n; len *= 2) {
        for (int batchPtr = 0; batchPtr < n; batchPtr += len) {
            int half = len/2;
            for (int j = 0; j < half; j++) {
                int evenIdx = batchPtr + j;
                int oddIdx  = evenIdx + half;
                auto twiddle = std::polar(1.0, -2.0 * M_PI * j / len);

                auto u = result[evenIdx];
                auto v = result[oddIdx] * twiddle;

                result[evenIdx] = u + v;
                result[oddIdx]  = u - v;
            }
        }
    }
    
    return result;
}