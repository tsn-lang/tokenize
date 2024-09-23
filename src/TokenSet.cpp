#include <tokenize/TokenSet.h>
#include <utils/Array.hpp>
#include <string.h>
#include <regex>

namespace tokenize {
    TokenSet::TokenSet() : m_strSearchTree(nullptr) { }
    TokenSet::~TokenSet() {
        if (m_strSearchTree) delete m_strSearchTree;
        m_strSearchTree = nullptr;
    }

    void TokenSet::addStringToken(const char* matchString, TokenType type, i32 subType) {
        if (m_strSearchTree) {
            delete m_strSearchTree;
            m_strSearchTree = nullptr;
        }

        m_stringTokens.push({
            type,
            subType,
            matchString,
            nullptr,
            nullptr,
            strlen(matchString),
            0
        });
    }

    void TokenSet::addStringToken(const char* beginMatchString, const char* endMatchString, const char* escapeChars, TokenType type, i32 subType) {
        if (m_strSearchTree) {
            delete m_strSearchTree;
            m_strSearchTree = nullptr;
        }

        m_stringTokens.push({
            type,
            subType,
            beginMatchString,
            endMatchString,
            escapeChars,
            strlen(beginMatchString),
            strlen(endMatchString)
        });
    }

    void TokenSet::addRegexToken(const char* matchString, TokenType type, i32 subType) {
        if (m_strSearchTree) {
            delete m_strSearchTree;
            m_strSearchTree = nullptr;
        }

        std::string beginTok = matchString;
        if (beginTok[0] != '^') beginTok = "^" + beginTok;

        m_regexTokens.push({
            type,
            subType,
            std::regex(beginTok),
            std::regex(),
            false
        });
    }

    void TokenSet::addRegexToken(const char* beginMatchString, const char* endMatchString, TokenType type, i32 subType) {
        if (m_strSearchTree) {
            delete m_strSearchTree;
            m_strSearchTree = nullptr;
        }

        std::string beginTok = beginMatchString;
        if (beginTok[0] != '^') beginTok = "^" + beginTok;

        m_regexTokens.push({
            type,
            subType,
            std::regex(beginTok),
            std::regex(endMatchString),
            true
        });
    }

    MatchResult TokenSet::match(const char* input, MatchedToken* outMatch) {
        if (*input == 0 || (m_stringTokens.size() == 0 && m_regexTokens.size() == 0)) return MatchResult::NoMatch;

        u32 offset = 0;
        while (isspace(*input) && *input) {
            input++;
            offset++;
        }
        
        if (*input == 0) return MatchResult::NoMatch;

        MatchResult result = MatchResult::NoMatch;
        result = matchBasic(input, outMatch);
        if (result == MatchResult::NoMatch) result = matchRegex(input, outMatch);

        if (result != MatchResult::NoMatch) {
            outMatch->offset = offset;
            if (outMatch->contentBeginOffset != -1) outMatch->contentBeginOffset += offset;
            if (outMatch->contentEndOffset != -1) outMatch->contentEndOffset += offset;
        }

        return result;
    }

    MatchResult TokenSet::matchBasic(const char* input, MatchedToken* outMatch) {
        if (!m_strSearchTree) initialize();

        i32 strIdx = m_strSearchTree->findMatch(input);
        if (strIdx == -1) return MatchResult::NoMatch;

        StringTokenMatcher& tok = m_stringTokens[strIdx];

        if (tok.end) {
            const char* begin = input;
            input += tok.beginLen;
            bool foundEndToken = false;

            while (*input) {
                bool didEscape = false;
                
                if (tok.escapeChars) {
                    const char* ch = tok.escapeChars;
                    while (*ch) {
                        if (*input == *ch) {
                            didEscape = true;
                            input++;
                            break;
                        }

                        ch++;
                    }
                }

                if (strncmp(input, tok.end, tok.endLen) == 0) {
                    if (didEscape) {
                        input += tok.endLen;
                        continue;
                    }

                    foundEndToken = true;
                    break;
                }
                
                if (didEscape) {
                    // input already incremented
                    continue;
                }

                input++;
            }

            outMatch->type = tok.type;
            outMatch->subType = tok.subType;
            if (!foundEndToken) {
                outMatch->length = u32(input - begin);
                outMatch->contentBeginOffset = tok.beginLen;
                outMatch->contentEndOffset = outMatch->length;
                return MatchResult::EndNotMatched;
            }

            outMatch->length = u32(input - begin) + tok.endLen;
            outMatch->contentBeginOffset = tok.beginLen;
            outMatch->contentEndOffset = outMatch->length - tok.endLen;
            return MatchResult::Matched;
        }

        outMatch->type = tok.type;
        outMatch->subType = tok.subType;
        outMatch->length = tok.beginLen;
        outMatch->contentBeginOffset = -1;
        outMatch->contentEndOffset = -1;
        return MatchResult::Matched;
    }

