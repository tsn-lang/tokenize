#include <tokenize/TokenizedSource.h>
#include <tokenize/Token.h>
#include <tokenize/TokenSet.h>
#include <tokenize/Resource.h>
#include <tokenize/SourceException.h>
#include <utils/Array.hpp>

namespace tokenize {
    constexpr u32 TOKEN_POOL_SIZE = 512;

    TokenizedSource::TokenizedSource() : m_src(nullptr) {
    }

    TokenizedSource::~TokenizedSource() {
        reset();
    }

    void TokenizedSource::init(Resource* src, TokenSet* tokenSet) {
        if (m_src) reset();
        createNewPool();
        m_src = src;

        const char* inputBegin = src->getContents();
        const char* input = inputBegin;
        while (*input) {
            while (*input && isspace(*input)) input++;

            u32 beginOffset = u32(input - inputBegin);

            if (*input == 0) {
                Token* eoi = getNewToken();
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

            Token* tok = getNewToken();
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

        Token* eoi = getNewToken();
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

        for (u32 i = 0;i < m_pools.size();i++) {
            delete [] m_pools[i].mem;
        }

        m_pools.clear(true);
    }

    Resource* TokenizedSource::getSource() const {
        return m_src;
    }

    const Array<Token*>& TokenizedSource::getTokens() const {
        return m_tokens;
    }
    
    void TokenizedSource::createNewPool() {
        m_pools.push({
            new u8[sizeof(Token) * TOKEN_POOL_SIZE],
            0,
            TOKEN_POOL_SIZE
        });

        memset(m_pools.last().mem, 0, sizeof(Token) * TOKEN_POOL_SIZE);
    }

    Token* TokenizedSource::getNewToken() {
        if (!m_src) return nullptr;
        TokenPool* lastPool = &m_pools.last();

        if (lastPool->used == lastPool->capacity) {
            createNewPool();
            lastPool = &m_pools.last();
        }

        Token& tok = ((Token*)lastPool->mem)[lastPool->used++];
        return &tok;
    }
};