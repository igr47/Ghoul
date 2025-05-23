#ifndef NOTIFICATION_H
#define NOTIFICATION_H
#include <string>
#include <nlohmann/json.hpp>
using json=nlohmann::json;

struct Notifications{
	std::string id;
	std::string sender;
	std::string receiver;
	std::string message;
	std::time_t timestamp;
	bool is_read;

	json toJson()const{
		return{
			{"ID" , id},
			{"From" , sender},
			{"To" , receiver},
			{"Message" , message},
			{"Seen_at" , timestamp},
			{"✓✓" , is_read}
		};
	}

	void fromJson(const json& j){
		id=j.value("id","");
		receiver=j.value("To","");
		sender=j.value("From","");
		message=j.value("Message","");
		timestamp=j.value("Seen_at",0);
		is_read=j.value("✓✓",false);
	}
};
#endif
