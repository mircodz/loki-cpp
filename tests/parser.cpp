#include <catch2/catch.hpp>

#include <loki/builder.hpp>
#include <loki/parser.hpp>

#include <fstream>

TEST_CASE("Empty Parser") {
	using namespace loki;
	Parser parser{""};
	REQUIRE(parser.metrics().size() == 0);
}
