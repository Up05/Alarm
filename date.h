//std::string to_str(int number, const char* unit);
namespace date {
	std::string format_relative(time_t _time);
	std::string format_absolute(time_t _time);
}