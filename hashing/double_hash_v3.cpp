

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>

int CUR_PRIME = 2;
long TABLE_SIZE = 0;
int INDICES = 0;

// sets next prime number after CUR_PRIME
void get_next_prime() {
    int isPrime = 0;
    CUR_PRIME++;
    int num = CUR_PRIME;
    while (isPrime == 0) {
        std::cout << "num = " << num << std::endl;
        int count = 0;

        // loop through values less than num
        for (int i = 1; i <= num; i++) {
            if ((num % i) == 0) {
                // not prime
                count++;
            }
        }

        if (count > 2) {
            // not prime
            num++;
            CUR_PRIME++;
        } else {
            // prime found
            isPrime = 1; 
        }
    }
    CUR_PRIME = num;
}

// -------------------- Hashing Functions

// first hash function, h()
unsigned int hash1(int input) {
    unsigned int hash = input % TABLE_SIZE;
    return hash;
}

// second hash function, h()
unsigned int hash2(int input) {
    //unsigned int hash = CUR_PRIME + (input % CUR_PRIME);
    unsigned int hash = 1 + (input % (TABLE_SIZE-1));
    //get_next_prime();
    return hash;
}

// double hash combining the other two
unsigned long doublehash(int k, int i) {
    unsigned long hash = (hash1(k) + i * hash2(k) ) % TABLE_SIZE;
    return hash;
}

// double double hash
/*
unsigned long ddhash(int[] hash_arr, int k, int i) {
    unsigned long hash1 = doublehash(k, i);
    unsigned long hash2 = doublehash(k, i+1);
    hash_arr[0] = hash1;
    hash_arr[1] = hash2;
}
    */

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

    unsigned long q1_index = doublehash(q1_int, 1);
    unsigned long q2_index = doublehash(q2_int, 1);
    unsigned long q3_index = doublehash(q3_int, 1);
    unsigned long q4_index = doublehash(q4_int, 1);
    unsigned long q5_index = doublehash(q1_int, 2);
    unsigned long q6_index = doublehash(q2_int, 2);
    unsigned long q7_index = doublehash(q3_int, 2);
    unsigned long q8_index = doublehash(q4_int, 2);

    if (!(arr[q1_index] & arr[q2_index] & arr[q3_index] & arr[q4_index] & arr[q5_index] & arr[q6_index] & arr[q7_index] & arr[q8_index])) {
        // hash found as all 0s, return immediately
        return 0;
    }

    // hash not found as all 0s
    return 1;
}

// --------------------  Main workflow

int main(int argc, char *argv[]) {
    std::cout << "---------------\nPassword Breach Checker" << std::endl;

    // dataset filt paths
    std::string training_file = "./pwnedpasswords_500k_pos.txt";
    std::string testing_file = "./pwnedpasswords_500k_neg.txt";

    // Sizing filter calculations
    long n = 500000;    // number of items to insert
    float p = 0.005;    // target false positive rate
    long m = ( (-1 * n) * std::log(p) ) / pow(std::log(2), 2);  // number of bits in bir array
    int k = std::round((m/n)*std::log(2));                      // number of times we'll run through double hash
        // set globals
    TABLE_SIZE = m;
    INDICES = k;
    std::cout << "Filter Sizing:"
    std::cout << "n = " << n << ", p = " << p << std::endl;
    std::cout << "m = " << m << ", k = " << k << std::endl;

    // init bit array and fill with zeroes
    int* bit_array = new int[m];
    for (int i = 0; i < m; i++) { 
        bit_array[i] = 0;
    }
    
    int count = 0;

    // open training dataset
    std::ifstream dataset(training_file);
    if (!dataset.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    std::cout << "File opened successfully" << std::endl;


    // loop through every line of dataset
    std::string line = "";
        // testing count variables
        int attempt_count = 0;
    while (std::getline(dataset, line, ':')) {
        //std::cout << line << std::endl; 
        
        // split hash in 'line' into more parts?
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
        //
        while (new_hash < 1) {
            //std::cout << "new_hash = " << new_hash << std::endl;
            if(attempts > 1) { 
                //std::cout << "attempts = " << attempts << std::endl;
            }
            // get 8 indeces
            unsigned long q1_index = doublehash(q1_int, attempts);
            unsigned long q2_index = doublehash(q2_int, attempts);
            unsigned long q3_index = doublehash(q3_int, attempts);
            unsigned long q4_index = doublehash(q4_int, attempts);
            unsigned long q5_index = doublehash(q1_int, attempts+1);
            unsigned long q6_index = doublehash(q2_int, attempts+1);
            unsigned long q7_index = doublehash(q3_int, attempts+1);
            unsigned long q8_index = doublehash(q4_int, attempts+1);
            attempt_count++;

            // check if indeces have already been entered in bit array
            if (bit_array[q1_index] & bit_array[q2_index] & bit_array[q3_index] & bit_array[q4_index] & bit_array[q5_index] & bit_array[q6_index] & bit_array[q7_index] & bit_array[q8_index]) {
                // not new hash
                attempts++;
            } else {
                bit_insert(bit_array, q1_index);
                bit_insert(bit_array, q2_index);
                bit_insert(bit_array, q3_index);
                bit_insert(bit_array, q4_index);
                bit_insert(bit_array, q5_index);
                bit_insert(bit_array, q6_index);
                bit_insert(bit_array, q7_index);
                bit_insert(bit_array, q8_index);
                new_hash++;
            }
        }
        // extra getline to get rid of extra value
        std::getline(dataset, line);
    }

    std::cout << "Closing training dataset\n" << std::endl;
    dataset.close();

    std::cout << "attempt_count = " << attempt_count << std::endl;

    std::ifstream test_dataset(testing_file);
    if (!test_dataset.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    std::cout << "Testing file opened successfully" << std::endl;

    float false_pos = 0;
    int query_count = 0;
    //auto start = std::chrono::steady_clock::now();
    //std::time_t start = std::time(0); 
    std::clock_t start = std::clock();
    //auto end = std::chrono::steady_clock::now();
    double query_time_array[m/1000];
    int key_count = 0;
    double query_sum = 0;

    // loop through testing dataset
    while (std::getline(test_dataset, line, ':')) {
        int temp_query = bit_query(bit_array, line);
        if (temp_query == 1) {
            false_pos++;
        }
        query_count++;
        std::getline(test_dataset, line);
        if (query_count == 1000) {
            //std::time_t end = std::time(0);
            std::clock_t end = std::clock();
            //double elapsed = std::difftime(end, start);
            double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC;
            query_sum += elapsed;
            key_count++;
            query_count = 0;
            //start = std::time(0);
            start = std::clock();
        }
    }

    // report false positive rate
    std::cout << "False positives = " << false_pos << std::endl;
    std::cout << "n = " << n << std::endl;
    float false_pos_rate = (false_pos / float(n)) * 100;
    std::cout << "False positive rate = " << false_pos_rate << "%" << std::endl;

    // report query timing
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double query_avg = query_sum / key_count;
    std::cout << "----- timing check;\nquery_sum = " << query_sum << "\nkey_count = " << key_count << std::endl;
    std::cout << "Query for 1,000 keys took: " << query_avg << " seconds on avg" << std::endl;
    //double query_s = duration.count() / 1e6;
    //std::cout << "                  or " << query_s << " seconds" << std::endl;

    // close testing file
    test_dataset.close();
    std::cout << "Closed testing dataset" << std::endl;

    // free memory
    delete[] bit_array;

    std::cout << "--------------" << std::endl;

    return 0;
}