#pragma once
#ifndef PAYMENT_TYPES_H
#define PAYMENT_TYPES_H
#include <nlohmann/json.hpp>
#include <string>

struct FarmerPayment {
       std::string farmerId;
       std::string farmerName;
       std::string produceType;
       double quantity;
       double amount;
       std::string date;

       nlohmann::json toJson() const{
	                return {
                                {"FarmerId", farmerId},
                                {"FarmerName", farmerName},
                                {"ProduceType", produceType},
                                {"Quantity", quantity},
                                {"Amount", amount},
                                {"Date", date}                                           };
	}
       void fromJson(const nlohmann::json& j){                                           farmerId = j.value("FarmerId", "");
                        farmerName = j.value("FarmerName", "");
                        produceType = j.value("ProduceType", "");
                        quantity = j.value("Quantity", 0.0);
                        amount = j.value("Amount", 0.0);
			date = j.value("Date", "");
                }
};
#endif
