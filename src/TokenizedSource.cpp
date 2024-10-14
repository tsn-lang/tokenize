#include <tokenize/TokenizedSource.h>
#include <tokenize/Token.h>
#include <tokenize/TokenSet.h>
#include <tokenize/Resource.h>
#include <tokenize/SourceException.h>
#include <utils/Array.hpp>

namespace tokenize {
    TokenizedSource::TokenizedSource() : m_src(nullptr), m_tokenSet(nullptr), m_pool(sizeof(Token), 512, true) {
    }

    TokenizedSource::~TokenizedSource() {
        reset();
    }

    void TokenizedSource::init(Resource* src, TokenSet* tokenSet) {
        if (m_src) reset();
        m_src = src;
        m_tokenSet = tokenSet;

        const char* inputBegin = src->getContents();
        const char* input = inputBegin;
        while (*input) {
            while (*input && isspace(*input)) input++;

            u32 beginOffset = u32(input - inputBegin);

            if (*input == 0) {
                Token* eoi = (Token*)m_pool.alloc();
                eoi->location.resourceId = src->getResourceId();
                eoi->source = src;
                eoi->subType = -1;
                eoi->contentBeginOffset = eoi->contentLength = -1;
                eoi->type = TokenType::EndOfInput;
                m_tokens.push(eoi);
                return;
            }

            MatchedToken mt;
            MatchResult mr = tokenSet->match(input, &mt);

            if (mr == MatchResult::NoMatch) {
                throw SourceException(
                    src,
                    src->calculateSourceLocationFromRange(beginOffset, beginOffset),
                    "Invalid Token"
                );
            } else if (mr == MatchResult::EndNotMatched) {
                throw SourceException(
                    src,
                    src->calculateSourceLocationFromRange(
                        beginOffset + mt.offset,
                        beginOffset + mt.offset + mt.length
                    ),
                    "Ranged token not terminated"
                );
            }
            
            u32 endOffset = beginOffset + mt.length;

            Token* tok = (Token*)m_pool.alloc();
            tok->location = src->calculateSourceLocationFromRange(beginOffset, endOffset);
            tok->source = src;
            tok->type = mt.type;
            tok->subType = mt.subType;
            
            if (mt.contentBeginOffset == -1) {
                tok->contentBeginOffset = -1;
                tok->contentLength = -1;
            } else {
                tok->contentBeginOffset = beginOffset + mt.contentBeginOffset;
                tok->contentLength = mt.contentEndOffset - mt.contentBeginOffset;
            }

            m_tokens.push(tok);
            input += mt.length;
        }

        Token* eoi = (Token*)m_pool.alloc();
        eoi->location.resourceId = src->getResourceId();
        eoi->source = m_src;
        eoi->subType = -1;
        eoi->contentBeginOffset = eoi->contentLength = -1;
        eoi->type = TokenType::EndOfInput;
        m_tokens.push(eoi);
    }

    void TokenizedSource::reset() {
        if (!m_src) return;
        m_src = nullptr;
        m_tokens.clear(true);
        m_pool.reset();
    }

    Resource* TokenizedSource::getSource() const {
        return m_src;
    }

    const Array<Token*>& TokenizedSource::getTokens() const {
        return m_tokens;
    }
};