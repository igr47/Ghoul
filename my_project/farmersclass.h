#ifndef FARMERS_CLASS
#define FARMERS_CLASS
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <memory>
#include <numeric>
#include "agriculture_types.h"
#include "payment_types.h"
#include "login.h"
#include <functional>
using json=nlohmann::json;


class Base{
	private:
		std::string created_at;
		std::string updated_at;
		std::string id;
	public:
		Base();
		std::string getId() const;
		std::string getCreatedAt() const;
		std::string getUpdatedAt() const;
		json toJsonBase() const;
		void fromJsonBase(const json& jsonData);
		void updateTimestamp();
};
class Farmers:public Base{
	public:
		//private members so as to encapaslate
		struct Produce{
			std::string label;
			std::shared_ptr<std::vector<double>> quantities;
			json toJson() const{
				return{
					{"Label",label},
					{"Quantities",*quantities},
					{"Total", std::accumulate(quantities->begin(),quantities->end(),0.0)},
					{"Average", quantities->empty()? 0 : std::accumulate(quantities->begin(),quantities->end(),0.0)/quantities->size()}
				};
			}
			void fromJson(const json& j){
				label=j.value("Label","");
				quantities=std::make_shared<std::vector<double>>();
				for(const auto& val : j.value("Quantities",json::array())){
					if(val.is_number()){
					        quantities->push_back(val);
					}
				}
			}
		};

		struct Farmer{
		        std::string farmersname;
		        std::string farmname;
		        int phonenumber;
		        std::vector<double>farmproduce;
			std::vector<Produce>produce;
		        double totalproduce;
			double averageproduce;
			std::string id;
		        std::string location;
			std::string dealer_id;
		        int age;
			std::vector<FarmerPayment> paymentHistory;
			std::vector<std::shared_ptr<UserCredentials>> user;
			json toJson() const{
                                json e;
                                e["Id"] = id;
                                e["Name"] = farmersname;
                                e["FarmName"] = farmname;
                                e["Location"] = location;
                                e["Year Of Birth"] = age;
                                e["PhoneNumber"] = phonenumber;
                
                                json produceArray = json::array();
                                for (const auto& item : produce) {
                    
                                        produceArray.push_back(item.toJson());
                                }
				json paymentsArray = json::array();
                                for (const auto& payment : paymentHistory){ 

			                paymentsArray.push_back(payment.toJson());
				}
				e["PaymentHistory"] = paymentsArray;
				json loginArray = json::array();
				for(const auto& pass : user){
					loginArray.push_back(pass->toJson());
				}
				e["Account"] = loginArray;
                                e["Produce"] = produceArray;
                                e["My_Dealers_Id"] = dealer_id;
                                e["Overall_Total"] = totalproduce;
                                e["Overall_Average"] = averageproduce;
                
                                return e;
                }
		void fromJson(const json& j) {
                        try {
                                id = j.value("Id", "");
                                farmersname = j.value("Name", "");
                                farmname = j.value("FarmName", "");
                                location = j.value("Location", "");
                                age = j.value("Year Of Birth", 0);
                                phonenumber = j.value("PhoneNumber", 0);
                                dealer_id = j.value("My_Dealers_Id", "");
                                totalproduce = j.value("Overall_Total", 0.0);
                                averageproduce = j.value("Overall_Average", 0.0);

                                // Clear existing produce
                                produce.clear();

                                // Parse produce if it exists
                                if (j.contains("Produce") && j["Produce"].is_array()) {
                                        for (const auto& item : j["Produce"]) {
                                                Produce p;
						p.fromJson(item);
						produce.push_back(p);
                                        }
                                }
				paymentHistory.clear();
                                if (j.contains("PaymentHistory") && j["PaymentHistory"].is_array()) {
                                        for (const auto& item : j["PaymentHistory"]) {
                                                FarmerPayment payment;
                                                payment.fromJson(item);
						paymentHistory.push_back(payment);
                                        }
				}
				user.clear();
				if(j.contains("Account") && j["Account"].is_array()){
						for(const auto& pass : j["Account"]){
						        UserCredentials account;
							account.fromJson(pass);
							user.push_back(account);
						}
				}
                        } catch (const json::exception& e) {
                                std::cerr << "JSON parsing error in fromJson: " << e.what() << "\n";
                                
                                *this = Farmer();
                        }                 
                }
		};
		std::vector<std::shared_ptr<Farmer>>farmerslist;
	public:
		Farmers();
		void getFarmersDetails();
		static std::vector<std::shared_ptr<Farmer>> readFromFile();
		void displayFarmersDetails(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction);
		void searchFarmersDetails(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction,const  std::string& name);
		void updateFarmersDetails(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction);
		void deleteFarmersDetails(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction);
		void addFarmProduce(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction);
		json toJsonFarmers(const std::vector<std::shared_ptr<Farmer>>& farmers) const;
		bool registerUser(const std::string& username,const std::string& email,const std::string& password,const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction);
                bool loginUser(const std::string& username_or_email,const std::string& password,const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction);
		void displayMenu();
		void registerUser();
		void loginUser();
                void LogIn();
		friend class Dealers;

};
#endif
