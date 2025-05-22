#ifndef MY_CLOCK_H
#define MY_CLOCK_H
#include <chrono>
#include <vector>
#include <date/date.h>

class TimestampManager{
	private:
		std::vector<std::chrono::system_clock::time_point> timestamps;
	public:
		//method to add new timestamps
		void addTimestamp();
		void clear();
		int counUniqueDays() const;
		bool hasNDaysPassed(int required_days) const;
		bool hasWeeksPassed(int weeks) const;
		bool hasMonthsPassed(int months) const;
		std::vector<std::string> getTimestampAsString() const;
		template<typename Duration>
		bool hasDurationPassed(const Duration& duration) const{
			if(timestamps.empty())return false;
			return (std::chrono::system_clock::now()-timestamps.front())>=duration;
		}
