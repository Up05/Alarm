#include <time.h>
#include <regex>
//#include <chrono>
#include <queue>
#include <array>
#include <set>
#include "threads.h"
#include "date.h"


Alarm::Alarm() {
	created = (int)time(NULL);
}

namespace threads { extern std::mutex mutex; }


namespace alarm {

	std::priority_queue<Alarm, std::vector<Alarm>, Compare> alarm_queue;
	std::set<Alarm, Compare> alarms;


	time_t get_time(std::vector<std::string>& args, bool has_name) {
		int output = 0;


		for (int i = 1; i < args.size() - has_name; i++) { // args[0] is action type!
			char unit = tolower(args[i][args[i].size() - 1]);
			char* unitless = (char*)args[i].c_str();
			unitless[args[i].size() - 1] = '\0';
			int time = atoi(unitless);
			unitless[args[i].size() - 1] = unit; // useless? I say: time saving.
			switch (unit) {
			case 'y': time = time * 60 * 60 * 24 * 356; break;
			case 'w': time = time * 60 * 60 * 24 * 7; break;
			case 'd': time = time * 60 * 60 * 24; break;
			case 'h': time = time * 60 * 60; break;
			case 'm': time = time * 60;
			}
			output += time;
		}
		return output;
	}

	void add(std::vector<std::string>& args) {

		bool has_name = !std::regex_match(args[args.size() - 1], std::regex("\\d+[ywdhms]", std::regex_constants::icase));

		time_t relative_time = get_time(args, has_name);

		Alarm alarm;
		alarm.end_time = time(NULL) + relative_time;
		alarm.created = time(NULL);
		if (has_name)
			alarm.name = args[args.size() - 1];
		else {
			alarm.name = "Alarm-" + std::to_string(alarms.size());
		}
		LOG2("alarm", "name of alarm: \"%s\"", alarm.name.c_str())

		server::send("[SUCCESS] Successfully added the alarm");
		server::send("[END]");
		
		register_alarm(alarm, true);
	}

	void remove(std::vector<std::string>& args) {
		bool removed_any = false;

		{ // BEAUTIFUL! ABSOLUTE FUCKING BEAUTIFUL! WORK OF ART RIGHT HERE! NOTHING, BUT PRAISE SHOULD BE GIVEN FOR THIS SOLUTION!
			std::lock_guard<std::mutex> lock(threads::mutex);

			int i = 1; // ! USE i!!!
			while (!alarms.empty() && i < args.size()) {
				auto iter = std::find_if(
					alarms.begin(), alarms.end(),
					[args, i](const Alarm& alarm) {
						return alarm.name.find(args[i]) != std::string::npos;
					}
				);
				if (iter != std::end(alarms)) {
					LOG2("alarm", "Alarm by name \"%s\" has been removed!", iter->name.c_str());
					alarms.erase(iter);
					server::send(("[SUCCESS] Alarm No. " + std::to_string(i) + ", with name: \"" + std::string(args[i]) + "\" has been removed.").c_str());
					server::send("END");
					removed_any = true;
				} else {
					LOG("! alarm", ("Alarm No. " + std::to_string(i) + ", with name: \"" + std::string(args[i]) + "\" not found!").c_str());
					server::send(("[ERROR] Alarm No. " + std::to_string(i) + ", with name: \"" + std::string(args[i]) + "\" not found!").c_str());
					server::send("END");
				}
				i++;
			} // need to test this! and see what happens if find_if doesn't find!

			if (removed_any)
				io::update_alarm_file();

		}
		if (removed_any)
			threads::update();

	}

	namespace str_utils {
		std::string center(std::string str, int width) {
			std::string centered_str(width, ' ');

			size_t pad = ( width - 2 - str.size() ) / 2;
			for (size_t i = 0; i < width; i ++) 
				if (i > pad && i < pad + str.size() + 1) 
					centered_str[i] = str[i - 1 - pad];			
			
			return centered_str;
		}
	
	
	}


