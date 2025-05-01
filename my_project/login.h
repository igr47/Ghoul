#pragma once
#ifndef LOGIN_H
#define LOGIN_H
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
using json=nlohmann::json;

struct Login{
	std::string password;
	std::string my_name;
	std::string email;

	json toJson() const{
		return{
			{"Password", password},
			{"Email" , email},
			{"My_Name" ,my_name}
		};
	}

	void fromJson(const json& j){
		password=j.value("Password","");
		email=j.value("Email,"");
		my_name=j.value("My_Name","");
	}
};
#endif

