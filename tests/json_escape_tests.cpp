#include <gtest/gtest.h>

#include "detail/utils.hpp"

namespace loki::detail {

static std::string test_json_escape(const std::string& input) {
	std::string output;
	json_escape(output, input);
	return output;
}

TEST(JsonEscapeTest, EmptyString) {
	EXPECT_EQ("", test_json_escape(""));
}

TEST(JsonEscapeTest, StringWithoutSpecialCharacters) {
	EXPECT_EQ("abc 123 +!-", test_json_escape("abc 123 +!-"));
}

TEST(JsonEscapeTest, StringWithQuotes) {
	EXPECT_EQ("\\\"hello'", test_json_escape("\"hello'"));
}

TEST(JsonEscapeTest, StringWithBasicEscapedCharacters) {
	EXPECT_EQ("test\\\\me", test_json_escape("test\\me"));
	EXPECT_EQ("test\\bme", test_json_escape("test\bme"));
	EXPECT_EQ("test\\fme", test_json_escape("test\fme"));
	EXPECT_EQ("test\\nme", test_json_escape("test\nme"));
	EXPECT_EQ("test\\rme", test_json_escape("test\rme"));
	EXPECT_EQ("test\\tme", test_json_escape("test\tme"));
}

TEST(JsonEscapeTest, StringWithNonPrintableAsciiCharacters) {
	EXPECT_EQ("test\\u000bme", test_json_escape("test\013me"));
	EXPECT_EQ("test\\u0016me", test_json_escape("test\026me"));
	EXPECT_EQ("test\\u001bme", test_json_escape("test\033me"));
}

TEST(JsonEscapeTest, StringWithNullCharacter) {
	EXPECT_EQ("test\\u0000me", test_json_escape(std::string("test\0me", 7)));
}

}  // namespace loki::detail
