

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

int CUR_PRIME = 2;
long TABLE_SIZE = 0;

// sets next prime number after CUR_PRIME
void get_net_prime() {
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

// -------------------- Bit Array functions

// inserts single hash value into bit array to flip bit
void bit_insert(int* arr, unsigned long val) {
    arr[val] = 1;
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

    unsigned long q1_hash = doublehash(q1_int, 1);
    unsigned long q2_hash = doublehash(q2_int, 1);
    unsigned long q3_hash = doublehash(q3_int, 1);
    unsigned long q4_hash = doublehash(q4_int, 1);

    int hash_found = 0;
    if (arr[q1_hash] & arr[q2_hash] & arr[q3_hash] & arr[q4_hash]) {
        hash_found = 1;
    }

    return hash_found;
}

// --------------------  Main workflow

int main(int argc, char *argv[]) {
    std::cout << "---------------" << std::endl;

    // get dataset(s)
    std::string training_file = "../dataset/pwnedpasswords_500k_pos.txt";
    std::string testing_file = "../dataset/pwnedpasswords_500k_neg.txt";

    // open dataset
    std::ifstream dataset(training_file);
    if (!dataset.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    std::cout << "File opened successfully" << std::endl;


    // Sizing filter calculations
    long n = 500000;
    float p = 0.01;     // change to 0.02 if splitting line

    long m = ( (-1 * n) * std::log(p) ) / pow(std::log(2), 2);
    TABLE_SIZE = m;
    int k = std::round((m/n)*std::log(2));

    // init bit array and fill with zeroes
    int* bit_array = new int[m];
    for (int i = 0; i < m; i++) { 
        bit_array[i] = 0;
    }
    
    int count = 0;

    // loop through every line of dataset
    std::string line = "";
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
        while (new_hash == 0) {
            if(attempts > 1) { 
                //std::cout << "attempts = " << attempts << std::endl;
            }
            unsigned long q1_hash = doublehash(q1_int, attempts);
            unsigned long q2_hash = doublehash(q2_int, attempts);
            unsigned long q3_hash = doublehash(q3_int, attempts);
            unsigned long q4_hash = doublehash(q4_int, attempts);

            if (bit_array[q1_hash] & bit_array[q2_hash] & bit_array[q3_hash] & bit_array[q4_hash]) {
                // not new hash
                attempts++;
            } else {
                bit_insert(bit_array, q1_hash);
                bit_insert(bit_array, q2_hash);
                bit_insert(bit_array, q3_hash);
                bit_insert(bit_array, q4_hash);
                new_hash = 1;

            }
        }

        // extra getline to get rid of extra value
        std::getline(dataset, line);
    }

    std::cout << "Closing training dataset\n" << std::endl;
    dataset.close();

    std::ifstream test_dataset(testing_file);
    if (!test_dataset.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    std::cout << "Testing file opened successfully" << std::endl;

    // loop while
    float false_pos = 0;
    while (std::getline(test_dataset, line, ':')) {
        int temp_query = bit_query(bit_array, line);
        if (temp_query == 1) {
            false_pos++;
        }

        std::getline(test_dataset, line);
    }

    std::cout << "False positives = " << false_pos << std::endl;
    std::cout << "n = " << n << std::endl;
    float false_pos_rate = (false_pos / float(n)) * 100;
    std::cout << "False positive rate = " << false_pos_rate << "%" << std::endl;

    std::cout << "Closing testing dataset" << std::endl;
    test_dataset.close();


    delete[] bit_array;

    std::cout << "--------------" << std::endl;

    return 0;
}