	void list(std::vector<std::string>& args) { // TODO: Make more beautiful!
		LOG("alarm", "Listing alarms...");

		if (alarms.size() == 0) {
			LOG("alarms", "No alarms found!");
			server::send("[INFO] No alarms found!");
			server::send("[END]");
			return;
		}

		std::string output;

		int 
			longest_name = 12,
			longest_date = 13;

		for (const Alarm& alarm : alarms) {
			if (longest_name < alarm.name.size()) 
				longest_name = (int) alarm.name.size() + 4; 

			if (longest_date < date::format_relative(alarm.end_time).size())
				longest_date = (int) date::format_relative(alarm.end_time).size() + 4;
		}

		bool relative_date = false;
		if (args.size() >= 2)
			if (args[1][0] == 'r' || (args[1].size() > 1 && args[1][1] == 'r'))
				relative_date = true;

#define A "-o-"
#define B " I "


		output += 
			" | " + 
			str_utils::center("name", longest_name) + 
			" | " + 
			(relative_date ? str_utils::center("time until", longest_date) : str_utils::center("will end by", 19)) + 
			" | " +
			str_utils::center("date created", 19) +
			" |\n";



		//output += std::string(3 * 4 + longest_name + (relative_date ? longest_date : 19) + 19, '=') + '\n';
		output += A + std::string(longest_name, '-') + A + std::string(relative_date ? longest_date : 19, '-') + A + std::string(19, '-') + A + '\n';

		//std::cout << "[alarm] " << date::format_absolute(alarms.begin()->end_time) << std::endl;


		for (const Alarm& alarm : alarms) {
			LOG2("alarm", "name[i]: \"%s\"", alarm.name.c_str())

			using namespace str_utils; using namespace date;
			output += " | ";
			output += center(alarm.name, longest_name);
			output += " | ";
			output += relative_date ? center(format_relative(alarm.end_time), longest_date) : format_absolute(alarm.end_time);
			output += " | ";
			output += format_absolute(alarm.created);
			output += " |\n";
		}
		//output += std::string(3 * 4 + longest_name + (relative_date ? longest_date: 19) + 19, '=') + '\n';
		output += A + std::string(longest_name, '-') + A + std::string(relative_date ? longest_date : 19, '-') + A + std::string(19, '-') + A + '\n';


		std::cout << output;
		server::send(output.c_str());
		server::send("[END]");

#undef A
#undef B
	
	}

	void register_alarm(Alarm alarm, bool append_to_file){

		LOG2("alarm", "Started registering alarm(%s, %lld)...", alarm.name.c_str(), alarm.end_time);

		if (alarm.end_time < time(NULL)) {
			LOG("alarm", "Alarm missed, ignoring.");
			std::string info = ("You have missed an alarm by name: \"") + (std::string) alarm.name + "\". \nIt was due by: " + date::format_absolute(alarm.end_time);
			MessageBox(NULL, info.c_str(), "Alarm missed", MB_ICONWARNING | MB_OK);
			return;
		}

		// MUTEX LOCK HERE!!!!
		threads::mutex.lock();
			alarms.emplace(alarm);
		threads::mutex.unlock();

		threads::update();
		if(append_to_file)
			io::append_alarm(alarm);

		server::send("alarm registered!\n");

		LOG("alarm", "Finished registering alarm.")

		//std::cout << "Alarm registered!    alarm.cpp#register_alarm()\n";
	}

	/** This means that alarms should be sorted in descending order */
	void pop_alarm() {
		//if(alarms.erase)
		alarms.erase(*std::prev(alarms.end()));
	}

	Alarm peak_alarm() {
		return *std::prev(alarms.end());
	}

	void notify(const char* name) {
		// this won't work for other people which do not have Toast, although, I could download the lib for this.
		system((std::string("powershell Toast -Text \"") + name + '"').c_str());
		// Dialog box?
		
	}


}


//int main() {
//	std::vector<std::string> test = { "a", "4h", "30m", "5s" };
//	std::cout << alarm::str_to_args(test) << std::endl;
//
//
//
//}