

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

int CUR_PRIME = 2;

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



// Hashing Functions

// first hash function, h()
int hash1(int input) {
    int hash = input % 11;
    return hash;
}

// second hash function, h()
int hash2(int input) {
    int hash = CUR_PRIME - (input % CUR_PRIME);
    //get_next_prime();
    return hash;
}

// double hash combining the other two
int doublehash(int k, int i) {
    int hash = (hash1(k) + i * hash2(k) ) % 11;
    return hash;
}


//
int main(int argc, char *argv[]) {

    // handle command line arguments 
    std::string arg = argv[1];
    std::string hash_file = "";

    if (arg == "train") {
        std::cout << "Using positive set for training" << std::endl;
        hash_file = "../dataset/pwnedpasswords_500k_pos.txt";
    } else if (arg == "test") {
        std::cout << "Using negative set for testing" << std::endl;
        hash_file = "../dataset/pwnedpasswords_500k_neg.txt";
    } else {
        std::cout << "Method invalid" << std::endl;
    }


    // open dataset
    std::ifstream dataset(hash_file);
    if (!dataset.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return 1;
    }
    std::cout << "File opened successfully" << std::endl;

    std::string line = "";

    // loop through every line of dataset
    while (std::getline(dataset, line, ':')) {
        std::cout << line << std::endl; 

        //doublehash()

        // extra getline to get rid of extra value
        std::getline(dataset, line);
    }


    std::cout << "Closing dataset" << std::endl;
    dataset.close();

    std::cout << "--------------" << std::endl;

    return 0;
}