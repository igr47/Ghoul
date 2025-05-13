#include "farmersclass.h"
#include "uuid.h"
#include "dealersclass.h"
#include "paswordhashing.h"
#include <sodium.h>
#include "shared_utils.h"
#include "payment_types.h"
#include "notifications.h"
#include <ranges>
#include <iostream>
#include <string>
#include <fstream>
#include <numeric>
#include <utility>
#include <memory>
#include <set>
#include <map>
#include <iterator>
#include <cctype>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <functional>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

// Default constructor
Dealers::Dealers() {}


// JSON serialization for dealers
json Dealers::toJsonDealers(std::vector<std::shared_ptr<Dealer>>& dealers) const {
    json dealersArray = json::array();
    for (const auto& dealer : dealers) {
        json dealerJson = dealer->toJson();
        dealersArray.push_back(dealerJson);
    }
    return dealersArray;
}

std::string Dealers::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Dealers::getDealersDetails() {
    dealerslist = readFromFile();
    int num;
    std::cout << "Enter the number of dealers you wish to save: ";
    std::cin >> num;
    std::cin.ignore();

    for (int i = 0; i < num; ++i) {  // Changed from <= to < to match input count
        auto dealer = std::make_shared<Dealer>();
        std::cout << "*********************************************************************\n";
        std::cout << "Enter details for dealer " << (i + 1) << ":\n";
        std::cout << "Name:\n";
        std::getline(std::cin, dealer->dealersname);
        std::cout << "Location:\n";
        std::getline(std::cin, dealer->location);
        std::cout << "Cooperation:\n";
        std::getline(std::cin, dealer->cooperation);
        std::cout << "Year of Birth:\n";
        std::cin >> dealer->age;
        std::cout << "Phone Number:\n";
        std::cin >> dealer->phonenumber;
        std::cin.ignore();
        dealer->dealer_id = generateUUID();  // Assuming generateUUID() is defined in uuid.h
        dealerslist.push_back(dealer);
        std::cout << "*********************************************************************\n";
    }

    json dealersJson = toJsonDealers(dealerslist);
    writeToFile("dealers.json", dealersJson);  // Assuming writeToFile is defined in shared_utils.h
}

std::vector<std::shared_ptr<Dealers::Dealer>> Dealers::readFromFile() {
    std::vector<std::shared_ptr<Dealer>> dealers;
    std::ifstream file("dealers.json");
    if (file.is_open()) {
        try {
            json dealersJson;
            file >> dealersJson;
            if (dealersJson.is_array()) {
                for (const auto& item : dealersJson) {
                    auto dealer = std::make_shared<Dealer>();
                    dealer->fromJson(item);
                    dealers.push_back(dealer);
                }
                std::cout << "Dealers data parsed successfully.\n";
            } else {
                std::cerr << "Invalid JSON format: expected array of dealers\n";
                std::ofstream newFile("dealers.json");
                newFile << "[]";
                newFile.close();
            }
        } catch (const json::exception& e) {
            std::cerr << "JSON parsing error: " << e.what() << "\n";
            std::ofstream newFile("dealers.json");
            newFile << "[]";
            newFile.close();
        }
        file.close();
    } else {
        std::cerr << "dealers.json not found. Creating new file.\n";
        std::ofstream newFile("dealers.json");
        newFile << "[]";
        newFile.close();
    }
    return dealers;
}

void Dealers::displayDealersDetails(const std::function<std::vector<std::shared_ptr<Dealer>>()> &readFunction) {
    auto dealers = readFunction();
    std::cout << "The current saved dealers are:\n";
    for (const auto& dealer : dealers) {
        std::cout << "=============================================\n";
        std::cout << "Name: " << dealer->dealersname << "\n"
                  << "Cooperation: " << dealer->cooperation << "\n"
                  << "Location: " << dealer->location << "\n"
                  << "Year of Birth: " << dealer->age << "\n"
                  << "Phone Number: " << dealer->phonenumber << "\n"
                  << "ID: " << dealer->dealer_id << "\n";
        // Note: getCreatedAt() and getUpdatedAt() need to be implemented in Base class
        std::cout << "=============================================\n";
    }
}

