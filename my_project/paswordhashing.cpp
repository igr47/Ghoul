#include <iostream>
#include <string>
#include <sodium.h>
#include "paswordhashing.h"

std::string argon2_hash(const std::string &password) {
    char hashed_password[crypto_pwhash_STRBYTES];
    if (crypto_pwhash_str(
            hashed_password,
            password.c_str(),
            password.length(),
            crypto_pwhash_OPSLIMIT_MODERATE,
            crypto_pwhash_MEMLIMIT_MODERATE) != 0) {
        std::cerr << "Hashing failed (out of memory?)" << std::endl;
        return "";
    }
    return std::string(hashed_password);
}

bool verify_password(const std::string &hashed_password, const std::string &password) {
    return crypto_pwhash_str_verify(hashed_password.c_str(), password.c_str(), password.length()) == 0;
}

