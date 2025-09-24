export module lib2.test:test_case;

import std;

import :assert_exception;
import :test_result;

namespace lib2::test
{
	export
	class test_case
	{
		public:
			test_case() noexcept = default;

			test_case(std::string str) noexcept
				: name_{std::move(str)} {}

			[[nodiscard]] const std::string& name() const noexcept
			{
				return name_;
			}

			virtual void setup() {}
			virtual void tear_down() {}
			virtual ~test_case() noexcept = default;

			test_result run() noexcept
			{
				test_result result;

				try
				{
					setup();
				}
				catch (const std::exception&)
				{
					result.failure(std::current_exception());
					return result;
				}

				try
				{
					operator()();
				}
				catch (const std::exception&)
				{
					result.failure(std::current_exception());
				}

				try
				{
					tear_down();
				}
				catch (const std::exception&)
				{
					result.failure(std::current_exception());
				}

				return result;
			}

			virtual void operator()() = 0;
		private:
			std::string name_;
	};
}