void Dealers::searchDealersDetails(const std::function<std::vector<std::shared_ptr<Dealer>>()> &readFunction, const std::string &name) {
    auto dealers = readFunction();
    auto it = std::find_if(dealers.begin(), dealers.end(),
                          [&name](const std::shared_ptr<Dealer>& dealer) {
                              return dealer->dealersname == name;
                          });
    if (it != dealers.end()) {
        std::cout << "=============================================\n";
        std::cout << "Dealer found!\n";
        std::cout << "Name: " << (*it)->dealersname << "\n"
                  << "Cooperation: " << (*it)->cooperation << "\n"
                  << "Location: " << (*it)->location << "\n"
                  << "Year of Birth: " << (*it)->age << "\n"
                  << "Phone Number: " << (*it)->phonenumber << "\n"
                  << "ID: " << (*it)->dealer_id << "\n";
        std::cout << "=============================================\n";
    } else {
        std::cout << "The dealer with name " << name << " was not found!\n";
    }
}

void Dealers::updateDealersDetails(const std::function<std::vector<std::shared_ptr<Dealer>>()> &readFunction) {
    dealerslist = readFunction();
    std::string name;
    std::cout << "Enter name of the dealer you wish to update!\n";
    std::cin.ignore();
    std::getline(std::cin, name);
    auto it = std::find_if(dealerslist.begin(), dealerslist.end(),
                          [&name](const std::shared_ptr<Dealer>& dealer) {
                              return dealer->dealersname == name;
                          });
    if (it != dealerslist.end()) {
        std::cout << "Enter new details for dealer " << (*it)->dealersname << ":\n";
        std::cout << "Location:\n";
        std::getline(std::cin, (*it)->location);
        std::cout << "Cooperation:\n";
        std::getline(std::cin, (*it)->cooperation);
        std::cout << "Year of Birth:\n";
        std::cin >> (*it)->age;
        std::cin.ignore();
        std::cout << "Phone Number:\n";
        std::cin >> (*it)->phonenumber;
        std::cin.ignore();

        json dealersJson = toJsonDealers(dealerslist);
        writeToFile("dealers.json", dealersJson);
        std::cout << "Dealer details with name " << name << " were successfully updated!\n";
    } else {
        std::cout << "Dealer with name " << name << " was not found\n";
    }
}

void Dealers::deleteDealersDetails(const std::function<std::vector<std::shared_ptr<Dealer>>()> &readFunction) {
    dealerslist = readFunction();
    std::string name;
    std::cout << "Enter the name of the dealer you wish to delete!\n";
    std::cin.ignore();
    std::getline(std::cin, name);
    auto it = std::remove_if(dealerslist.begin(), dealerslist.end(),
                            [&name](const std::shared_ptr<Dealer>& dealer) {
                                return dealer->dealersname == name;
                            });
    if (it != dealerslist.end()) {
        dealerslist.erase(it, dealerslist.end());
        json dealersJson = toJsonDealers(dealerslist);
        writeToFile("dealers.json", dealersJson);
        std::cout << "Dealer with name " << name << " was successfully deleted!\n";
    } else {
        std::cout << "Dealer with name " << name << " was not found\n";
    }
}

