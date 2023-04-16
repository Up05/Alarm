#pragma once
#include "alarm.h"


namespace server {
	/* Sends a message to THE client. Client port -- 5288 */
	void send(const char* message, int len);
	/* Sends a message to THE client. Client port -- 5288 */
	void send(const char* message);


} // it shouldn't be here, but I should have a single header file in general...

namespace io {
	void append_alarm(Alarm& alarm);
	void append_alarm(int index);
	void init();
	void update_alarm_file();

}