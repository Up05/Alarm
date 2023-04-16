#include <time.h>
#include <string>
#include <math.h>
#include <memory>
#include <ctime> 


std::string to_str(int number, const char* unit) {
	if (number == 0)
		return std::string();
	return std::to_string(number) + unit;
}
namespace date {

	std::string format_relative(time_t _time) {
		_time = abs(_time - time(NULL));

		int minutes = _time / 60,
			hours = minutes / 60,
			days = hours / 24,
			years = days / 365; // everything is perfect. No.

		minutes %= 60;
		hours %= 24;
		days %= 365;

		return
			to_str(years, "y ") +
			to_str(days, "d ") +
			to_str(hours, "h ") +
			to_str(minutes, "m ") +
			to_str(_time % 60, "s");
	}

	std::string format_absolute(time_t _time) {

		std::tm* ptm = std::localtime(&_time);
		char buffer[32];
		std::strftime(buffer, 32, "%d/%m/%Y %H:%M:%S", ptm);

		return std::string(buffer);

	} // need to test both functions!


}

