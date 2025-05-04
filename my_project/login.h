#pragma once
#ifndef LOGIN_H
#define LOGIN_H
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
using json=nlohmann::json;

struct UserCredentials{
	std::string hashed_password;
	std::string username;
	std::string email;

	json toJson() const{
		return{
			{"Password", hashed_password},
			{"Email" , email},
			{"My_Name" ,username}
		};
	}

	void fromJson(const json& j){
		hashed_password=j.value("Password","");
		email=j.value("Email","");
		username=j.value("My_Name","");
	}
};
#endif

