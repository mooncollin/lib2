export module lib2.test:test_suite;

import std;

import lib2.fmt;

import :test_result;

namespace lib2::test
{
	struct test_entry
	{
		std::string name;
		// std::move_only_function<test_result() const> run;
		std::function<test_result()> run;
	};

	export
	class test_suite
	{
	public:
		test_suite() noexcept = default;

		test_suite(const test_suite&) = delete;
		test_suite& operator=(const test_suite&) = delete;

		test_suite(test_suite&&) noexcept = default;
		test_suite& operator=(test_suite&&) noexcept = default;

		[[nodiscard]] auto begin() const noexcept
		{
			return cases.begin();
		}

		[[nodiscard]] auto end() const noexcept
		{
			return cases.end();
		}

		[[nodiscard]] std::size_t size() const noexcept
		{
			return cases.size();
		}

		[[nodiscard]] bool empty() const noexcept
		{
			return cases.empty();
		}

		void clear() noexcept
		{
			cases.clear();
		}

		template<typename F, class... Args>
			requires(std::invocable<F, Args...>)
		void add_test_case(std::string name, F&& func, Args&&... args)
		{
			cases.emplace_back(std::move(name),
				[fn = std::forward<F>(func),
				 params = std::make_tuple(std::forward<Args>(args)...)] {
				test_result result;
				try
				{
					std::apply(fn, params);
				}
				catch (const std::exception&)
				{
					result.failure(std::current_exception());
				}
				return result;
			});
		}

		template<class T, class... Args>
			requires(std::constructible_from<T, Args...>)
		void add_test_case(std::string name, Args&&... args)
		{
			add_test_case(
				std::move(name),
				[](const auto&... args) {
					T fixture{args...};
					fixture();
				},
				std::forward<Args>(args)...
			);
		}

		template<template<class...> class T, class... Args>
		void add_test_case(Args&&... args)
		{
			using type = decltype(T{std::declval<Args>()...});
			add_test_case<type>(std::forward<Args>(args)...);
		}

		void add_test_suite(test_suite&& other)
		{
			cases.insert(
				cases.end(),
				std::make_move_iterator(other.cases.begin()),
				std::make_move_iterator(other.cases.end())
			);
		}
		
		void add_test_suite(const std::string_view prefix, test_suite&& other)
		{
			cases.reserve(cases.size() + other.cases.size());
			for (auto& entry : other.cases)
			{
				cases.emplace_back(
					lib2::format<"{}::{}">(prefix, entry.name),
					std::move(entry.run)
				);
			}
		}
	private:
		std::vector<test_entry> cases;
	};
}