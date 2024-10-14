#pragma once
#include <tokenize/types.h>
#include <tokenize/SourceLocation.h>
#include <utils/Array.h>
#include <regex>

namespace tokenize {
    struct StringTokenMatcher {
        TokenType type;
        i32 subType;
        const char* begin;
        const char* end;
        const char* escapeChars;
        u32 beginLen;
        u32 endLen;
    };

    struct RegexTokenMatcher {
        TokenType type;
        i32 subType;
        std::regex begin;
        std::regex end;
        bool hasEnd;
    };

    struct MatchedToken {
        TokenType type;
        i32 subType;
        u32 offset;
        u32 length;
        i32 contentBeginOffset;
        i32 contentEndOffset;
    };

    enum class MatchResult {
        NoMatch,
        Matched,
        EndNotMatched
    };

    class TokenSet {
        public:
            TokenSet();
            ~TokenSet();

            void addStringToken(const char* matchString, TokenType type, i32 subType = -1);
            void addStringToken(const char* beginMatchString, const char* endMatchString, const char* escapeChars, TokenType type, i32 subType = -1);
            void addRegexToken(const char* matchString, TokenType type, i32 subType = -1);
            void addRegexToken(const char* beginMatchString, const char* endMatchString, TokenType type, i32 subType = -1);
            void addCustomToken(MatchResult (*matchFunc)(const char* input, MatchedToken* outMatch));
            
            MatchResult match(const char* input, MatchedToken* outMatch);

        protected:
            MatchResult matchBasic(const char* input, MatchedToken* outMatch);
            MatchResult matchCustom(const char* input, MatchedToken* outMatch);
            MatchResult matchRegex(const char* input, MatchedToken* outMatch);

            struct SearchNode {
                SearchNode();
                ~SearchNode();

                void insert(const char* str, i32 tokenIdx);
                i32 findMatch(const char* str);

                char value;
                Array<SearchNode*> following;
                u8 followingIndices[255];
                i32 strTokenIndex;
            };

            void initialize();

            SearchNode* m_strSearchTree;
            Array<StringTokenMatcher> m_stringTokens;
            Array<RegexTokenMatcher> m_regexTokens;
            Array<MatchResult (*)(const char* input, MatchedToken* outMatch)> m_customTokens;
    };
};