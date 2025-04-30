#ifndef DEALERS_CLASS
#define DEALERS_CLASS
#include "farmersclass.h"
#include "payment_types.h"
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <memory>
#include <stdexcept>
#include "agriculture_types.h"
#include <functional>
using json=nlohmann::json;

class Dealers:public Farmers{
	public:
		struct PaymentRate {
                        std::string produceType;
                        double rate;
                        std::string lastUpdated;
                        json toJson() const {
                                return {
                                        {"ProduceType", produceType},
                                        {"Rate", rate},
                                        {"LastUpdated", lastUpdated}
                                };
                        }

                        void fromJson(const json& j) {
				produceType = j.value("ProduceType", "");
                                rate = j.value("Rate", 0.0);
                                lastUpdated = j.value("LastUpdated", "");
                        }
		};


                struct PaymentRecord {
                        std::string paymentDate;
                        double totalAmount;
                        std::vector<FarmerPayment> farmerPayments;
                        std::vector<PaymentRate> ratesUsed;

                        json toJson() const {
                                json j;
                                j["PaymentDate"] = paymentDate;
                                j["TotalAmount"] = totalAmount;

                                json paymentsArray = json::array();
                                for (const auto& payment : farmerPayments) {
                                        paymentsArray.push_back(payment.toJson());               }
				j["FarmerPayments"] = paymentsArray;

                                json ratesArray = json::array();
                                for (const auto& rate : ratesUsed) {
                                        ratesArray.push_back(rate.toJson());
                                }
                                j["RatesUsed"] = ratesArray;

                                return j;
                        }

                        void fromJson(const json& j) {
                                paymentDate = j.value("PaymentDate", "");
                                totalAmount = j.value("TotalAmount", 0.0);

                                farmerPayments.clear();
                                if (j.contains("FarmerPayments") && j["FarmerPayments"].is_array()){
                                                for (const auto& item : j["FarmerPayments"]) {
                                                        FarmerPayment payment;
                                                        payment.fromJson(item);
                                                        farmerPayments.push_back(payment);
                                                }
                                        }

                                ratesUsed.clear();
                                if (j.contains("RatesUsed") && j["RatesUsed"].is_array()) {
                                        for (const auto& item : j["RatesUsed"]) {
                                                PaymentRate rate;
                                                rate.fromJson(item);
                                                ratesUsed.push_back(rate);
                                        }
                                }
                        }
                };	
		struct Inventory{
			std::string item;
			std::string farmer_name;
			std::unique_ptr<std::vector<double>> quantity=std::make_unique<std::vector<double>>();
			json toJson() const{
				return{
					{"Item",item},
					{"farmer_name",farmer_name},
					{"Quantity",*quantity}
				};
			}
			void fromJson(const json& j){
				item=j.value("Item","");
				farmer_name=j.value("farmer_name","");
				quantity=std::make_unique<std::vector<double>>();
				quantity->clear();
				for(const auto& val : j.value("Item",json::array())){
					if(val.is_number()){
						quantity->push_back(val.get<double>());
					}
				}
			}
		};
		struct Transaction {
                                        std::string date;
                                        std::shared_ptr<std::string> farmer;  // Shared ownership of farmer reference
                                        std::shared_ptr<std::string> farm;    // Shared ownership of farm reference
                                        std::unique_ptr<double> amount;       // Exclusive ownership of amount

                                        Transaction(const std::string& d,std::shared_ptr<std::string> f,std::shared_ptr<std::string> fm,double a): date(d),farmer(std::move(f)),farm(std::move(fm)),amount(std::make_unique<double>(a)){}
					Transaction(const Transaction&)=delete;
					Transaction(Transaction&&)=default;
					Transaction& operator=(Transaction&&)=default;
                                        json toJson() const {
                                                return {
                                                        {"Date", date},
                                                        {"Farmer", farmer ? *farmer : ""},
                                                        {"Farm", farm ? *farm : ""},
                                                        {"Amount", amount ? *amount : 0.0}
                                                };
                                        }

                                        void fromJson(const json& j) {
                                                date = j.value("Date", "");
                                                farmer = std::make_shared<std::string>(j.value("Farmer", ""));
                                                farm = std::make_shared<std::string>(j.value("Farm", ""));
                                                amount = std::make_unique<double>(j.value("Amount", 0.0));
                                        }
                                };

		struct Dealer:public Base{
			std::string dealer_id;
			std::string dealersname;
			std::string cooperation;
			std::string location;
			int age;
			int phonenumber;
			std::vector<Inventory>inventory;
			std::vector<Transaction>transactions;
			std::vector<PaymentRate> paymentRates;
                        std::vector<PaymentRecord> paymentRecords;
                        double totalPayments = 0.0;

