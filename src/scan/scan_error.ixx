export module lib2.scan:scan_error;

import std;

namespace lib2
{
	export
	class scan_parse_error : public std::logic_error
	{
	public:
		using std::logic_error::logic_error;
	};

	export
	class scan_pattern_not_matched : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};

	export
	class bad_scan : public std::exception, public std::nested_exception
	{
	public:
		using std::nested_exception::nested_exception;

		const char* what() const noexcept
		{
			return "bad scan";
		}
	};
}