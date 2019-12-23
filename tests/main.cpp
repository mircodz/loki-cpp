#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "builder.hpp"
#include "parser.hpp"

#include <fstream>

TEST_CASE( "Empty", "[parser]" ) {
	using namespace loki;
	Parser parser{""};
	REQUIRE( parser.metrics().size() == 0 );
}
