#include <benchmark/benchmark.h>

#include <loki/builder.hpp>
#include <loki/parser.hpp>

static void BM_Parser(benchmark::State& state) {
	using namespace loki;
	auto registry = Builder().Build();
	std::string s = registry.Metrics();
	for (auto _ : state) {
		Parser parser{s};
		parser.metrics();
	}
}

static void BM_ParserEmpty(benchmark::State& state) {
	using namespace loki;
	for (auto _ : state) {
		Parser parser{""};
		parser.metrics();
	}
}

BENCHMARK(BM_Parser);
BENCHMARK(BM_ParserEmpty);
