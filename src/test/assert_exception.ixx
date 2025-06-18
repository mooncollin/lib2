export module lib2.test:assert_exception; 

import std;

namespace lib2::test
{
	export
	class assert_exception : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};

	export
	class error_exception : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};
}