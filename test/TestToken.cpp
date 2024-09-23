#include "Common.h"
#include <tokenize/Token.h>
#include <tokenize/Resource.h>

using namespace tokenize;

TEST_CASE("Test Token methods", "[tokenize]") {
    SECTION("toString returns null when the location is for a different resource id") {
        Resource src("abcd\nefgh\nhijk", 0);

        Token tok;
        memset(&tok, 0, sizeof(Token));
        tok.location.resourceId = 1;
        tok.source = &src;

        REQUIRE(tok.toString().size() == 0);
    }

    SECTION("toString returns the correct string") {
        Resource src("abcd\nefgh\nhijk", 0);

        Token tok;
        memset(&tok, 0, sizeof(Token));
        tok.location.resourceId = 0;
        tok.location.startBufferPosition = 5;
        tok.location.endBufferPosition = 9;
        tok.source = &src;

        REQUIRE(tok.toString() == "efgh");
        
        tok.location.startBufferPosition = 2;
        tok.location.endBufferPosition = 9;
        REQUIRE(tok.toString() == "cd\nefgh");
    }
    
    SECTION("getContentString returns null when the location is for a different resource id") {
        Resource src("abcd\nefgh\nhijk", 0);

        Token tok;
        memset(&tok, 0, sizeof(Token));
        tok.location.resourceId = 1;
        tok.source = &src;

        REQUIRE(tok.getContentString().size() == 0);
    }

    SECTION("getContentString returns the correct string") {
        Resource src("ab'cd\nef'gh\nhijk", 0);

        Token tok;
        memset(&tok, 0, sizeof(Token));
        tok.location.resourceId = 0;
        tok.location.startBufferPosition = 2;
        tok.location.endBufferPosition = 8;
        tok.contentBeginOffset = 3;
        tok.contentLength = 5;
        tok.source = &src;

        REQUIRE(tok.getContentString() == "cd\nef");
    }
}