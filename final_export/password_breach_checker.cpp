

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>

// Global variables
int CUR_PRIME = 2;
long TABLE_SIZE = 0;
int INDICES = 0;

// -------------------- Hashing Functions

// first hash function, h1(x)
unsigned long hash1(unsigned long x) {
    unsigned long hash = x % TABLE_SIZE;
    return hash;
}

// second hash function, h2(x)
unsigned long hash2(unsigned long x) {
    unsigned long hash = 1 + (x % (TABLE_SIZE-1));
    return hash;
}

// double hash combining the other two
unsigned long doublehash(int x, unsigned long i) {
    unsigned long hash = (hash1(x) + i * hash2(x) ) % TABLE_SIZE;
    return hash;
}

// generate and combine 4 quarters of hash
unsigned long get_combined_hash(unsigned long long q1, unsigned long long q2, unsigned long long q3, unsigned long long q4, int attempts){
    unsigned long q1_index = doublehash(q1, attempts);
    unsigned long q2_index = doublehash(q2, attempts);
    unsigned long q3_index = doublehash(q3, attempts);
    unsigned long q4_index = doublehash(q4, attempts);
    unsigned long h_index = (q1_index + q2_index + q3_index + q4_index) % TABLE_SIZE;
    return h_index;
}

// -------------------- Bit Array functions

// inserts single hash value into bit array to flip bit
void bit_insert(int* arr, unsigned long val) {
    if (arr[val] == 0) {
        arr[val] = 1;
    }
}

// checks whole bit array for given hash
int bit_query(int* arr, std::string hash_val) {
    int quarter = hash_val.size() / 4;
    std::string q1_hex = hash_val.substr(0,quarter);
    std::string q2_hex = hash_val.substr(quarter,quarter);
    std::string q3_hex = hash_val.substr(quarter*2,quarter);
    std::string q4_hex = hash_val.substr(quarter*3);

    unsigned long long q1_int = stoull(q1_hex,nullptr,16);
    unsigned long long q2_int = stoull(q2_hex,nullptr,16);
    unsigned long long q3_int = stoull(q3_hex,nullptr,16);
    unsigned long long q4_int = stoull(q4_hex,nullptr,16);

   for(int i = 0; i < INDICES; i++){
        // get k hash index
        unsigned long h_index = get_combined_hash(q1_int, q2_int, q3_int, q4_int, i+1);
        
        //check if the hash index is 0, if any is 0 then we know its not in the bit array
        if (arr[h_index] == 0){
            // hash found as all 0s, return immediately
            return 0;
        }
   }
    return 1;   // hash not found as all 0s
}

// --------------------  Main workflow

int main(int argc, char *argv[]) {
    std::cout << "--------------- Password Breach Checker with Bloom Filter" << std::endl;

    // dataset file paths
    std::string training_file = "pwnedpasswords_500k_pos.txt";
    std::string testing_file = "pwnedpasswords_500k_neg.txt";

    // Sizing filter calculations
    long n = 500000;    // number of items to insert
    float p = 0.005;    // target false positive rate
    long m = ( (-1 * n) * std::log(p) ) / pow(std::log(2), 2);  // number of bits in bir array
    int k = std::round((m/n)*std::log(2));                      // number of times we'll run through double hash
        // set globals
    TABLE_SIZE = m;
    INDICES = k;

    std::cout << "--- Filter Sizing:" << std::endl;
    std::cout << "n = " << n << ", p = " << p << std::endl;
    std::cout << "m = " << m << ", k = " << k << std::endl;

    // initialize bit array and fill with zeroes
    int* bit_array = new int[m];
    for (int i = 0; i < m; i++) { 
        bit_array[i] = 0;
    }

    std::cout << "\n--- Training and Testing Filter:" << std::endl;
    
    // open dataset
    std::ifstream dataset(training_file);
    if (!dataset.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    std::cout << "> Training dataset file opened successfully" << std::endl;

    // loop variables
    int count = 0;
    std::string line = "";

    // loop through every line of dataset
    while (std::getline(dataset, line, ':')) {
        // split hash in 'line' into four parts
        int quarter = line.size() / 4;
        std::string q1_hex = line.substr(0,quarter);
        std::string q2_hex = line.substr(quarter,quarter);
        std::string q3_hex = line.substr(quarter*2,quarter);
        std::string q4_hex = line.substr(quarter*3);

        unsigned long long q1_int = stoull(q1_hex,nullptr,16);
        unsigned long long q2_int = stoull(q2_hex,nullptr,16);
        unsigned long long q3_int = stoull(q3_hex,nullptr,16);
        unsigned long long q4_int = stoull(q4_hex,nullptr,16);

        // run double hash and fill bit array
        int new_hash = 0;
        int attempts = 1;

        while (new_hash < 1) {
            unsigned long h_index[k];
            bool all_ones = true;
            // get 8 indices
            // each of the 4 quaters are put through the double hash function
            // Then combined all together to get one index
            for(int i = 0; i < k; i++){
                attempts++;
                h_index[i] = get_combined_hash(q1_int, q2_int, q3_int, q4_int, attempts);

                //check if one bit is 0, if one is 0 then that mean new hash
                if(bit_array[h_index[i]] == 0){
                    all_ones = false;
                }
            }

            // check if indeces have already been entered in bit array
            if (all_ones) {
                // not new hash
                attempts++;
            } else {
                for(int i = 0; i < k; i++){
                    bit_insert(bit_array, h_index[i]);
                }
                new_hash++;
            }
        }
        std::getline(dataset, line);    // extra getline to get rid of un-needed value
    }

    // done with training, close file
    dataset.close();
    std::cout << "< Closed training dataset file" << std::endl;

    // open testing dataset
    std::ifstream test_dataset(testing_file);
    if (!test_dataset.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    std::cout << "> Testing dataset file opened successfully" << std::endl;

    // testing variables
    float false_pos = 0;    // num of false positives
    int query_count = 0;    // for counting all queries
    double query_sum = 0;   // to sum the time taken for 1000 queries
    std::clock_t start = std::clock();

    // loop through testing dataset
    while (std::getline(test_dataset, line, ':')) {
        int temp_query = bit_query(bit_array, line);
        if (temp_query == 1) {
            false_pos++;
        }
        query_count++;
        std::getline(test_dataset, line);
        if (query_count == 1000) {
            std::clock_t end = std::clock();
            double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;
            query_sum += elapsed;
            query_count = 0;
            start = std::clock();   // reset start point for next 1000
        }
    }
    
    // close testing file
    test_dataset.close();
    std::cout << "< Closed testing dataset" << std::endl;

    // report false positive rate
    std::cout << "\n--- Training Report:" << std::endl;
    std::cout << "False positives = " << false_pos << std::endl;
    float false_pos_rate = (false_pos / float(n)) * 100;
    std::cout << "False positive rate = " << false_pos_rate << "%" << std::endl;

    // report query timing
    std::cout << "\n--- Timing Report: " << std::endl;
    std::cout << "Total run time = " << query_sum << " seconds" << std::endl;
    double query_avg_s = query_sum / (m/1000);
    double query_avg_us = query_avg_s * 1e6;
    std::cout << "Average query for 1,000 keys is:" << std::endl;
    std::cout << "\t" << query_avg_s << " seconds, or " << query_avg_us << " microseconds" << std::endl;

    // free memory
    delete[] bit_array;

    std::cout << "-------------- EOF" << std::endl;
}