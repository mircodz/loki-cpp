#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "builder.hpp"
#include "parser.hpp"

TEST_CASE( "Parsers", "[parser]" ) {
	using namespace loki;
	Parser parser{""};
	REQUIRE( parser.metrics().size() == 0 );

}
