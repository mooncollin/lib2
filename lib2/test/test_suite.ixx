export module lib2.test:test_suite;

import std;

import lib2.iterator;

import :test_case;
import :test_result;

namespace lib2::test
{
	export
	class test_suite
	{
	public:
		test_suite() noexcept = default;

		test_suite(std::string name) noexcept
			: name_{std::move(name)} {}

		test_suite(const test_suite&) = delete;
		test_suite& operator=(const test_suite&) = delete;

		test_suite(test_suite&&) noexcept = default;
		test_suite& operator=(test_suite&&) noexcept = default;

		[[nodiscard]] const std::string& name() const noexcept
		{
			return name_;
		}

		[[nodiscard]] auto begin() const noexcept
		{
			return indirect_iterator{cases.cbegin()};
		}

		[[nodiscard]] auto begin() noexcept
		{
			return indirect_iterator{cases.begin()};
		}

		[[nodiscard]] auto end() noexcept
		{
			return indirect_iterator{cases.end()};
		}

		[[nodiscard]] auto end() const noexcept
		{
			return indirect_iterator{cases.cend()};
		}

		[[nodiscard]] std::size_t size() const noexcept
		{
			return cases.size();
		}

		void clear() noexcept
		{
			cases.clear();
		}

		template<std::derived_from<test_case> T, class... Args>
		void add_test_case(Args&&... args)
		{
			cases.push_back(std::make_unique<T>(std::forward<Args>(args)...));
		}

		template<template<class...> class T, class... Args>
		void add_test_case(Args&&... args)
		{
			using type = decltype(T{std::declval<Args>()...});
			add_test_case<type>(std::forward<Args>(args)...);
		}

		void add_test_suite(test_suite&& suite) noexcept
		{
			cases.splice(cases.end(), std::move(suite.cases));
		}
	private:
		std::string name_;
		std::list<std::unique_ptr<test_case>> cases;
	};
}