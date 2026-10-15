export module lib2.benchmarking:benchmark;

import std;

import :do_not_optimize;
import :context;

namespace lib2::benchmarking
{
	export
	template<class F>
	concept benchmark = std::is_invocable_r_v<void, F, state&>;

	export
	void noop_benchmark(state& state) noexcept
	{
		for (auto _ : state)
		{
			do_not_optimize(0);
		}
	}

	static_assert(benchmark<decltype(noop_benchmark)>);
}