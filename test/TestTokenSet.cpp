#include "Common.h"
#include <tokenize/TokenSet.h>

using namespace tokenize;


void testBasicStringTokens() {
    SECTION("Doesn't match when no tokens are added") {
        TokenSet ts;
        MatchedToken mt;
        REQUIRE(ts.match("nomatch", &mt) == MatchResult::NoMatch);
    }

    SECTION("Doesn't match when no matching tokens are added") {
        TokenSet ts;
        ts.addStringToken("test", TokenType::Keyword);
        ts.addStringToken("test_str", TokenType::Keyword);
        ts.addStringToken("another_test_str", TokenType::Keyword);
        
        MatchedToken mt;
        REQUIRE(ts.match("nomatch", &mt) == MatchResult::NoMatch);
    }

    SECTION("Matches when a matching token is added") {
        TokenSet ts;
        ts.addStringToken("test", TokenType::Keyword);
        ts.addStringToken("test_str", TokenType::Keyword);
        ts.addStringToken("another_test_str", TokenType::Keyword);
        
        MatchedToken mt;
        REQUIRE(ts.match("test_str", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
    }

    SECTION("Preserves token subtype") {
        TokenSet ts;
        ts.addStringToken("test", TokenType::Keyword, 0);
        ts.addStringToken("test_str", TokenType::Keyword, 1);
        ts.addStringToken("another_test_str", TokenType::Keyword, 2);
        
        MatchedToken mt;
        REQUIRE(ts.match("test_str", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == 1);
        REQUIRE(mt.type == TokenType::Keyword);
    }

    SECTION("Ignores whitespace") {
        TokenSet ts;
        ts.addStringToken("test", TokenType::Keyword);
        ts.addStringToken("test_str", TokenType::Keyword);
        ts.addStringToken("another_test_str", TokenType::Keyword);
        
        MatchedToken mt;
        REQUIRE(ts.match(" test_str ", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 1);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
        
        REQUIRE(ts.match("\n\r\ttest_str\n", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 3);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
        
        REQUIRE(ts.match("\n\r\ttest_str\r", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 3);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
        
        REQUIRE(ts.match("\n\r\ttest_str\t", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 3);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
    }

    SECTION("Doesn't match a shorter token that's contained within a longer token when the input is the longer token") {
        TokenSet ts;
        ts.addStringToken("test", TokenType::Keyword);
        ts.addStringToken("test_str", TokenType::Keyword);
        ts.addStringToken("another_test_str", TokenType::Keyword);

        MatchedToken mt;
        
        REQUIRE(ts.match("test_str", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
    }

    SECTION("Doesn't match a longer token that contains a shorter token when the input is the shorter token") {
        TokenSet ts;
        ts.addStringToken("test", TokenType::Keyword);
        ts.addStringToken("test_str", TokenType::Keyword);
        ts.addStringToken("another_test_str", TokenType::Keyword);

        MatchedToken mt;
        
        REQUIRE(ts.match("test", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 4);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
    }

    SECTION("Matches ranged tokens") {
        TokenSet ts;
        ts.addStringToken("'", "'", "\\", TokenType::StringLiteral);

        MatchedToken mt;
        
        REQUIRE(ts.match(" 'abc def'", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 1);
        REQUIRE(mt.length == 9);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.contentBeginOffset == 2);
        REQUIRE(mt.contentEndOffset == 9);
        REQUIRE(mt.type == TokenType::StringLiteral);
    }

    SECTION("Matches ranged token escape characters") {
        TokenSet ts;
        ts.addStringToken("'", "'", "\\$", TokenType::StringLiteral);

        MatchedToken mt;
        
        REQUIRE(ts.match(" 'abc\\'def$''", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 1);
        REQUIRE(mt.length == 12);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.contentBeginOffset == 2);
        REQUIRE(mt.contentEndOffset == 12);
        REQUIRE(mt.type == TokenType::StringLiteral);
    }

    SECTION("Unterminated ranged token") {
        TokenSet ts;
        ts.addStringToken("'", "'", "\\", TokenType::StringLiteral);

        MatchedToken mt;
        
        REQUIRE(ts.match(" 'abcdef   ", &mt) == MatchResult::EndNotMatched);
        REQUIRE(mt.offset == 1);
        REQUIRE(mt.length == 10);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.contentBeginOffset == 2);
        REQUIRE(mt.contentEndOffset == 11);
        REQUIRE(mt.type == TokenType::StringLiteral);
    }
}

void testRegexTokens() {
    SECTION("Doesn't match when no matching tokens are added") {
        TokenSet ts;
        ts.addRegexToken("test\\b", TokenType::Keyword);
        ts.addRegexToken("test_str\\b", TokenType::Keyword);
        ts.addRegexToken("another_test_str\\b", TokenType::Keyword);
        
        MatchedToken mt;
        REQUIRE(ts.match("nomatch", &mt) == MatchResult::NoMatch);
    }

    SECTION("Matches when a matching token is added") {
        TokenSet ts;
        ts.addRegexToken("([a-zA-Z_]+)\\w*", TokenType::Identifier);
        
        MatchedToken mt;
        REQUIRE(ts.match("_test1", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 6);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Identifier);
        
        REQUIRE(ts.match("test2", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 5);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Identifier);
        
        REQUIRE(ts.match("_3test", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 6);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Identifier);
        
        REQUIRE(ts.match("_", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 1);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Identifier);
        
        REQUIRE(ts.match("3test", &mt) == MatchResult::NoMatch);
    }

    SECTION("Preserves token subtype") {
        TokenSet ts;
        ts.addRegexToken("test\\b", TokenType::Keyword, 0);
        ts.addRegexToken("test_str\\b", TokenType::Keyword, 1);
        ts.addRegexToken("another_test_str\\b", TokenType::Keyword, 2);

        // int: -?\d+
        // uint: -?\d+u
        // double: -?\d+(?:(?:(?:\.\d*)?(?:[eE][+\-]?\d+))|(?:\.\d*))
        // float: -?\d+(?:(?:(?:\.\d*)?(?:[eE][+\-]?\d+))|(?:\.\d*))f

        constexpr u32 INT_TP = 0;
        constexpr u32 UINT_TP = 1;
        constexpr u32 FLT_TP = 2;
        constexpr u32 DBL_TP = 3;

        ts.addRegexToken("-?\\d+(?:(?:(?:\\.\\d*)?(?:[eE][+\\-]?\\d+))|(?:\\.\\d*))f", TokenType::NumberLiteral, FLT_TP);
        ts.addRegexToken("-?\\d+(?:(?:(?:\\.\\d*)?(?:[eE][+\\-]?\\d+))|(?:\\.\\d*))", TokenType::NumberLiteral, DBL_TP);
        ts.addRegexToken("-?\\d+u", TokenType::NumberLiteral, UINT_TP);
        ts.addRegexToken("-?\\d+", TokenType::NumberLiteral, INT_TP);
        
        MatchedToken mt;
        REQUIRE(ts.match("test_str", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == 1);
        REQUIRE(mt.type == TokenType::Keyword);

        REQUIRE(ts.match("-1", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 2);
        REQUIRE(mt.subType == INT_TP);
        REQUIRE(mt.type == TokenType::NumberLiteral);

        REQUIRE(ts.match("123u", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 4);
        REQUIRE(mt.subType == UINT_TP);
        REQUIRE(mt.type == TokenType::NumberLiteral);

        REQUIRE(ts.match("1.0", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 3);
        REQUIRE(mt.subType == DBL_TP);
        REQUIRE(mt.type == TokenType::NumberLiteral);

        REQUIRE(ts.match("4.0f", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 4);
        REQUIRE(mt.subType == FLT_TP);
        REQUIRE(mt.type == TokenType::NumberLiteral);
    }

    SECTION("Ignores whitespace") {
        TokenSet ts;
        ts.addRegexToken("test\\b", TokenType::Keyword);
        ts.addRegexToken("test_str\\b", TokenType::Keyword);
        ts.addRegexToken("another_test_str\\b", TokenType::Keyword);
        
        MatchedToken mt;
        REQUIRE(ts.match(" test_str ", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 1);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
        
        REQUIRE(ts.match("\n\r\ttest_str\n", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 3);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
        
        REQUIRE(ts.match("\n\r\ttest_str\r", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 3);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
        
        REQUIRE(ts.match("\n\r\ttest_str\t", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 3);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
    }

    SECTION("Doesn't match a shorter token that's contained within a longer token when the input is the longer token") {
        TokenSet ts;
        ts.addRegexToken("test\\b", TokenType::Keyword);
        ts.addRegexToken("test_str\\b", TokenType::Keyword);
        ts.addRegexToken("another_test_str\\b", TokenType::Keyword);

        MatchedToken mt;
        
        REQUIRE(ts.match("test_str", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 8);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
    }

    SECTION("Doesn't match a longer token that contains a shorter token when the input is the shorter token") {
        TokenSet ts;
        ts.addRegexToken("test\\b", TokenType::Keyword);
        ts.addRegexToken("test_str\\b", TokenType::Keyword);
        ts.addRegexToken("another_test_str\\b", TokenType::Keyword);

        MatchedToken mt;
        
        REQUIRE(ts.match("test", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 4);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
    }

    SECTION("Matches ranged tokens") {
        TokenSet ts;
        ts.addRegexToken("<c>", "<\\/c>", TokenType::Comment);

        MatchedToken mt;
        
        REQUIRE(ts.match(" <c>test test test</c>", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 1);
        REQUIRE(mt.length == 21);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.contentBeginOffset == 4);
        REQUIRE(mt.contentEndOffset == 18);
        REQUIRE(mt.type == TokenType::Comment);
    }

    SECTION("Unterminated ranged token") {
        TokenSet ts;
        ts.addRegexToken("<c>", "<\\/c>", TokenType::Comment);

        MatchedToken mt;
        
        REQUIRE(ts.match(" <c>abcdef   ", &mt) == MatchResult::EndNotMatched);
        REQUIRE(mt.offset == 1);
        REQUIRE(mt.length == 12);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.contentBeginOffset == 4);
        REQUIRE(mt.contentEndOffset == 13);
        REQUIRE(mt.type == TokenType::Comment);
    }
}

void testMixedTokens() {
    SECTION("Prefers string tokens over regex ones") {
        TokenSet ts;
        ts.addRegexToken("keyword\\b", TokenType::Keyword);
        ts.addRegexToken("([a-zA-Z_]+)\\w*", TokenType::Identifier);
        
        MatchedToken mt;
        REQUIRE(ts.match("keyword", &mt) == MatchResult::Matched);
        REQUIRE(mt.offset == 0);
        REQUIRE(mt.length == 7);
        REQUIRE(mt.subType == -1);
        REQUIRE(mt.type == TokenType::Keyword);
    }
}

TEST_CASE("Test TokenSet", "[parser]") {
    SECTION("Basic String Tokens") { testBasicStringTokens(); }
    SECTION("Regex Tokens") { testRegexTokens(); }
    SECTION("Mixed Strings and Regex Tokens") { testMixedTokens(); }
}