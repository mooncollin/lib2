module;

#include "lib2/strings/widen.h"

export module lib2.tests.strings:widen;

import lib2;

namespace lib2::tests::strings
{
	export
	class widen_string_test : public lib2::test::test_case
	{
	public:
		widen_string_test()
			: lib2::test::test_case{"widen_string_test"} {}

		void operator()() final
		{
			constexpr auto char_str   {STATICALLY_WIDEN(char, "Hello, world!")};
			constexpr auto wchar_str  {STATICALLY_WIDEN(wchar_t, "Hello, world!")};
			constexpr auto char8_str  {STATICALLY_WIDEN(char8_t, "Hello, world!")};
			constexpr auto char16_str {STATICALLY_WIDEN(char16_t, "Hello, world!")};
			constexpr auto char32_str {STATICALLY_WIDEN(char32_t, "Hello, world!")};

			lib2::test::assert_equal(char_str, "Hello, world!");
			lib2::test::assert_equal(wchar_str, L"Hello, world!");
			lib2::test::assert_equal(char8_str, u8"Hello, world!");
			lib2::test::assert_equal(char16_str, u"Hello, world!");
			lib2::test::assert_equal(char32_str, U"Hello, world!");
		}
	};
}