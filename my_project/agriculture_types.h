#ifndef AGRICULTURE_TYPES_H
#define AGRICULTURE_TYPES_H
#include "payment_types.h"
#include <string>
#include <vector>
#include <tuple>
#include <nlohmann/json.hpp>
using json=nlohmann::json;

class Farmers;
class Dealers;

// Shared types
struct Dealer{

	std::string dealer_id;
	std::string dealersname;
	std::string cooperation;
	std::string location;
	int age;
	int phonenumber;
};
struct PaymentRate{
	std::string produceType;
	double rate;
	std::string lastUpdated;
};


struct PaymentRecord { 
	std::string paymentDate; 
	double totalAmount;
	std::vector<FarmerPayment> farmerPayments;
	std::vector<PaymentRate> ratesUsed;
};



#endif
