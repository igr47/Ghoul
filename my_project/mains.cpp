#include "farmersclass.h"
#include "uuid.h"
#include "dealersclass.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;

int main() {
    int choice;
    Farmers f;
    Dealers d;

    do {
        // Main menu display
        std::cout << "**********************************************************\n";
        std::cout << "Welcome to my records\n";
        std::cout << "Which of the following are you?\n";
        std::cout << "1. Farmer.\n";
        std::cout << "2. Dealer.\n";
        std::cout << "3. Or press 0 if you wish to exit!\n";
        std::cout << "***********************************************************\n";
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1) {
            int option2;
            do {
                // Farmer menu display
                std::cout<<"#####################################################################\n";
                std::cout << "Welcome dear farmer and we hope you enjoy your visit.\n";
                std::cout << "Would you like to proceed as?\n";
                std::cout << "1. ADMIN.\n";
                std::cout << "2. FARMER.\n";
                std::cout<<"#####################################################################\n";
                std::cin >> option2;
                std::cin.ignore();

                if (option2 == 1) {
                    int option;
                    do {
                        // Farmer admin menu
                        std::cout<<"***************************************************************\n";
                        std::cout << "Which of the following operations do you wish to undertake?\n";
                        std::cout << "1. Process and upload farmers details?\n";
                        std::cout << "2. Display the saved farmers?\n";
                        std::cout << "3. Search for a specific farmer?\n";
                        std::cout << "4. Update farmers details?\n";
                        std::cout << "5. Delete farmers details?\n";
                        std::cout << "6. Return to main menu.\n";
                        std::cout<<"***************************************************************\n";
                        std::cin >> option;
                        std::cin.ignore();

                        switch (option) {
                            case 1:
                                f.getFarmersDetails();
                                break;
                            case 2:
                                f.displayFarmersDetails(Farmers::readFromFile);
                                break;
                            case 3: {
                                std::string name;
                                std::cout << "Enter the name of the farmer you wish to look for.\n";
                                std::cin.ignore();
                                std::getline(std::cin, name);
                                f.searchFarmersDetails(Farmers::readFromFile, name);
                                break;
                            }
                            case 4:
                                f.updateFarmersDetails(Farmers::readFromFile);
                                break;
                            case 5:
                                f.deleteFarmersDetails(Farmers::readFromFile);
                                break;
                            case 6:
                                std::cout << "Returning to main menu.\n";
                                break;
                            default:
                                std::cout << "Please enter a valid input.\n";
                                break;
                        }
                    } while (option != 6);
                } else if(option2==2){
		    f.logIn();
                    f.addFarmProduce(Farmers::readFromFile);
                }
            } while (option2 != 2);
        } else if (choice == 2) {
            int optiondealer;
            do {
                // Dealer menu display
                std::cout<<"#############################################################################\n";
                std::cout << "Welcome dear dealer and we hope you enjoy your experience!\n";
                std::cout << "Would you like to proceed as?\n";
                std::cout << "1. ADMIN.\n";
                std::cout << "2. DEALER.\n";
                std::cout<<"############################################################################\n";
                std::cin >> optiondealer;
                std::cin.ignore();

                if (optiondealer == 1) {
                    int processchoice;
                    do {
                        // Dealer admin menu
                        std::cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
                        std::cout << "Welcome ADMIN!\n";
                        std::cout << "Which of the following operations would you like to undertake?\n";
                        std::cout << "1. Process and upload dealers details?\n";
                        std::cout << "2. Display the saved details?\n";
                        std::cout << "3. Search for a specific dealer?\n";
                        std::cout << "4. Update dealers details?\n";
                        std::cout << "5. Delete dealers details?\n";
                        std::cout << "6. Return to main menu.\n";
                        std::cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
                        std::cin >> processchoice;
                        std::cin.ignore();

                        switch (processchoice) {
                            case 1:
                                d.getDealersDetails();
                                break;
                            case 2:
                                d.displayDealersDetails(Dealers::readFromFile);
                                break;
                            case 3: {
                                std::string name;
                                std::cout << "Enter the name of the dealer you wish to search for.\n";
                                std::getline(std::cin, name);
                                d.searchDealersDetails(Dealers::readFromFile, name);
                                break;
                            }
                            case 4:
                                d.updateDealersDetails(Dealers::readFromFile);
                                break;
                            case 5:
                                d.deleteDealersDetails(Dealers::readFromFile);
                                break;
                            case 6:
                                std::cout << "Returning to main menu.\n";
                                break;
                            default:
                                std::cout << "Please enter a valid option between 1-6!\n";
                                break;
                        }
                    } while (processchoice != 6);
                } else if(optiondealer==2){
                    int processchoise_2;
                    do{
                        // Dealer operations menu
                        std::cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
                        std::cout<<"Welcome dealer and we hope you enjoy your your experience here.\n";
                        std::cout<<"Which of the folowwing operation would you wish to undertake?\n";
                        std::cout<<"1.Display the available produce from the farmers registered under your cooperation for purchase?\n";
                        std::cout<<"2.Purchase produce from farmers/Update your inventory?\n";
                        std::cout<<"3.Veiw your current inventoro?\n";
                        std::cout<<"4.View all your transactions?\n";
                        std::cout<<"5.Search for a farmer by name or location?\n";
                        std::cout<<"6.Reaturn to main menu?\n";
                        std::cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
                        std::cin>>processchoise_2;
                        std::cin.ignore();
                        switch(processchoise_2){
                            case 1:
                                d.viewAvailableProduce(Farmers::readFromFile);
                                break;
                            case 2:
                                d.purchaseFarmersProduce(Farmers::readFromFile, Dealers::readFromFile);
                                break;
                            case 3:
				d.viewInventory(Dealers::readFromFile);
                                break;
                            case 4:
				d.viewTransactions(Dealers::readFromFile);
                                break;
                            case 5:
                                break;
                            case 6:
                                std::cout<<"Returning to main menu!\n";
                                break;
                            default:
                                std::cout<<"Pease enter valid input from (1-6)!\n";
                                break;
                        }
                    } while(processchoise_2 !=6);
                }
            } while (optiondealer != 2);
        }
    } while (choice != 0);
    
    std::cout << "Exiting the program. Goodbye!\n";
    return 0;
}				
