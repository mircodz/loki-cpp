#include <benchmark/benchmark.h>

#include "builder.hpp"
#include "parser.hpp"

static void BM_Parser(benchmark::State& state) {
	using namespace loki;
	auto registry = Builder().Build();
	std::string s = registry.Metrics();
	for (auto _ : state) {
		Parser parser{s};
		parser.metrics();
	}
}

BENCHMARK(BM_Parser);
BENCHMARK_MAIN();
