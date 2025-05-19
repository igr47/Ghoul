#include "farmersclass.h"
#include "dealersclass.h"
#include "uuid.h"
#include "shared_utils.h"
#include "paswordhashing.h"
#include <sodium.h>
#include "login.h"
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <sstream>
#include <chrono>
#include <ctime>
#include <functional>
#include <nlohmann/json.hpp>
#include <vector>
#include <numeric>
#include <tuple>
using json=nlohmann::json;

Base::Base(){
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
        created_at = ss.str(); // Set the creation timestamp
	updated_at=created_at;
}
// I define getters so as to allow aceesss to created at and id
std::string Base::getId() const{return id;}
std::string Base::getCreatedAt() const{return created_at;}
std::string Base::getUpdatedAt() const{return updated_at;}
void Base::updateTimestamp(){
	auto now=std::chrono::system_clock::now();
	auto now_time_t=std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss<<std::put_time(std::localtime(&now_time_t),"%Y-%M-%d %H:%M:%S");
	updated_at=ss.str();
}
json Base::toJsonBase() const{
	json jsonData;
	jsonData["Created_At"]=created_at;
	jsonData["Updated_At"]=updated_at;
	return jsonData;
}
void Base::fromJsonBase(const json& jsonData){
	created_at=jsonData["Created_At"];
	updated_at=jsonData["Updated_At"];
}
//the farmers default constructor

Farmers::Farmers(){}
json Farmers::toJsonFarmers(const std::vector<std::shared_ptr<Farmer>>& farmers) const{
	json farmersArray=json::array();
	for(const auto& farmer : farmerslist){
		json farmersJson=toJsonBase();
		farmersJson.update(farmer->toJson());
		farmersArray.push_back(farmersJson);
	}
	return farmersArray;
}
std::shared_ptr<Farmers::Farmer> Farmers::currentFarmer=nullptr;
std::shared_ptr<Farmers::Farmer> Farmers::getCurrentFarmer(){return currentFarmer;}
void Farmers::getFarmersDetails(){
	farmerslist=readFromFile();
	int num;
	std::cout<<"Enter the number of farmers you wish to save:";
	std::cout<<"\n";
	std::cin>>num;
	std::cin.ignore();
	for(int i=0;i<num;++i){
		auto farmer=std::make_shared<Farmer>();
		std::cout<<"*********************************************************************\n";
		std::cout<<"Enter details for farmer"<<i+1<<":\n";

		std::cout<<"Name:\n";
		std::getline(std::cin,farmer->farmersname);
		std::cout<<"Location:\n";
		std::getline(std::cin,farmer->location);
		std::cout<<"FarmName:\n";
		std::getline(std::cin,farmer->farmname);
		std::cout<<"Year Of Birth:\n";
		std::cin>>farmer->age;
		std::cin.ignore();
		std::cout<<"PhoneNumber:\n";
		std::cin>>farmer->phonenumber;
		std::cin.ignore();
		farmer->id=generateUUID();
		farmerslist.push_back(farmer);
		std::cout<<"*****************************************************************************\n";
	}
	//we dafine a lambda function  which will convert the  data to json format
	json farmersJson=toJsonFarmers(farmerslist);
	writeToFile("farmers.json",farmersJson);
	std::cout<<"YOur details were written successively!\n";
	
}

    // this method is static so it outlives the class.
    // it's function is to parse details from files 
    // so search update delete display and create relationships will all use the class.
    // the methods will reduce runtime greately since they are inline and do not branc
