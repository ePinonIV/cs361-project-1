#include <iostream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <algorithm>

/**
 * Function: merge
 * ----------------------------------------------------------------------
 * Merges three sorted subarrays of gArray[] back into destArray[].
 * This addresses the core requirement of combining 3 sorted partitions.
 */
template <typename T>
void merge(T gArray[], long long low, long long mid1, long long mid2, long long high, T destArray[]) {
    long long i = low;      // Cursor pointer for the first segment [low to mid1)
    long long j = mid1;     // Cursor pointer for the second segment [mid1 to mid2)
    long long k = mid2;     // Cursor pointer for the third segment [mid2 to high)
    long long l = low;      // Cursor pointer for writing to the destination array

    // Loop continuously as long as all three segments still have unpicked elements
    while ((i < mid1) && (j < mid2) && (k < high)) {
        if (gArray[i] < gArray[j]) {
            if (gArray[i] < gArray[k]) {
                destArray[l++] = gArray[i++]; // Element in 1st segment is smallest
            } else {
                destArray[l++] = gArray[k++]; // Element in 3rd segment is smallest
            }
        } else {
            if (gArray[j] < gArray[k]) {
                destArray[l++] = gArray[j++]; // Element in 2nd segment is smallest
            } else {
                destArray[l++] = gArray[k++]; // Element in 3rd segment is smallest
            }
        }
    }

    // Handlers for when only TWO of the three segments still have elements remaining:
    while ((i < mid1) && (j < mid2)) {
        destArray[l++] = (gArray[i] < gArray[j]) ? gArray[i++] : gArray[j++];
    }
    while ((j < mid2) && (k < high)) {
        destArray[l++] = (gArray[j] < gArray[k]) ? gArray[j++] : gArray[k++];
    }
    while ((i < mid1) && (k < high)) {
        destArray[l++] = (gArray[i] < gArray[k]) ? gArray[i++] : gArray[k++];
    }

    // Handlers for when only ONE segment still has elements remaining:
    while (i < mid1) destArray[l++] = gArray[i++];
    while (j < mid2) destArray[l++] = gArray[j++];
    while (k < high) destArray[l++] = gArray[k++];
}

/**
 * Function: threeWayMergeSortRec
 * ----------------------------------------------------------------------
 * The recursive driver. It breaks the array down into 3 parts, sorts
 * them recursively, and calls the merge function.
 */
template <typename T>
void threeWayMergeSortRec(T gArray[], long long low, long long high, T destArray[]) {
    // Base case: If size is less than 2, the subarray is already sorted
    if (high - low < 2) return;

    // Split the range into 3 roughly equal parts
    long long mid1 = low + ((high - low) / 3);
    long long mid2 = low + 2 * ((high - low) / 3) + 1;

    // Recursively sort the 3 split parts
    // Swapping gArray and destArray passes correctly sorted data up the tree
    threeWayMergeSortRec(destArray, low, mid1, gArray);
    threeWayMergeSortRec(destArray, mid1, mid2, gArray);
    threeWayMergeSortRec(destArray, mid2, high, gArray);

    // Merge the sorted parts back together
    merge(destArray, low, mid1, mid2, high, gArray);
}

/**
 * Function: threeWayMergeSort
 * ----------------------------------------------------------------------
 * The wrapper function exposed to the user. It creates the temporary array
 * required for out-of-place merging to satisfy project rules.
 */
template <typename T>
void threeWayMergeSort(T data[], long long n) {
    if (n <= 1) return;

    // Strict project rule: Use basic array instead of std::vector
    T* tempArray = new T[n]; 
    for (long long i = 0; i < n; i++) {
        tempArray[i] = data[i];
    }

    // Initiate recursive sorting
    threeWayMergeSortRec(tempArray, 0, n, data);

    // Clean up auxiliary heap memory
    delete[] tempArray;
}

/**
 * Function: runBenchmark
 * ----------------------------------------------------------------------
 * Fills arrays, isolates I/O from runtime, and computes elapsed time
 * for both Ints and Doubles at a given power of 2.
 */
void runBenchmark(int power) {
    long long n = (long long)1 << power; // Bit-shift computes 2^power efficiently
    std::cout << "--- Benchmarking Size 2^" << power << " (" << n << " elements) ---" << std::endl;

    // --- Part A: Integer Sort ---
    int* intData = new int[n];
    for (long long i = 0; i < n; i++) intData[i] = rand();

    // Start timer AFTER data load to exclude I/O time as required
    auto s1 = std::chrono::high_resolution_clock::now(); 
    threeWayMergeSort(intData, n);
    auto e1 = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> t1 = e1 - s1;
    std::cout << "Integer Sort Time: " << std::fixed << std::setprecision(5) << t1.count() << "s" << std::endl;
    
    delete[] intData; // Delete immediately to free RAM before making double array

    // --- Part B: Double Sort ---
    double* doubleData = new double[n];
    for (long long i = 0; i < n; i++) doubleData[i] = (double)rand() / RAND_MAX;

    auto s2 = std::chrono::high_resolution_clock::now();
    threeWayMergeSort(doubleData, n);
    auto e2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> t2 = e2 - s2;
    std::cout << "Double Sort Time:  " << std::fixed << std::setprecision(5) << t2.count() << "s" << std::endl;
    
    delete[] doubleData;
    std::cout << "------------------------------------------------" << std::endl;
}

int main() {
    // Loop through required benchmarking sizes 2^20 to 2^30
    for (int p = 20; p <= 30; p++) {
        try {
            runBenchmark(p);
        } catch (const std::bad_alloc& e) {
            // Catches any out-of-memory errors for report query 3(b)
            std::cerr << "Memory Limit Reached at 2^" << p << std::endl;
            break;
        }
    }
    return 0;
}