                        void addPaymentRecord(const PaymentRecord& record) {
                                paymentRecords.push_back(record);
                                totalPayments += record.totalAmount;
                                updateTimestamp();
                        }

			json toJson() const{
				json j;
				j["Id"]=dealer_id;
				j["Name"]=dealersname;
				j["Cooperation_Name"]=cooperation;
				j["Location"]=location;
				j["Yaer Of Birth"]=age;
				j["PhoneNumber"]=phonenumber;
				//serialize the inventory
				json inventoryArray=json::array();
				for(const auto& item : inventory){
					inventoryArray.push_back(item.toJson());
				}
				j["Inventory"]=inventoryArray;
				json transactionArray=json::array();
				for(const auto& txn : transactions){
					transactionArray.push_back(txn.toJson());
				}
				j["Transactions"]=transactionArray;

                                json ratesArray = json::array();
                                for (const auto& rate : paymentRates) {
                                        ratesArray.push_back(rate.toJson());
                                }
                                j["PaymentRates"] = ratesArray;

                                json recordsArray = json::array();
                                for (const auto& record : paymentRecords) {
                                        recordsArray.push_back(record.toJson());
                                }
                                j["PaymentRecords"] = recordsArray;

                                j["TotalPayments"] = totalPayments;
				return j;
			}
			void fromJson(const json& j) {
                                dealer_id=j.value("Id",""); 
				dealersname = j.value("Name", "");
                                cooperation = j.value("Cooperation_Name", "");
                                location = j.value("Location", "");
                                age = j.value("Year_Of_Birth", 0);
                                phonenumber = j.value("PhoneNumber", 0);

                                
                                inventory.clear();
                                if (j.contains("Inventory") && j["Inventory"].is_array()) {
                                        for (const auto& item : j["Inventory"]) {
                                                        Inventory i;
							i.fromJson(item);
							inventory.push_back(std::move(i));
                                        }
                                }

                                transactions.clear();
                                if (j.contains("Transactions") && j["Transactions"].is_array()) {
                                        for (const auto& txnJson : j["Transactions"]) {
                                                Transaction txn("",nullptr,nullptr,0.0);
						txn.fromJson(txnJson);
						transactions.emplace_back(std::move(txn));
                                        }
                                }
				paymentRates.clear();                                            if (j.contains("PaymentRates") && j["PaymentRates"].is_array()) {
                                        for (const auto& item : j["PaymentRates"]) {
                                                PaymentRate rate;
                                                rate.fromJson(item);
                                                paymentRates.push_back(rate);
					}
                                }

                                paymentRecords.clear();
                                if (j.contains("PaymentRecords") && j["PaymentRecords"].is_array()) {
                                        for (const auto& item : j["PaymentRecords"]) {
                                                PaymentRecord record;
						record.fromJson(item);
						paymentRecords.push_back(record);                            }
                                }

                                totalPayments = j.value("TotalPayments", 0.0);
		}
                                
		};
		std::vector<std::shared_ptr<Dealer>>dealerslist;
	public:
		Dealers();
		
		void getDealersDetails();
		static std::vector<std::shared_ptr<Dealers::Dealer>> readFromFile();
		void displayDealersDetails(const std::function<std::vector<std::shared_ptr<Dealer>>()>& readFunction);
		void searchDealersDetails(const std::function<std::vector<std::shared_ptr<Dealer>>()>& radeFunction,const std::string& name);
		void updateDealersDetails(const std::function<std::vector<std::shared_ptr<Dealer>>()>& readFunction);
		void deleteDealersDetails(const std::function<std::vector<std::shared_ptr<Dealer>>()>& readFunction);
		void viewAvailableProduce(const std::function<std::vector<std::shared_ptr<Farmers::Farmer>>()>& readFunction);
		void purchaseFarmersProduce(const std::function<std::vector<std::shared_ptr<Farmers::Farmer>>()>& readFunction,const std::function<std::vector<std::shared_ptr<Dealers::Dealer>>()>& readFunctions);
		json toJsonDealers(std::vector<std::shared_ptr<Dealer>>& dealers) const;
		void processProducePurchase(Farmers::Farmer& farmer, Dealer& dealer, const std::string& produceLabel, std::vector<double>& produceValues,double quantity);
		void viewInventory(const std::function<std::vector<std::shared_ptr<Dealer>>()>& readFunction);
		void viewTransactions(const std::function<std::vector<std::shared_ptr<Dealer>>()>& readFunction);
		void searchFarmers(const std::function<std::vector<Farmers::Farmer>()>& readFuction);
		void processFarmersPayments(const std::function<std::vector<std::shared_ptr<Farmers::Farmer>>()>& readFarmers, const std::function<std::vector<std::shared_ptr<Dealer>>()>& readDealers);
		std::string getCurrentTimestamp();
};
#endif