void Dealers::viewAvailableProduce(const std::function<std::vector<std::shared_ptr<Farmers::Farmer>>()> &readFunction) {
    try {
        std::string name, dealerId;
        std::cout << "Enter your dealer name: ";
        std::cin.ignore();
        std::getline(std::cin, name);
        std::cout << "Enter your dealer id code: \n";
        std::getline(std::cin, dealerId);

        auto dealers = readFromFile();
        auto it = std::find_if(dealers.begin(), dealers.end(),
                              [&name](const std::shared_ptr<Dealer>& dealer) {
                                  return dealer->dealersname == name;
                              });
        if (it == dealers.end()) {
            std::cout << "Dealer with name " << name << " was not found!\n";
            return;
        }

        auto farmers = readFunction();
        std::vector<std::shared_ptr<Farmers::Farmer>> associatedFarmers;
        std::copy_if(farmers.begin(), farmers.end(), std::back_inserter(associatedFarmers),
                    [&dealerId](const std::shared_ptr<Farmers::Farmer>& farmer) {
                        return farmer->dealer_id == dealerId;
                    });

        if (associatedFarmers.empty()) {
            std::cout << "No farmers are currently associated with your dealership.\n";
            return;
        }

        std::cout << "\nAvailable produce from your farmers:\n";
        for (const auto& farmer : associatedFarmers) {
            std::cout << "=============================================\n";
            std::cout << "Farmer: " << farmer->farmersname << "\n";
            std::cout << "Farm Name: " << farmer->farmname << "\n";
            std::cout << "Location: " << farmer->location << "\n";
            if (!farmer->produce.empty()) {
                std::cout << "Available Produce:\n";
                for (const auto& produce : farmer->produce) {
                    double total = std::accumulate(produce.quantities->begin(), produce.quantities->end(), 0.0);
                    std::cout << "  " << produce.label << ": " << total << " units available\n";
                }
            } else {
                std::cout << "No produce available from this farmer.\n";
            }
            std::cout << "=============================================\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error viewing available produce: " << e.what() << "\n";
    }
}

void Dealers::purchaseFarmersProduce(const std::function<std::vector<std::shared_ptr<Farmers::Farmer>>()> &readFunction,
                                    const std::function<std::vector<std::shared_ptr<Dealer>>()> &readFunctions) {
    try {
        std::string dealerId;
        std::cout << "Enter dealer ID: ";
        std::cin.ignore();
        std::getline(std::cin, dealerId);

        auto dealers = readFunctions();
        auto dealerIt = std::find_if(dealers.begin(), dealers.end(),
                                    [&dealerId](const std::shared_ptr<Dealer>& dealer) {
                                        return dealer->dealer_id == dealerId;
                                    });
        if (dealerIt == dealers.end()) {
            std::cout << "Dealer with ID '" << dealerId << "' not found.\n";
            return;
        }

        std::string farmerId;
        std::cout << "Enter the ID of the farmer you wish to buy from: ";
        std::getline(std::cin, farmerId);

        auto farmers = readFunction();
        auto farmerIt = std::find_if(farmers.begin(), farmers.end(),
                                    [&farmerId, &dealerId](const std::shared_ptr<Farmers::Farmer>& farmer) {
                                        return farmer->id == farmerId && farmer->dealer_id == dealerId;
                                    });
        if (farmerIt == farmers.end()) {
            std::cout << "Farmer not found or not registered under your dealership!\n";
            return;
        }

        if ((*farmerIt)->produce.empty()) {
            std::cout << "This farmer has no produce available.\n";
            return;
        }

        std::cout << "\nAvailable produce from " << (*farmerIt)->farmersname << ":\n";
        std::cout << "----------------------------------------\n";
        for (size_t i = 0; i < (*farmerIt)->produce.size(); ++i) {
            const auto& produce = (*farmerIt)->produce[i];
            double total = std::accumulate(produce.quantities->begin(), produce.quantities->end(), 0.0);
            std::cout << i + 1 << ". " << produce.label << " (" << total << " units available)\n";
        }
        std::cout << "----------------------------------------\n";

        int produceChoice;
        std::cout << "Enter the number of the produce you want to purchase (0 to cancel): ";
        std::cin >> produceChoice;
        std::cin.ignore();

        if (produceChoice == 0) {
            std::cout << "Purchase cancelled.\n";
            return;
        }

        if (produceChoice < 1 || produceChoice > (*farmerIt)->produce.size()) {
            std::cout << "Invalid produce selection.\n";
            return;
        }

        auto& selectedProduce = (*farmerIt)->produce[produceChoice - 1];
        double availableQuantity = std::accumulate(selectedProduce.quantities->begin(), selectedProduce.quantities->end(), 0.0);

        double quantity;
        std::cout << "Enter the quantity of " << selectedProduce.label << " you wish to purchase (max " << availableQuantity << "): ";
        std::cin >> quantity;
        std::cin.ignore();

        if (quantity <= 0) {
            std::cout << "Quantity must be positive.\n";
            return;
        }

        if (quantity > availableQuantity) {
            std::cout << "Insufficient " << selectedProduce.label << " available! Only " << availableQuantity << " units available.\n";
            return;
        }

        processProducePurchase(**farmerIt, **dealerIt, selectedProduce.label, *selectedProduce.quantities, quantity);

        json farmersJson = Farmers::toJsonFarmers(farmers);  // Assuming this static method exists in Farmers
        writeToFile("farmers.json", farmersJson);
        json dealersJson = toJsonDealers(dealers);
        writeToFile("dealers.json", dealersJson);

        std::cout << "\nPurchase successful!\n";
        std::cout << "Purchased " << quantity << " units of " << selectedProduce.label << " from " << (*farmerIt)->farmersname << "\n";
        std::cout << "Remaining " << selectedProduce.label << ": " << (availableQuantity - quantity) << " units\n";
    } catch (const std::exception& e) {
        std::cerr << "Error during purchase: " << e.what() << "\n";
    }
}

void Dealers::processProducePurchase(Farmers::Farmer& farmer, Dealer& dealer, const std::string& produceLabel,
                                    std::vector<double>& produceValues, double quantity) {
    double remaining = quantity;
    std::sort(produceValues.begin(), produceValues.end(), std::greater<double>());
    for (auto& val : produceValues) {
        if (remaining <= 0) break;
        if (val >= remaining) {
            val -= remaining;
            remaining = 0;
        } else {
            remaining -= val;
            val = 0;
        }
    }
    produceValues.erase(std::remove_if(produceValues.begin(), produceValues.end(),
                                       [](double val) { return val == 0; }), produceValues.end());

    dealer.inventory.push_back(Inventory{produceLabel,farmer.farmersname, std::make_unique<std::vector<double>>(1, quantity)});

    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream date_ss;
    date_ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");

    dealer.transactions.emplace_back(date_ss.str(),
                                    std::make_shared<std::string>(farmer.farmersname),
                                    std::make_shared<std::string>(farmer.farmname),
                                    quantity);
}

void Dealers::viewInventory(const std::function<std::vector<std::shared_ptr<Dealer>>()> &readFunction) {
    try {
        auto dealers = readFunction();
        std::string dealerName;
        std::cout << "Enter dealer name to view inventory: ";
        std::getline(std::cin, dealerName);

        auto it = std::find_if(dealers.begin(), dealers.end(),
                              [&dealerName](const std::shared_ptr<Dealer>& dealer) {
                                  return dealer->dealersname == dealerName;
                              });
        if (it == dealers.end()) {
            std::cout << "Dealer not found!\n";
            return;
        }

        std::cout << "\n=== Inventory for " << (*it)->dealersname << " ===\n";
        std::cout << "----------------------------------------\n";
        if ((*it)->inventory.empty()) {
            std::cout << "Inventory is empty.\n";
        } else {
            std::cout << std::left << std::setw(20) << "Item" << std::setw(15) << "Quantity" << "\n";
            std::cout << "----------------------------------------\n";
            for (const auto& item : (*it)->inventory) {
                double total = std::accumulate(item.quantity->begin(), item.quantity->end(), 0.0);
                std::cout << std::left << std::setw(20) << item.item << std::setw(15) << total << "\n";
            }
            double totalQuantity = 0.0;
            for (const auto& item : (*it)->inventory) {
                totalQuantity += std::accumulate(item.quantity->begin(), item.quantity->end(), 0.0);
            }
            std::cout << "----------------------------------------\n";
            std::cout << std::left << std::setw(20) << "TOTAL:" << std::setw(15) << totalQuantity << "\n";
        }
        std::cout << "========================================\n";
    } catch (const std::exception& e) {
        std::cerr << "Error viewing inventory: " << e.what() << "\n";
    }
}

void Dealers::viewTransactions(const std::function<std::vector<std::shared_ptr<Dealer>>()> &readFunction) {
    try {
        auto dealers = readFunction();
        std::string dealerName;
        std::cout << "\nEnter dealer name to view transactions: ";
        std::getline(std::cin, dealerName);

        auto it = std::find_if(dealers.begin(), dealers.end(),
                              [&dealerName](const std::shared_ptr<Dealer>& dealer) {
                                  return dealer->dealersname == dealerName;
                              });
        if (it == dealers.end()) {
            std::cout << "Dealer not found!\n";
            return;
        }

        std::cout << "\n=== Transaction History for " << (*it)->dealersname << " ===\n";
        std::cout << "------------------------------------------------------------\n";
        if ((*it)->transactions.empty()) {
            std::cout << "No transactions recorded.\n";
        } else {
            std::cout << std::left
                      << std::setw(20) << "Date"
                      << std::setw(20) << "Farmer"
                      << std::setw(20) << "Farm"
                      << std::setw(15) << "Amount" << "\n";
            std::cout << "------------------------------------------------------------\n";
            for (const auto& txn : (*it)->transactions) {
                std::cout << std::left
                          << std::setw(20) << txn.date
                          << std::setw(20) << (txn.farmer ? *txn.farmer : "")
                          << std::setw(20) << (txn.farm ? *txn.farm : "")
                          << std::setw(15) << (txn.amount ? *txn.amount : 0.0) << "\n";
            }
            double totalAmount = 0.0;
            for (const auto& txn : (*it)->transactions) {
                totalAmount += txn.amount ? *txn.amount : 0.0;
            }
            std::cout << "------------------------------------------------------------\n";
            std::cout << std::left << std::setw(60) << "TOTAL:" << std::setw(15) << totalAmount << "\n";
        }
        std::cout << "============================================================\n";
    } catch (const std::exception& e) {
        std::cerr << "Error viewing transactions: " << e.what() << "\n";
    }
}

void Dealers::searchFarmers(const std::function<std::vector<Farmers::Farmer>()> &readFunction) {
    try {
        auto farmers = readFunction();
        int searchOption;
        std::cout << "\n=== Farmer Search Options ===\n";
        std::cout << "1. Search by name\n";
        std::cout << "2. Search by produce type\n";
        std::cout << "3. Search by location\n";
        std::cout << "Enter your choice (1-3): ";
        std::cin >> searchOption;
        std::cin.ignore();

        std::string searchTerm;
        std::cout << "Enter search term: ";
        std::getline(std::cin, searchTerm);

        std::transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(),
                      [](unsigned char c) { return std::tolower(c); });

        bool found = false;
        for (const auto& farmer : farmers) {
            bool match = false;
            switch (searchOption) {
                case 1: {
                    std::string farmerName = farmer.farmersname;
                    std::transform(farmerName.begin(), farmerName.end(), farmerName.begin(),
                                  [](unsigned char c) { return std::tolower(c); });
                    match = farmerName.find(searchTerm) != std::string::npos;
                    break;
                }
                case 2: {
                    for (const auto& produce : farmer.produce) {
                        std::string label = produce.label;
                        std::transform(label.begin(), label.end(), label.begin(),
                                      [](unsigned char c) { return std::tolower(c); });
                        if (label.find(searchTerm) != std::string::npos) {
                            match = true;
                            break;
                        }
                    }
                    break;
                }
                case 3: {
                    std::string location = farmer.location;
                    std::transform(location.begin(), location.end(), location.begin(),
                                  [](unsigned char c) { return std::tolower(c); });
                    match = location.find(searchTerm) != std::string::npos;
                    break;
                }
                default:
                    std::cout << "Invalid search option!\n";
                    return;
            }
            if (match) {
                if (!found) {
                    std::cout << "\n=== Search Results ===\n";
                    std::cout << "----------------------------------------\n";
                    found = true;
                }
                std::cout << "Name: " << farmer.farmersname << "\n";
                std::cout << "Farm: " << farmer.farmname << "\n";
                std::cout << "Location: " << farmer.location << "\n";
                std::cout << "Produce: ";
                bool firstProduce = true;
                for (const auto& produce : farmer.produce) {
                    if (!firstProduce) std::cout << ", ";
                    std::cout << produce.label << " (" << produce.quantities->size() << " entries)";
                    firstProduce = false;
                }
                std::cout << "\nDealer: " << (farmer.dealer_id.empty() ? "None" : farmer.dealer_id) << "\n";
                std::cout << "----------------------------------------\n";
            }
        }
        if (!found) {
            std::cout << "No farmers found matching your search criteria.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error searching farmers: " << e.what() << "\n";
    }
}

void Dealers::processFarmersPayments(const std::function<std::vector<std::shared_ptr<Farmers::Farmer>>()>& readFarmers,const std::function<std::vector<std::shared_ptr<Dealer>>()>& readDealers)
{
    try {
        
        auto dealers = readDealers();
        auto farmers = readFarmers();

        
        std::string dealerId;
        std::cout << "\nEnter dealer ID: ";
        std::getline(std::cin, dealerId);

        auto dealerIt = std::find_if(dealers.begin(), dealers.end(),
            [&dealerId](const auto& d) { return d->dealer_id == dealerId; });

        if (dealerIt == dealers.end()) {
            std::cout << "Dealer not found!\n";
            return;
        }

        auto& dealer = **dealerIt;


        std::vector<PaymentRate> currentRates = dealer.paymentRates;

        
        if (!currentRates.empty()) {
            std::cout << "\n=== Current Payment Rates ===\n";
            for (const auto& rate : currentRates) {
                std::cout << rate.produceType << ": " << rate.rate << " per unit\n";
            }

            char choice;
            std::cout << "\nUpdate rates? (y/n): ";
            std::cin >> choice;
            std::cin.ignore();

            if (tolower(choice) == 'y') {
                std::set<std::string> produceTypes;
                for (const auto& item : dealer.inventory) {
                    produceTypes.insert(item.item);
                }

                for (const auto& produce : produceTypes) {
                    char update;
                    std::cout << "\nUpdate rate for " << produce << "? (y/n): ";
                    std::cin >> update;
                    std::cin.ignore();

                    if (tolower(update) == 'y') {
                        double newRate;
                        std::cout << "Enter new rate per unit for " << produce << ": ";
                        std::cin >> newRate;
                        std::cin.ignore();

                        auto rateIt = std::find_if(currentRates.begin(), currentRates.end(),
                            [&produce](const PaymentRate& r) { return r.produceType == produce; });

                        if (rateIt != currentRates.end()) {
                            rateIt->rate = newRate;
                            rateIt->lastUpdated = getCurrentTimestamp();
                        } else {
                            currentRates.push_back({
                                produce,
                                newRate,
                                getCurrentTimestamp()
                            });
                        }
                    }
                }
            }
        } else {
            std::set<std::string> produceTypes;
            for (const auto& item : dealer.inventory) {
                produceTypes.insert(item.item);
            }

            for (const auto& produce : produceTypes) {
                double newRate;
                std::cout << "Enter rate per unit for " << produce << ": ";
                std::cin >> newRate;
                std::cin.ignore();

                currentRates.push_back({
                    produce,
                    newRate,
                    getCurrentTimestamp()
                });
            }
        }

        PaymentRecord newRecord;
        newRecord.paymentDate = getCurrentTimestamp();
        newRecord.totalAmount = 0.0;

        for (auto& farmer : farmers) {
            if (farmer->dealer_id == dealerId) {
                std::vector<FarmerPayment> farmerPayments;

                for (const auto& item : dealer.inventory) {
                    if (item.farmer_name == farmer->farmersname) {
                        auto rateIt = std::find_if(currentRates.begin(), currentRates.end(),
                            [&item](const PaymentRate& r) { return r.produceType == item.item; });

                        if (rateIt != currentRates.end()) {
                            double quantity = std::accumulate(item.quantity->begin(),
                                                            item.quantity->end(), 0.0);
                            double amount = quantity * rateIt->rate;

                            FarmerPayment payment;
                            payment.farmerId = farmer->id;
                            payment.farmerName = farmer->farmersname;
                            payment.produceType = item.item;
                            payment.quantity = quantity;
                            payment.amount = amount;
                            payment.date = newRecord.paymentDate;

                            farmerPayments.push_back(payment);
                            newRecord.totalAmount += amount;
                        }
                    }
                }

                if (!farmerPayments.empty()) {
                
                    for( auto& payment : farmerPayments){
			    farmer->paymentHistory.push_back(payment);
		    }

                    newRecord.farmerPayments.insert(
                        newRecord.farmerPayments.end(),
                        farmerPayments.begin(),
                        farmerPayments.end()
                    );
                }
            }
        }

        if (!newRecord.farmerPayments.empty()) {
            newRecord.ratesUsed = currentRates;

            dealer.paymentRates = currentRates;
            dealer.paymentRecords.push_back(newRecord);
            dealer.totalPayments += newRecord.totalAmount;

            json farmersJson = json::array();
            for (const auto& farmer : farmers) {
                farmersJson.push_back(farmer->toJson());
            }
            writeToFile("farmers.json", farmersJson);

            json dealersJson = json::array();
            for (const auto& d : dealers) {
                dealersJson.push_back(d->toJson());
            }
            writeToFile("dealers.json", dealersJson);

            std::cout << "\n=== Payment Processing Complete ===\n";
            std::cout << "Date: " << newRecord.paymentDate << "\n";
            std::cout << "Total Amount Paid: " << newRecord.totalAmount << "\n";
            std::cout << "Number of Farmers Paid: " << [&]() {
                std::set<std::string> uniqueFarmers;
                for (const auto& payment : newRecord.farmerPayments) {
                    uniqueFarmers.insert(payment.farmerId);
                }
                return uniqueFarmers.size();
            }() << "\n";
	    time_t now = std::time(nullptr);
	    tm* local_time=localtime(&now);

            std::stringstream reportFilename;
            reportFilename << "payment_report_" << dealer.dealersname << "_"
                         << std::put_time(local_time, "%Y%m%d_%H%M%S")
                         << ".json";

            std::ofstream reportFile(reportFilename.str());
            reportFile << std::setw(4) << newRecord.toJson() << std::endl;
            std::cout << "Detailed report saved to: " << reportFilename.str() << "\n";
        } else {
            std::cout << "No payments were processed - no eligible produce found.\n";
        }

    } catch (const json::exception& e) {
        std::cerr << "JSON error: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}
bool Dealers::registerUser(const std::string& username,const std::string& email,const std::string& password,const std::function<std::vector<std::shared_ptr<Dealer>>()>& readFunction){
	auto dealers=readFunction();
	for(const auto& dealer : dealers){
	        if(std::any_of(dealer->user.begin(),dealer->user.end(),[&username,&email](const auto& u){
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
			dealer->user.push_back(new_user);
			json dealersJson=toJsonDealers(dealers);
			writeToFile("dealers.json",dealersJson);
		}
	}
}
bool Dealers::loginUser(const std::string& username_or_email,const std::string& password,const std::function<std::vector<std::shared_ptr<Dealer>>()>& readFunction){
	 auto dealers=readFunction();
	 for(const auto& item : dealers){
		 auto userIt=std::find_if(item->user.begin(),item->user.end(),[&username_or_email](const auto& u){
				 return u->username==username_or_email || u->email==username_or_email;
				});
		 if(userIt==item->user.end()){
			 return false;
		}
		return verify_password((*userIt)->hashed_password,password);
	}
}
void Dealers::displayMenu(){
	std::cout<<"\n=======Aunthentication System=========="
		<<"\n1.Register:"
		<<"\n2.Login:"
		<<"\n3.Exit:"
		<<"\nChoise";
}
void Dealers::registerUser(const std::function<std::vector<std::shared_ptr<Dealer>>()>& readFunction){
	auto dealers=readFunction();
	auto dealer = std::make_shared<Dealer>();
	std::string username,password,email;
	std::cout<<"\n=======Registration=======";
	std::cout<<"\nUsername: ";
	std::getline(std::cin,username);
	dealer->current_user=username;
	std::cout<<"\nEmail: ";
	std::getline(std::cin,email);
	std::cout<<"\nPassword: ";
	std::getline(std::cin,password);
	json dealersJson=toJsonDealers(dealers); 
	writeToFile("dealers.json",dealersJson);
	if(registerUser(username,email,password,readFromFile)){
		std::cout<<"Registration successfull.\n";
	}else{
		std::cout<<"\nRegistration failed(username or email alredy exists)";
	}
}
void Dealers::loginUser(){
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
void Dealers::LogIn(){
	int opt;
	displayMenu();
	std::cin>>opt;
	switch(opt){
		case 1:
			registerUser();
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
bool Dealers::sendNotification(const std::string& receiver,const std::string& message,const std::function<std::vector<std::shared_ptr<Farmers::Farmer>>()>& readFarmers,std::function<std::vector<std::shared_ptr<Dealer>>()>& readDealers){
	auto farmers=readFarmers();
	auto dealers=readDealers();
	Notifications n;
	n.id=generateUUID;
	n.receiver=receiver;
	n.sender=dealers->current_user;
	n.message=message;
	n.timestamp=std::time(nullptr);
	n.is_read=false;
	json dealersJson=toJsonDealers(dealers);  
	writeToFile("dealers.json",dealersJson);
	json farmersJson=toJsonFarmers(farmers);
	writeToFile("Farmers.json",farmersJson);
}
void Dealers::sendNotifications(const std::function<std::vector<std::shared_ptr<Farmers::Farmer>>()>& readFarmers,const std::function<std::vector<std::shared_ptr<Dealer>>()>& readDealers){
	auto farmers=readFarmers();
	auto dealers=readDealers();
	int opt;
	std::cout<<"\n========Messanger=======:";
	std::cout<<"\nWhich of the following would wish to undertake?";
	std::cout<<"\n1.Send a notification";
	std::cout<<"\n2.LogOut:";
	std::cin>>opt;
	std::cin.ignore();
	std::vector<Farmer::Farmer> matchingFarmers;
	for(const auto& farmer :farmers){
		if(std::ranges::find_if(dealers,[&farmer](const std::shared_ptr<Dealer>& dealer){
					return farmer.dealer_id==dealer->dealer_id;
				}) !=dealers.end()){
			matchingFarmers.push_back(farmer);
		}
	}
	do{
		std::cout<<"\nFarmers you can send notifications in your dealership: ";
		for(const auto& farmer : matchingFarmers){
			std::cout<<"\nFarmer: "<<farmer.farmersname <<"||"<<"FarmeName: "<<farmer.farmname;
		}

		std::cout<<"\nSelect farmer(0 to send to all) who you wish to send your notifications to: ";
		int farmer_choise;
		std::cin>>farmer_choise;
		std::cin.ignore();
		for(const auto& farmer : matchingFarmers){
			if(farmer_choise<0 || farmer_choise>farmer.size()){
				std::cout<<"\nInvalid input!!";
			}
			std::cout<<"\nEnter the your notification: ";
			std::string message;
			std::getline(std::cin,message);
			if(farmer_choise==0){
				sendNotification(farmer,message,Farmers::readFromFile,readFromFile);
				std::cout<<"\nNotification sent to all farmers in your dealership!!";
			}else{
				std::cout<<"\nEnter the name of the farmer you wish to send the notification.Should be in your dealership,";
				std::string name;
				std::getline(std::cin,name);
				auto farmerIt=std::find_if(farmer.begin(),farmer.end(),[&name](const std::shared_ptr<Farmers::Farmer>& f){
						return f->farmersname==name;
					});
				if(farmerIt!=farmer.end()){
					sendNotification(farmerIt,message,Farmers::readFromFile,readFromFile);
					std::cout<<"\nNotification sent.";
				}else{
					std::cout<<"\nFarmer not found!!";
				}
			}
		}
	}while(opt==1);
	if(opt==2){
		return;
	}
}




