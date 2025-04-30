#include <random>
#include <sstream>
#include <string>
#include <array>
#include <iomanip>

std::string generateUUID() {
    // Use a better random number generator
    std::random_device rd;
    std::array<uint32_t, 4> seed_data{};
    std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
    std::seed_seq seq(seed_data.begin(), seed_data.end());
    std::mt19937_64 gen(seq); // 64-bit Mersenne Twister
    
    // Uniform distributions
    std::uniform_int_distribution<uint64_t> dis(0, 0xFFFFFFFFFFFFFFFF);
    
    // Generate all random bits at once
    uint64_t rb1 = dis(gen);
    uint64_t rb2 = dis(gen);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    // Format as UUID (8-4-4-4-12)
    ss << std::setw(8) << ((rb1 >> 32) & 0xFFFFFFFF);
    ss << "-";
    ss << std::setw(4) << ((rb1 >> 16) & 0xFFFF);
    ss << "-";
    ss << std::setw(4) << (((rb1 >> 0) & 0x0FFF) | 0x4000); // Version 4
    ss << "-";
    ss << std::setw(4) << (((rb2 >> 48) & 0x3FFF) | 0x8000); // Variant 1
    ss << "-";
    ss << std::setw(4) << ((rb2 >> 32) & 0xFFFF);
    ss << std::setw(8) << (rb2 & 0xFFFFFFFF);
    
    return ss.str();
}