std::vector<std::shared_ptr<Farmers::Farmer>> Farmers::readFromFile() {
    std::vector<std::shared_ptr<Farmer>> farmers;
    std::ifstream file("farmers.json");
    
    if (file.is_open()) {
        try {
            json farmersJson;
            file >> farmersJson;
            
            if (farmersJson.is_array()) {
                for (const auto& item : farmersJson) {
                    auto farmer=std::make_shared<Farmer>();
                    farmer->fromJson(item);
                    farmers.push_back(farmer);
                }
                std::cout << "Farmers data parsed successfully.\n";
            } else {
                std::cerr << "Invalid JSON format: expected array of farmers\n";
            }
        } catch (const json::exception& e) {
            std::cerr << "JSON parsing error: " << e.what() << "\n";
        }
        file.close();
    } else {
        std::cerr << "Error opening farmers.json! Creating new file.\n";
        // Create an empty file if it doesn't exist
        std::ofstream newFile("farmers.json");
        newFile << "[]";
        newFile.close();
    }
    
    return farmers;
}
void Farmers::displayFarmersDetails(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction) {
    try {
        auto farmers = readFunction();
        std::cout << "The current saved farmers are:\n";
        
        for (const auto& farmer : farmers) {
            std::cout << "=============================================\n";
            std::cout << "Name: " << farmer->farmersname << "\n"
                      << "Farmname: " << farmer->farmname << "\n"
                      << "Location: " << farmer->location << "\n"
                      << "Year Of Birth: " << farmer->age << "\n"
                      << "PhoneNumber: " << farmer->phonenumber << "\n"
                      << "Id: " << farmer->id << "\n";
            
            // Display produce information if available
            if (!farmer->produce.empty()) {
                std::cout << "Produce:\n";
                for (const auto& produce : farmer->produce) {
                    std::cout << "  " << produce.label << ": ";
                    for (double value : *produce.quantities) {
                        std::cout << value << " ";
                    }
                    std::cout << "\n";
                }
                std::cout << "Total Produce: " << farmer->totalproduce << "\n"
                          << "Average Produce: " << farmer->averageproduce << "\n";
            }
            
            if (farmer->dealer_id.empty()) {
                std::cout << "Dealer: None\n";
            }else{
		    auto dealers=Dealers::readFromFile();
		    auto dealerIt=std::find_if(dealers.begin(),dealers.end(),[&](const auto& d){return d->dealer_id==farmer->dealer_id;});
		    if(dealerIt !=dealers.end()){
			    std::cout<<"Dealer: " << (*dealerIt)->dealersname<<"\n";
		    }else{
			std::cout<<"Dealer: [ID: "<<farmer->dealer_id<< "- Not Found]\n";
		   }
	    }

            
            std::cout << "=============================================\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error displaying farmers details: " << e.what() << "\n";
    }
}


void Farmers::searchFarmersDetails(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction,const std::string& name){
	auto farmers=readFunction();
	auto it=std::find_if(farmers.begin(),farmers.end(),[&name](const std::shared_ptr<Farmer>& farmer){
			return farmer->farmersname==name;
		});
	if(it !=farmers.end()){
		std::cout<<"========================================================\n";

		std::cout<<"Farmer found!\n";
		std::cout<<"Name: "<<(*it)->farmersname<<"\n";
		std::cout<<"FarmName: "<<(*it)->farmname<<"\n";
		std::cout<<"Location: "<<(*it)->location<<"\n";
		std::cout<<"Year Of Birth: "<<(*it)->age<<"\n";
		std::cout<<"PhoneNumber"<<(*it)->phonenumber<<"\n";
		std::cout<<"Id:"<<(*it)->id<<"\n";
		std::cout<<"Created_At:"<<getCreatedAt()<<"\n";
		std::cout<<"Updated_At:"<<getUpdatedAt()<<"\n";
		std::cout<<"=======================================================\n";
	}else{
		std::cout<<"The farmer with name "<<name<<"was not found!\n";
	}
}
void Farmers::updateFarmersDetails(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction){
	auto farmerslist=readFunction();
	std::string name;
	std::cout<<"Enter name of the farmer you wish to update!\n";
	std::cin.ignore();
	std::getline(std::cin,name);
	auto it=std::find_if(farmerslist.begin(),farmerslist.end(),[&name](const std::shared_ptr<Farmer>& farmer){
			return farmer->farmersname==name;
		});
	if(it !=farmerslist.end()){
		std::cout<<"Enter new details for farmer:"<<(*it)->farmersname<<"\n";
		std::cout<<"Location:\n";
		std::getline(std::cin,(*it)->location);
		std::cout<<"FarmName:\n";
		std::getline(std::cin,(*it)->farmname);
		std::cout<<"Year Of Birth:\n";
		std::cin>>(*it)->age;
		std::cin.ignore();
		std::cout<<"PhoneNumber:\n";
		std::cin>>(*it)->phonenumber;
		std::cin.ignore();
		updateTimestamp();
		
		json farmersJson=toJsonFarmers(farmerslist);
		writeToFile("farmers.json",farmersJson);
		std::cout<<"Farmers details with name "<<name<< " successively deleated!\n";

	}else{
		std::cout<<"Farmer with name "<<name<<" was nat found\n";
	}
}
void Farmers::deleteFarmersDetails(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction){
	farmerslist=readFunction();
	std::string name;
	std::cout<<"Enter the name of the farmer you wish to delete!\n";
	std::cin.ignore();
	std::getline(std::cin,name);
	auto it=std::remove_if(farmerslist.begin(),farmerslist.end(),[&name](const std::shared_ptr<Farmer>& farmer){
			return farmer->farmersname==name;
		});
	if(it !=farmerslist.end()){
		farmerslist.erase(it,farmerslist.end());
		std::cout<<"Farmer "<<name<<" deleted!\n";
		updateTimestamp();
		json farmersJson=toJsonFarmers(farmerslist);
		writeToFile("farmers.json",farmersJson);
	}else{
		std::cout<<"Farmer with name "<<name<<" was naot found\n";
	}
}
void Farmers::addFarmProduce(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction) {
    if(!currentFarmer){
	    std::cout<<"No farmer is currently logged in\n";
	    return;
	}

    farmerslist = readFunction();
    
    
    auto it = std::find_if(farmerslist.begin(), farmerslist.end(), 
        [](const std::shared_ptr<Farmer>& farmer) { 
            return farmer->id == currentFarmer->id; 
        });
    
    if(it == farmerslist.end()) {
        std::cout << "LoggedIn farmer not found in the database.\n";
        return;
    }
    
    int dealerinput_option;
    do{
        std::cout << "Would you like to add/change a farmproduce dealer?\n"
                << "1.Yes/2.No\n";
        std::cin >> dealerinput_option;
        std::cin.ignore();
    
        if(dealerinput_option == 1) {
                auto dealers = Dealers::readFromFile();
                std::cout << "Available Dealers:\n";
                for(const auto& dealer : dealers) {
                        std::cout << "| ID: " << dealer->dealer_id
                                << "| Name: " << dealer->dealersname
                                << "| Cooperation: " << dealer->cooperation << "\n";
                }
		DealerInfo n;
		std::string newDealersId,newDealersName;
		std::cout<<"\nEnter the dealer's id you wish to be your buyer: ";
		std::getline(std::cin,newDealersId);
		std::cout<<"\nEnter the dealers name you wish to be your buyer: ";
		std::getline(std::cin,newDealersName);
		n.dealerName=newDealersName;
		n.dealer_id=newDealersId;
        
                auto dealersIt = std::find_if(dealers.begin(), dealers.end(),
                                [&newDealersId,&newDealersId](const std::shared_ptr<Dealers::Dealer>& dealer) {
                                                return dealer->dealer_id == newDealersId &&  dealer->dealersname==newDealersName;
                        });
        
                if(dealersIt != dealers.end()) {
                        (*it)->dealerInfo.push_back(n);
                        std::cout << "Dealer assigned successively!\n";
                } else {
                        std::cout << "\nInvalid dealer ID. No dealer assigned. ";
                }
		std::cout<<"\nWould you like to add another dealer (1 for Yes && 2 for No)? ";
		std::cin>>dealerinput_option;
		std::cin.ignore();
        }else{
		return;
	}
    }while(dealerinput_option ==1);
    
    int num_of_produce;
    std::cout << "Enter the number of produce categories you want to add: ";
    std::cin >> num_of_produce;
    std::cin.ignore();
    
    for(int i = 0; i < num_of_produce; ++i) {
        std::string label;
        std::cout << "Enter the label for produce #" << i+1 
                  << " (e.g., corn, wheat, milk, apples): ";
        std::getline(std::cin, label);
        
        // Check if this produce label already exists
        bool found = false;
        for (auto& produce : (*it)->produce) {
            if (produce.label == label) {
                found = true;
                int numvalues;
                std::cout << "Enter the number of values to add to " << label << ": ";
                std::cin >> numvalues;
                std::cin.ignore();
                
                for(int j = 0; j < numvalues; ++j) {
                    double value;
                    std::cout << "Enter value " << j+1 << " for " << label << ": ";
                    std::cin >> value;
                    std::cin.ignore();
                    produce.quantities->push_back(value);
                }
                break;
            }
        }
        
        if (!found) {
            // New produce type
            Produce newProduce;
            newProduce.label=label;
		    newProduce.quantities=std::make_shared<std::vector<double>>();
		    int numvalues;
		    std::cout << "Enter the number of values for " << label << ": ";
		    std::cin >> numvalues;
		    std::cin.ignore();
		    
		    for(int j = 0; j < numvalues; ++j) {
			double value;
			std::cout << "Enter value " << j+1 << " for " << label << ": ";
			std::cin >> value;
			std::cin.ignore();
			newProduce.quantities->push_back(value);
		    }
		    
		    (*it)->produce.push_back(newProduce);
		}
	    }
	    
	    // Recalculate overall statistics
	    (*it)->totalproduce = 0.0;
	    int totalValueCount = 0;
	    
	    for (const auto& produce : (*it)->produce) {
		double produceTotal = std::accumulate(produce.quantities->begin(), produce.quantities->end(), 0.0);
		(*it)->totalproduce += produceTotal;
		totalValueCount += produce.quantities->size();
	    }
	    
	    (*it)->averageproduce = (totalValueCount > 0) ? ((*it)->totalproduce / totalValueCount) : 0.0;
	    
	    // Display updated statistics
	    std::cout << "\nUpdated Produce Summary for " << (*it)->farmersname << ":\n";
	    std::cout << "----------------------------------------\n";
	    
	    for (const auto&  produce : (*it)->produce) {
		double total = std::accumulate(produce.quantities->begin(), produce.quantities->end(), 0.0);
		double average = produce.quantities->empty() ? 0 : total / produce.quantities->size();
		
		std::cout << produce.label << ":\n";
		std::cout << "  Values: ";
		for (double val : *produce.quantities) {
		    std::cout << val << " ";
		}
		std::cout << "\n";
        std::cout << "  Total: " << total << "\n";
        std::cout << "  Average: " << average << "\n\n";
    }
    
    std::cout << "Overall Statistics:\n";
    std::cout << "  Total Produce: " << (*it)->totalproduce << "\n";
    std::cout << "  Average Produce: " << (*it)->averageproduce << "\n";
    
    // Save all farmers' data back to file
    json farmersJson = toJsonFarmers(farmerslist);
    writeToFile("farmers.json", farmersJson);
    
    std::cout << "\nAll produce data successfully saved!\n";
}
bool Farmers::registerUser(const std::string& username,const std::string& email,const std::string& password,const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction){ 
	auto farmers=readFunction();                            
	for(const auto& farmer : farmers){                                       
		if(std::any_of(farmer->user.begin(),farmer->user.end(),[&username,&email](const auto& u){
					return u->username==username || u->email==email;
					})){ 
			return false;
                        std::string hashed_pw=argon2_hash(password);
                        if(hashed_pw.empty()){                                                 
				return false;      
		        }                                        
			auto new_user=std::make_shared<UserCredentials>();    
			new_user->username=username;
                        new_user->email=email;
                        new_user->hashed_password=hashed_pw;
			farmer->user.push_back(new_user);                                
			json farmersJson=toJsonFarmers(farmers);
                        writeToFile("farmers.json",farmersJson);
                }
	}
}
bool Farmers::loginUser(const std::string& username_or_email,const std::string& password,const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction){
         auto farmers=readFunction();
         for(const auto& farmer : farmers){
                 auto userIt=std::find_if(farmer->user.begin(),farmer->user.end(),[&username_or_email](const auto& u){
                                 return u->username==username_or_email || u->email==username_or_email;
                                });
                 if(userIt !=farmer->user.end() && verify_password((*userIt)->hashed_password,password)){
			 currentFarmer=farmer;
                         return true;
                }
        }
	return false;
}
void Farmers::displayMenu(){
        std::cout<<"\n=======Aunthentication System=========="
                <<"\n1.Register:" 
		<<"\n2.Login:"                                                 
		<<"\n3.Exit:"
                <<"\nChoise";
}
void Farmers::registerUser(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction){
	auto farmerslist=readFunction();
	auto farmer=std::make_shared<Farmer>();
        std::string username,password,email;
        std::cout<<"\n=======Registration=======";
	std::cout<<"\nUsername: ";
        std::getline(std::cin,username);
	farmer->current_user=username;
        std::cout<<"\nEmail: ";
        std::getline(std::cin,email);
        std::cout<<"\nPassword: ";
        std::getline(std::cin,password);
	json farmerJson=toJsonFarmers(farmerslist);
	writeToFile("farmer.json",farmerJson);
        if(registerUser(username,email,password,readFromFile)){
                std::cout<<"Registration successfull.\n";
        }else{
                std::cout<<"\nRegistration failed(username or email alredy exists)";
        }
}                                                               
void Farmers::loginUser(){
        std::string username_or_email,password;
        std::cout<<"\n======LogIn=======";
        std::cout<<"\nUsername or Email: ";
        std::getline(std::cin,username_or_email);
        std::cout<<"\nPassword: ";
        std::getline(std::cin,password);    
	if(loginUser(username_or_email,password,readFromFile)){ 
		std::cout<<"\nLogin successfull.";                       
	}else{                                                                   
		std::cout<<"\nInvalid credentials!!!!!";                 
	}                                                        
}                                                                
void Farmers::LogIn(){
	int opt;
	displayMenu();
	std::cin>>opt;
        switch(opt){
                case 1:
                        registerUser(readFromFile);                                                  
			break;                                                   
		case 2:                         
			loginUser();              
			break;                   
		case 3:
                        break;
                default:      
			std::cout<<"\nPlease enter valid option.";
                        break;
        }
}		
std::vector<Notifications> Farmers::getUnreadNotifications(const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction){
	auto farmers=readFunction();
	auto farmer=std::make_shared<Farmer>();
	std::vector<Notifications> unread;
	for(auto& notif : farmer->notifications){
		if(notif.receiver==farmer->current_user && !notif.is_read){
			unread.push_back(notif);
		}
	}
	return unread;
}
bool Farmers::markAsRead(const std::string& notification_id,const std::function<std::vector<std::shared_ptr<Farmer>>()>& readFunction){
	auto farmers=readFunction();
	auto farmer=std::make_shared<Farmer>();
	for(auto& notif : farmer->notifications){
		if(notif.id==notification_id){
			notif.is_read=true;
			json farmerJson=toJsonFarmers(farmers);
			writeToFile("farmers.json",farmerJson);
			return true;
		}
	}
	return false;
}
void Farmers::viewNotifications(){
	auto unread=getUnreadNotifications(readFromFile);
	if(unread.empty()){
		std::cout<<"\nNo notifications!!!";
	}else{
		std::cout<<"\nUnread notifications:";
		for(size_t i=0;i<unread.size();++i){
			std::cout<<i+1<<"\nFrom: "<<unread[i].sender
				<<"\nMeaasge: "<<unread[i].message
				<<"\nTime: "<<std::put_time(std::localtime(&unread[i].timestamp), "%Y-%m-%d %H:%M:%S")
				<<"\n";
		}

		std::cout<<"\nEnter number to veiw(0 to cancle):";
		int notif_choise;
		std::cin>>notif_choise;
		std::cin.ignore();
		if(notif_choise>0 && notif_choise<unread.size()){
			markAsRead(unread[notif_choise-1].id,readFromFile);
		}
	}
}
void Farmers::checkNotifications(){
	auto unread=getUnreadNotifications(readFromFile);	
	if(!unread.empty()){
		std::cout<<"\nYou have "<<unread.size()<< "new notification(s)!!";
	}
}

	




		
