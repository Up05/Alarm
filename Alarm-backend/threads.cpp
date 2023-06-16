#include "fileio.h"
#include <map>
#include <chrono> 
#include <thread>
#include <condition_variable>
//#include <queue>
#include <set>
namespace alarm {
	//extern std::priority_queue<Alarm, std::vector<Alarm>, Compare> alarm_queue;
	extern std::set<Alarm, Compare> alarms;
}

namespace threads {

	std::thread thread;

	std::mutex mutex;
	std::condition_variable cond;
	bool flag = false;
	std::atomic<bool> interupted = false;

	void run() { // ! this seems to either not execute or die immediately!
		

		while (true) {
			std::unique_lock<std::mutex> lock(mutex);
			flag = false;

			LOG("threads", "Starting run cycle...")
			if (alarm::alarms.size() == 0) {
				LOG("threads", "No active alarms found.")

				cond.wait_for(
					lock,
					std::chrono::minutes(30), // this could be INT_MAX just as well
					[]() { return flag; }
				);
				continue;
			}

			if (alarm::peak_alarm().end_time < time(NULL)) {
				alarm::pop_alarm(); // alarm::notify missed alarm?
				io::update_alarm_file();
				continue;
			}

			cond.wait_until(
				lock,
				std::chrono::system_clock::from_time_t(alarm::peak_alarm().end_time),
				[]() { return flag; }
			);

			if (!interupted) {
				
				LOG("threads", "Alarm has popped!")
				
				alarm::notify(alarm::peak_alarm().name.c_str());
				alarm::pop_alarm(); 
				io::update_alarm_file(); 

			}
			else
				interupted = false;
		}
	}

	void init() {
		thread = std::thread(run);	
	}


	void update() {
		
		std::lock_guard<std::mutex> lock(mutex);
		interupted = true;
		flag = true;
		cond.notify_one();
		
		LOG("threads", "New alarm added.")


	}

}

// I could use 1 thread keep interupting it 