    MatchResult TokenSet::matchRegex(const char* input, MatchedToken* outMatch) {
        std::cmatch match;

        for (u32 i = 0;i < m_regexTokens.size();i++) {
            RegexTokenMatcher& tok = m_regexTokens[i];
            if (!std::regex_search(input, match, tok.begin)) continue;

            u32 beginPos = u32(match.position());
            u32 beginLen = u32(match.length());

            outMatch->type = tok.type;
            outMatch->subType = tok.subType;
            outMatch->length = beginLen;
            outMatch->contentBeginOffset = -1;
            outMatch->contentEndOffset = -1;

            if (tok.hasEnd) {
                const char* begin = input;
                const char* end = input + beginLen;

                std::cmatch ematch;
                if (!std::regex_search(end, ematch, tok.end)) {
                    while (*end) end++;
                    outMatch->length = u32(end - begin);
                    outMatch->contentBeginOffset = beginPos + beginLen;
                    outMatch->contentEndOffset = outMatch->length;
                    return MatchResult::EndNotMatched;
                }

                u32 endPos = u32(ematch.position());
                u32 endLen = u32(ematch.length());

                end += ematch.position();
                end += ematch.length();

                outMatch->length = u32(end - begin);
                outMatch->contentBeginOffset = beginPos + beginLen;
                outMatch->contentEndOffset = outMatch->length - endLen;
                return MatchResult::Matched;
            }

            return MatchResult::Matched;
        }

        return MatchResult::NoMatch;
    }

    TokenSet::SearchNode::SearchNode() {
        value = 0;
        strTokenIndex = -1;
        memset(followingIndices, 0, sizeof(u8) * 255);
    }

    TokenSet::SearchNode::~SearchNode() {
        for (u32 i = 0;i < following.size();i++) delete following[i];
        following.clear(true);
    }

    void TokenSet::SearchNode::insert(const char* str, i32 tokenIdx) {
        if (*str == 0) return;
        bool isFinalChar = *(str + 1) == 0 || isspace(*(str + 1));

        u8 idx = followingIndices[*str];
        if (idx) {
            idx--;

            if (isFinalChar) {
                if (following[idx]->strTokenIndex != -1) {
                    // duplicate string
                    return;
                }

                following[idx]->strTokenIndex = tokenIdx;
                return;
            }

            following[idx]->insert(str + 1, tokenIdx);
            return;
        }

        SearchNode* node = new SearchNode();
        node->value = *str;
        if (isFinalChar) node->strTokenIndex = tokenIdx;
        else node->insert(str + 1, tokenIdx);

        following.push(node);
        followingIndices[node->value] = u8(following.size());
    }

    i32 TokenSet::SearchNode::findMatch(const char* str) {
        if (*str == 0) return -1;

        char nextCh = *(str + 1);
        bool isFinalChar = nextCh == 0 || isspace(nextCh);

        u8 idx = followingIndices[*str];
        if (!idx) return -1;
        idx--;
        
        if (isFinalChar) {
            return following[idx]->strTokenIndex;
        }

        if (following[idx]->strTokenIndex != -1) {
            bool curIsWord = isalpha(*str) || *str == '_';
            bool nextIsWord = isalpha(nextCh) || nextCh == '_';
            // see if we're at a token boundary
            if (curIsWord != nextIsWord) {
                // yes
                return following[idx]->strTokenIndex;
            }

            bool curIsNum = (*str >= '0' && *str <= '9');
            bool nextIsNum = (nextCh >= '0' && nextCh <= '9');
            if (curIsNum != nextIsNum) {
                // yes
                return following[idx]->strTokenIndex;
            }

            if (!curIsWord && !nextIsWord && !curIsNum && !nextIsNum) {
                // maybe?
                if (following[idx]->following.size() == 0) {
                    // nothing else to match with, everything up to now matched
                    return following[idx]->strTokenIndex;
                }

                i32 matchIdx = following[idx]->findMatch(str + 1);
                if (matchIdx != -1) {
                    // there was more to match with
                    return matchIdx;
                }

                // nothing else matched, everything up to now matched
                return following[idx]->strTokenIndex;
            }
        }

        return following[idx]->findMatch(str + 1);
    }

    void TokenSet::initialize() {
        m_strSearchTree = new SearchNode();

        for (u32 i = 0;i < m_stringTokens.size();i++) {
            m_strSearchTree->insert(m_stringTokens[i].begin, i32(i));
        }
    }
};