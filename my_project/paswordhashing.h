#ifndef PASSWORD_HASH_H
#define PASSWORD_HASH_H
#include <iostream>
#include <string>
#include <sodium.h>

std::string argon2_hash(const std::string& password);
bool verify_password(const::std::string& hashed_password, const std::string& password);
#endif
