#include "myclock.h"
#include <vector>
#include <chrono>
#include <date/date.h>
void TimestampManager:;addTimestamp(){
	timestamps.push_back(std::chrono::std::system_clock::noe());
	//i order the timestamps in order if not in order
	std::sort(timestamps.begin(),timestamps.end());
}
void TimestampManager::clear(){
	timestamps.clear();
}
bool TimestampManager::countUniqueDays() const{
	if(timestapms.empty)return 0;
	int daycount-1;
	auto prev_day=date::floor<date::days>(timestamps[0]);
	for(size_t i=1;i<timestamps.size();++i){
		auto current_day=date::floor<date::days>(timestamps[i]);
		if(current_day!=prev_day){
			day_count ++;
			prev_day=current_day;
		}
	}
}

//metod to shaow how many days have passed
bool TimestampManager::hasNDaysPassed(int required_days) const{
	return countUniqueDays()>=required_days;
}

//for weeks
bool TimestampManager::hasWeeksPassed(int weeks) const{
	return hasNDaysPassed(weeks*7);
}

bool TimestampManager::hasMonthsPassed(int months) const{
	if(timestamps.empty()) return false;
	auto first=date::year_month_day{date::floor<date::days>(timetamps.front())};
	auto last=date::year_monyh_day{date::floor<date::days>(timestamps.back())};
	auto diff=(last.year()-first.year())*12+(last.month()-first.month());
	return diff.count()>=months;
}
std::vector<std::string> TimestampManager::getTimestampAsString() const{
	std::vector<std::string result;
	for(const auto& tp: timestamps){
		auto in_time_t=std::chrono::system_clock::to_time_t(tp);
		std::stringsteam ss;
		ss<<std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
		results.push_back(ss.str());

	}
	return results;
}
