#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstring>
#include <cmath>

// Keep the SIZE constant and the function exactly as provided by you.
const int SIZE = 256;

double multiply_matrices(double a[256][256], double b[256][256]) {
    double result[SIZE][SIZE] = {0};
    double sumOfSquares = 0.0;

    // Perform matrix multiplication using a cache-friendly approach
    for (int i = 0; i < SIZE; i++) {
        for (int k = 0; k < SIZE; k++) {
            double temp = a[i][k];
            for (int j = 0; j < SIZE; j++) {
                result[i][j] += temp * b[k][j];
            }
        }
    }

    // Calculate the sum of squares of the resulting matrix
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            sumOfSquares += result[i][j] * result[i][j];
        }
    }

    return sumOfSquares;
}

/*
  Helper functions and test harness (do not modify multiply_matrices above).

  - fill_test_matrices fills A and B with deterministic, nontrivial values so
    results are reproducible across runs and machines.
  - print_matrix_sample prints a small sample of the product (for sanity checks).
  - main runs a single correctness run and a short timed benchmark loop.
*/

/* Fill matrices with a deterministic pattern for testing.
   Using simple arithmetic patterns makes it easy to reason about correctness
   and reproduce results. */
void fill_test_matrices(double a[SIZE][SIZE], double b[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            // Nontrivial, bounded values to avoid overflow and keep numerical stability.
            a[i][j] = static_cast<double>((i + 1) % 17) * 0.125 + static_cast<double>((j + 3) % 13) * 0.007;
            b[i][j] = static_cast<double>((i + 5) % 19) * 0.0625 - static_cast<double>((j + 7) % 11) * 0.003;
        }
    }
}

/* Optional: compute a tiny 4x4 product using the same algorithm but on a small
   subset to print and inspect values for sanity. This does not change the main
   multiply_matrices function and is only for human-readable verification. */
void print_sample_product(double a[SIZE][SIZE], double b[SIZE][SIZE]) {
    // Compute a small 4x4 product naively for display
    const int M = 4;
    double small[M][M] = {0};
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < M; ++j) {
            double s = 0.0;
            for (int k = 0; k < M; ++k) s += a[i][k] * b[k][j];
            small[i][j] = s;
        }
    }

    std::cout << "Sample 4x4 product (top-left corner):\n";
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < M; ++j) {
            std::cout << std::setw(12) << std::setprecision(6) << small[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::flush;
}

int main() {
    // Allocate matrices statically to avoid large stack frames in some environments.
    static double A[SIZE][SIZE];
    static double B[SIZE][SIZE];

    // Fill matrices with deterministic test data.
    fill_test_matrices(A, B);

    // Print a small sample of the product computed on a 4x4 subset for sanity.
    // This is only a human-check; the official function computes the full 256x256 product.
    print_sample_product(A, B);

    // Single-run correctness check: call the provided function and print the result.
    double sumSq = multiply_matrices(A, B);
    std::cout << std::fixed << std::setprecision(10);
    std::cout << "Sum of squares (single run): " << sumSq << "\n";

    // Short benchmark: run the function multiple times and measure elapsed time.
    // The problem statement aims for many calls in a short time; here we run a small loop.
    const int runs = 5; // adjust for quick local benchmarking
    // Warm-up run to reduce first-call overhead (page faults, caches, etc.)
    (void)multiply_matrices(A, B);

    auto t0 = std::chrono::high_resolution_clock::now();
    double last = 0.0;
    for (int r = 0; r < runs; ++r) {
        last = multiply_matrices(A, B);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = t1 - t0;

    std::cout << "Last run sum of squares: " << last << "\n";
    std::cout << "Total time for " << runs << " runs: " << elapsed.count() << " seconds\n";
    std::cout << "Average time per run: " << (elapsed.count() / runs) << " seconds\n";

    // Basic sanity check: ensure the returned value is finite and non-negative.
    if (!std::isfinite(last) || last < 0.0) {
        std::cerr << "Sanity check failed: result is not a finite non-negative number.\n";
        return 2;
    }

    return 0;
}
//only submit method and keep size constant
//Need faster code 3.2s down to 0.8s