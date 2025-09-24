export module lib2.test:test_result;

import std;

import :assert_exception;

namespace lib2::test
{
	export
	class test_result
	{
		public:
			test_result() noexcept = default;

			[[nodiscard]] bool failed() const noexcept
			{
				return static_cast<bool>(ptr);
			}

			[[nodiscard]] bool passed() const noexcept
			{
				return !failed();
			}

			[[nodiscard]] bool errored() const noexcept
			{
				if (ptr) [[unlikely]]
				{
					try
					{
						std::rethrow_exception(ptr);
					}
					catch (const assert_exception&)
					{
						return false;
					}
					catch (const std::exception&)
					{
						return true;
					}
				}

				return false;
			}

			[[nodiscard]] explicit operator bool() const noexcept
			{
				return passed();
			}

			[[nodiscard]] std::exception_ptr failure() const noexcept
			{
				return ptr;
			}

			void failure(std::exception_ptr e) noexcept 
			{
				ptr = e;
			}

			template<class F>
			void on(F&& func) const
			{
				try
				{
					std::rethrow_exception(ptr);
				}
				catch (const assert_exception& e)
				{
					if constexpr (std::invocable<F, const assert_exception&>)
					{
						std::invoke(std::forward<F>(func), e);
						return;
					}
				}
				catch (const error_exception& e)
				{
					if constexpr (std::invocable<F, const error_exception&>)
					{
						std::invoke(std::forward<F>(func), e);
						return;
					}
				}
				catch (const std::exception& e)
				{
					if constexpr (std::invocable<F, const std::exception&>)
					{
						std::invoke(std::forward<F>(func), e);
						return;
					}
				}

				if constexpr (std::invocable<F>)
				{
					std::invoke(std::forward<F>(func));
				}
			}
		private:
			std::exception_ptr ptr;
	};
}