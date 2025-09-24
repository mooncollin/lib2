export module lib2.benchmarking:benchmark;

import std;

import :do_not_optimize;

namespace lib2::benchmarking
{
	export
	class benchmark
	{
	public:
		benchmark(std::string str) noexcept
			: name_{std::move(str)} {}

		virtual void setup() {}
		virtual void tear_down() {}
		virtual void operator()() = 0;
		virtual ~benchmark() noexcept = default;

		[[nodiscard]] const std::string& name() const noexcept
		{
			return name_;
		}
	private:
		std::string name_;
	};

	export
	class noop_benchmark : public benchmark
	{
	public:
		noop_benchmark()
			: benchmark{"lib2::benchmarking::noop"} {}

		void operator()() noexcept final
		{
			do_not_optimize(0);
		}
	};
}