#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <windows.h>

#define _ULEN(LOCATION) 20 - (int) ( strlen(LOCATION) + (log10(__LINE__) + 1) + 7 )
#define LOG(LOCATION, STRING) printf("[%s ln: %i]%*s %s\n", LOCATION, __LINE__, _ULEN(LOCATION), "", STRING);
#define LOG2(LOCATION, FORMATTING, ...)                  \
	printf("[%s ln: %i]%*s " FORMATTING "\n",             \
		LOCATION, __LINE__, _ULEN(LOCATION), "", __VA_ARGS__ \
	);


typedef struct Alarm {
	std::string name;
	time_t end_time; // absolute time, not relative!
	time_t created;

public: 
	Alarm();


} Alarm;



namespace alarm {

	//void run(std::string name, time_t end);
	void add(std::vector<std::string>& args);
	void remove(std::vector<std::string>& args);
	void list(std::vector<std::string>& args);
	void register_alarm(Alarm alarm, bool append_to_file);
	void notify(const char* name);
	void pop_alarm();
	Alarm peak_alarm();

	namespace str_utils {
		std::string center(std::string str, int width);
	}


}

class Compare {
public:
	bool operator() (Alarm alarm1, Alarm alarm2) const {
		return 
			alarm1.end_time == alarm2.end_time ?
			alarm1.created  >  alarm2.created :
			alarm1.end_time >  alarm2.end_time;
	}
};