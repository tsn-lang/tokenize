#include "Common.h"
#include <tokenize/Resource.h>
#include <tokenize/Token.h>
#include <tokenize/TokenSet.h>
#include <tokenize/TokenizedSource.h>
#include <tokenize/SourceException.h>
#include <utils/Array.hpp>
using namespace tokenize;

TEST_CASE("Test Resource methods", "[tokenize]") {
    SECTION("Calculates the length correctly") {
        Resource src("abcde", 0);
        REQUIRE(src.getLength() == 5);
    }
    
    SECTION("Clones the contents passed to the constructor") {
        const char* input = "abcde";
        Resource src(input, 0);
        REQUIRE(src.getContents() != input);
        REQUIRE(strcmp(input, src.getContents()) == 0);
    }
    
    SECTION("getPointerToLocation returns null if specified location does not refer to this resource") {
        const char* input = "abcde";
        Resource src(input, 0);

        SourceLocation loc;
        memset(&loc, 0, sizeof(SourceLocation));

        loc.resourceId = 1;
        REQUIRE(src.getPointerToLocation(loc) == nullptr);
    }
    
    SECTION("getPointerToLocation returns null if specified location is out of range") {
        const char* input = "abcde";
        Resource src(input, 0);

        SourceLocation loc;
        memset(&loc, 0, sizeof(SourceLocation));
        loc.resourceId = 0;
        loc.startBufferPosition = 10;

        REQUIRE(src.getPointerToLocation(loc) == nullptr);
    }
    
    SECTION("getPointerToLocation returns correct address when the input is valid") {
        const char* input = "abcde";
        Resource src(input, 0);

        SourceLocation loc;
        memset(&loc, 0, sizeof(SourceLocation));
        loc.resourceId = 0;
        loc.startBufferPosition = 3;

        const char* ptr = src.getPointerToLocation(loc);
        REQUIRE(ptr == src.getContents() + loc.startBufferPosition);
    }
    
    SECTION("getStringAtLocation returns null if specified location does not refer to this resource") {
        const char* input = "abcde";
        Resource src(input, 0);

        SourceLocation loc;
        memset(&loc, 0, sizeof(SourceLocation));

        loc.resourceId = 1;
        REQUIRE(src.getStringAtLocation(loc).size() == 0);
    }
    
    SECTION("getStringAtLocation returns null if specified location is out of range") {
        const char* input = "abcde";
        Resource src(input, 0);

        SourceLocation loc;
        memset(&loc, 0, sizeof(SourceLocation));
        loc.resourceId = 0;
        loc.startBufferPosition = 10;

        REQUIRE(src.getStringAtLocation(loc).size() == 0);
    }
    
    SECTION("getStringAtLocation returns correct string when the input is valid") {
        const char* input = "abcde";
        Resource src(input, 0);

        SourceLocation loc;
        memset(&loc, 0, sizeof(SourceLocation));
        loc.resourceId = 0;
        loc.startBufferPosition = 3;
        loc.endBufferPosition = 4;

        REQUIRE(src.getStringAtLocation(loc) == "d");

        loc.startBufferPosition = 2;
        loc.endBufferPosition = 4;

        REQUIRE(src.getStringAtLocation(loc) == "cd");

        Resource src1 = Resource("abc\ndef", 0);
        loc.startBufferPosition = 1;
        loc.endBufferPosition = 5;

        REQUIRE(src1.getStringAtLocation(loc) == "bc\nd");
    }

    SECTION("calculateSourceLocationFromRange returns invalid location when the input invalid") {
        const char* input = "abcde";
        Resource src(input, 0);

        SourceLocation loc;
        
        loc = src.calculateSourceLocationFromRange(0, 10);
        REQUIRE(loc.resourceId == u32(-1));

        loc = src.calculateSourceLocationFromRange(10, 0);
        REQUIRE(loc.resourceId == u32(-1));
    }

    SECTION("calculateSourceLocationFromRange returns the correct location when the input is valid") {
        const char* input = "abc\ndef\nghi";
        Resource src(input, 0);

        SourceLocation loc;

        loc = src.calculateSourceLocationFromRange(1, 1);
        REQUIRE(loc.resourceId == 0);
        REQUIRE(loc.startBufferPosition == 1);
        REQUIRE(loc.endBufferPosition == 1);
        REQUIRE(loc.startLine == 0);
        REQUIRE(loc.endLine == 0);
        REQUIRE(loc.startColumn == 1);
        REQUIRE(loc.endColumn == 1);

        loc = src.calculateSourceLocationFromRange(5, 5);
        REQUIRE(loc.resourceId == 0);
        REQUIRE(loc.startBufferPosition == 5);
        REQUIRE(loc.endBufferPosition == 5);
        REQUIRE(loc.startLine == 1);
        REQUIRE(loc.endLine == 1);
        REQUIRE(loc.startColumn == 1);
        REQUIRE(loc.endColumn == 1);
        
        loc = src.calculateSourceLocationFromRange(0, 3);
        REQUIRE(loc.resourceId == 0);
        REQUIRE(loc.startBufferPosition == 0);
        REQUIRE(loc.endBufferPosition == 3);
        REQUIRE(loc.startLine == 0);
        REQUIRE(loc.endLine == 0);
        REQUIRE(loc.startColumn == 0);
        REQUIRE(loc.endColumn == 3);

        loc = src.calculateSourceLocationFromRange(1, 7);
        REQUIRE(loc.resourceId == 0);
        REQUIRE(loc.startBufferPosition == 1);
        REQUIRE(loc.endBufferPosition == 7);
        REQUIRE(loc.startLine == 0);
        REQUIRE(loc.endLine == 1);
        REQUIRE(loc.startColumn == 1);
        REQUIRE(loc.endColumn == 3);

        loc = src.calculateSourceLocationFromRange(1, 11);
        REQUIRE(loc.resourceId == 0);
        REQUIRE(loc.startBufferPosition == 1);
        REQUIRE(loc.endBufferPosition == 11);
        REQUIRE(loc.startLine == 0);
        REQUIRE(loc.endLine == 2);
        REQUIRE(loc.startColumn == 1);
        REQUIRE(loc.endColumn == 3);

        loc = src.calculateSourceLocationFromRange(8, 11);
        REQUIRE(loc.resourceId == 0);
        REQUIRE(loc.startBufferPosition == 8);
        REQUIRE(loc.endBufferPosition == 11);
        REQUIRE(loc.startLine == 2);
        REQUIRE(loc.endLine == 2);
        REQUIRE(loc.startColumn == 0);
        REQUIRE(loc.endColumn == 3);
    }

    SECTION("getLine returns null when the specified index is out of range") {
        const char* input = "abc\ndef\nghi";
        Resource src(input, 0);

        REQUIRE(src.getLine(4).size() == 0);
    }

    SECTION("getLine returns the correct string when the input is valid") {
        const char* input = "abc\ndef\nghi\r\njkl";
        Resource src(input, 0);

        REQUIRE(src.getLine(0) == "abc\n");
        REQUIRE(src.getLine(1) == "def\n");
        REQUIRE(src.getLine(2) == "ghi\r\n");
        REQUIRE(src.getLine(3) == "jkl");
    }

    SECTION("getLine returns the correct string when the input is valid") {
        const char* input = "abc\ndef\nghi\r\njkl";
        Resource src(input, 0);

        REQUIRE(src.getLine(0) == "abc\n");
        REQUIRE(src.getLine(1) == "def\n");
        REQUIRE(src.getLine(2) == "ghi\r\n");
        REQUIRE(src.getLine(3) == "jkl");
    }

    SECTION("tokenize throws a SourceException when an unterminated ranged token is found") {
        Resource src("'abc", 0);
        TokenSet ts;
        ts.addStringToken("'", "'", "\\", TokenType::Literal);

        REQUIRE_THROWS_AS(src.tokenize(&ts), SourceException);

        try {
            src.tokenize(&ts);
        } catch (const SourceException& e) {
            REQUIRE(e.getSource() == &src);
            SourceLocation loc = e.getLocation();
            REQUIRE(loc.resourceId == 0);
            REQUIRE(loc.startBufferPosition == 0);
            REQUIRE(loc.endBufferPosition == 4);
            REQUIRE(loc.startLine == 0);
            REQUIRE(loc.endLine == 0);
            REQUIRE(loc.startColumn == 0);
            REQUIRE(loc.endColumn == 4);
        }
    }

    SECTION("tokenize throws a SourceException when an unrecognized token is found") {
        Resource src("'abc' test", 0);
        TokenSet ts;
        ts.addStringToken("'", "'", "\\", TokenType::Literal);

        REQUIRE_THROWS_AS(src.tokenize(&ts), SourceException);

        try {
            src.tokenize(&ts);
        } catch (const SourceException& e) {
            REQUIRE(e.getSource() == &src);
            SourceLocation loc = e.getLocation();
            REQUIRE(loc.resourceId == 0);
            REQUIRE(loc.startBufferPosition == 6);
            REQUIRE(loc.endBufferPosition == 6);
            REQUIRE(loc.startLine == 0);
            REQUIRE(loc.endLine == 0);
            REQUIRE(loc.startColumn == 6);
            REQUIRE(loc.endColumn == 6);
        }
    }
    
    SECTION("tokenize throws a SourceException when an unrecognized token is found") {
        Resource src(
            "type Test = {\n"
            "    a: i32;\n"
            "};",
            0
        );

        TokenSet ts;
        ts.addStringToken("type", TokenType::Keyword);
        ts.addRegexToken("([a-zA-Z_]+)\\w*", TokenType::Identifier);
        ts.addStringToken("=", TokenType::Symbol);
        ts.addStringToken("{", TokenType::Symbol);
        ts.addStringToken("}", TokenType::Symbol);
        ts.addStringToken(":", TokenType::Symbol);
        ts.addStringToken(";", TokenType::EndOfStatement);

        TokenizedSource* result = nullptr;
        REQUIRE_NOTHROW(result = src.tokenize(&ts));
        REQUIRE(result != nullptr);
        REQUIRE(result->getSource() == &src);

        const Array<Token*>& tokens = result->getTokens();
        REQUIRE(tokens.size() == 11);

        REQUIRE(tokens[0]->type == TokenType::Keyword);
        REQUIRE(tokens[0]->subType == -1);
        REQUIRE(tokens[0]->source == &src);
        REQUIRE(tokens[0]->contentBeginOffset == -1);
        REQUIRE(tokens[0]->contentLength == -1);
        REQUIRE(tokens[0]->toString() == "type");
        REQUIRE(tokens[0]->location.resourceId == 0);
        REQUIRE(tokens[0]->location.startBufferPosition == 0);
        REQUIRE(tokens[0]->location.endBufferPosition == 4);
        REQUIRE(tokens[0]->location.startLine == 0);
        REQUIRE(tokens[0]->location.endLine == 0);
        REQUIRE(tokens[0]->location.startColumn == 0);
        REQUIRE(tokens[0]->location.endColumn == 4);

        REQUIRE(tokens[1]->type == TokenType::Identifier);
        REQUIRE(tokens[1]->subType == -1);
        REQUIRE(tokens[1]->source == &src);
        REQUIRE(tokens[1]->contentBeginOffset == -1);
        REQUIRE(tokens[1]->contentLength == -1);
        REQUIRE(tokens[1]->toString() == "Test");
        REQUIRE(tokens[1]->location.resourceId == 0);
        REQUIRE(tokens[1]->location.startBufferPosition == 5);
        REQUIRE(tokens[1]->location.endBufferPosition == 9);
        REQUIRE(tokens[1]->location.startLine == 0);
        REQUIRE(tokens[1]->location.endLine == 0);
        REQUIRE(tokens[1]->location.startColumn == 5);
        REQUIRE(tokens[1]->location.endColumn == 9);

        REQUIRE(tokens[2]->type == TokenType::Symbol);
        REQUIRE(tokens[2]->subType == -1);
        REQUIRE(tokens[2]->source == &src);
        REQUIRE(tokens[2]->contentBeginOffset == -1);
        REQUIRE(tokens[2]->contentLength == -1);
        REQUIRE(tokens[2]->toString() == "=");
        REQUIRE(tokens[2]->location.resourceId == 0);
        REQUIRE(tokens[2]->location.startBufferPosition == 10);
        REQUIRE(tokens[2]->location.endBufferPosition == 11);
        REQUIRE(tokens[2]->location.startLine == 0);
        REQUIRE(tokens[2]->location.endLine == 0);
        REQUIRE(tokens[2]->location.startColumn == 10);
        REQUIRE(tokens[2]->location.endColumn == 11);

        REQUIRE(tokens[3]->type == TokenType::Symbol);
        REQUIRE(tokens[3]->subType == -1);
        REQUIRE(tokens[3]->source == &src);
        REQUIRE(tokens[3]->contentBeginOffset == -1);
        REQUIRE(tokens[3]->contentLength == -1);
        REQUIRE(tokens[3]->toString() == "{");
        REQUIRE(tokens[3]->location.resourceId == 0);
        REQUIRE(tokens[3]->location.startBufferPosition == 12);
        REQUIRE(tokens[3]->location.endBufferPosition == 13);
        REQUIRE(tokens[3]->location.startLine == 0);
        REQUIRE(tokens[3]->location.endLine == 0);
        REQUIRE(tokens[3]->location.startColumn == 12);
        REQUIRE(tokens[3]->location.endColumn == 13);
        
        REQUIRE(tokens[4]->type == TokenType::Identifier);
        REQUIRE(tokens[4]->subType == -1);
        REQUIRE(tokens[4]->source == &src);
        REQUIRE(tokens[4]->contentBeginOffset == -1);
        REQUIRE(tokens[4]->contentLength == -1);
        REQUIRE(tokens[4]->toString() == "a");
        REQUIRE(tokens[4]->location.resourceId == 0);
        REQUIRE(tokens[4]->location.startBufferPosition == 18);
        REQUIRE(tokens[4]->location.endBufferPosition == 19);
        REQUIRE(tokens[4]->location.startLine == 1);
        REQUIRE(tokens[4]->location.endLine == 1);
        REQUIRE(tokens[4]->location.startColumn == 4);
        REQUIRE(tokens[4]->location.endColumn == 5);
        
        REQUIRE(tokens[5]->type == TokenType::Symbol);
        REQUIRE(tokens[5]->subType == -1);
        REQUIRE(tokens[5]->source == &src);
        REQUIRE(tokens[5]->contentBeginOffset == -1);
        REQUIRE(tokens[5]->contentLength == -1);
        REQUIRE(tokens[5]->toString() == ":");
        REQUIRE(tokens[5]->location.resourceId == 0);
        REQUIRE(tokens[5]->location.startBufferPosition == 19);
        REQUIRE(tokens[5]->location.endBufferPosition == 20);
        REQUIRE(tokens[5]->location.startLine == 1);
        REQUIRE(tokens[5]->location.endLine == 1);
        REQUIRE(tokens[5]->location.startColumn == 5);
        REQUIRE(tokens[5]->location.endColumn == 6);
        
        REQUIRE(tokens[6]->type == TokenType::Identifier);
        REQUIRE(tokens[6]->subType == -1);
        REQUIRE(tokens[6]->source == &src);
        REQUIRE(tokens[6]->contentBeginOffset == -1);
        REQUIRE(tokens[6]->contentLength == -1);
        REQUIRE(tokens[6]->toString() == "i32");
        REQUIRE(tokens[6]->location.resourceId == 0);
        REQUIRE(tokens[6]->location.startBufferPosition == 21);
        REQUIRE(tokens[6]->location.endBufferPosition == 24);
        REQUIRE(tokens[6]->location.startLine == 1);
        REQUIRE(tokens[6]->location.endLine == 1);
        REQUIRE(tokens[6]->location.startColumn == 7);
        REQUIRE(tokens[6]->location.endColumn == 10);
        
        REQUIRE(tokens[7]->type == TokenType::EndOfStatement);
        REQUIRE(tokens[7]->subType == -1);
        REQUIRE(tokens[7]->source == &src);
        REQUIRE(tokens[7]->contentBeginOffset == -1);
        REQUIRE(tokens[7]->contentLength == -1);
        REQUIRE(tokens[7]->toString() == ";");
        REQUIRE(tokens[7]->location.resourceId == 0);
        REQUIRE(tokens[7]->location.startBufferPosition == 24);
        REQUIRE(tokens[7]->location.endBufferPosition == 25);
        REQUIRE(tokens[7]->location.startLine == 1);
        REQUIRE(tokens[7]->location.endLine == 1);
        REQUIRE(tokens[7]->location.startColumn == 10);
        REQUIRE(tokens[7]->location.endColumn == 11);
        
        REQUIRE(tokens[8]->type == TokenType::Symbol);
        REQUIRE(tokens[8]->subType == -1);
        REQUIRE(tokens[8]->source == &src);
        REQUIRE(tokens[8]->contentBeginOffset == -1);
        REQUIRE(tokens[8]->contentLength == -1);
        REQUIRE(tokens[8]->toString() == "}");
        REQUIRE(tokens[8]->location.resourceId == 0);
        REQUIRE(tokens[8]->location.startBufferPosition == 26);
        REQUIRE(tokens[8]->location.endBufferPosition == 27);
        REQUIRE(tokens[8]->location.startLine == 2);
        REQUIRE(tokens[8]->location.endLine == 2);
        REQUIRE(tokens[8]->location.startColumn == 0);
        REQUIRE(tokens[8]->location.endColumn == 1);
        
        REQUIRE(tokens[9]->type == TokenType::EndOfStatement);
        REQUIRE(tokens[9]->subType == -1);
        REQUIRE(tokens[9]->source == &src);
        REQUIRE(tokens[9]->contentBeginOffset == -1);
        REQUIRE(tokens[9]->contentLength == -1);
        REQUIRE(tokens[9]->toString() == ";");
        REQUIRE(tokens[9]->location.resourceId == 0);
        REQUIRE(tokens[9]->location.startBufferPosition == 27);
        REQUIRE(tokens[9]->location.endBufferPosition == 28);
        REQUIRE(tokens[9]->location.startLine == 2);
        REQUIRE(tokens[9]->location.endLine == 2);
        REQUIRE(tokens[9]->location.startColumn == 1);
        REQUIRE(tokens[9]->location.endColumn == 2);

        delete result;
    }
}