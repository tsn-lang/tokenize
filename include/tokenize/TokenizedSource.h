#pragma once
#include <tokenize/types.h>
#include <utils/Array.h>
#include <utils/MemoryPool.h>

namespace tokenize {
    class Resource;
    struct Token;
    class TokenSet;

    class TokenizedSource {
        public:
            TokenizedSource();
            ~TokenizedSource();

            void init(Resource* src, TokenSet* tokenSet);
            void reset();

            Resource* getSource() const;
            TokenSet* getTokenSet() const;
            const Array<Token*>& getTokens() const;
        
        protected:
            Resource* m_src;
            TokenSet* m_tokenSet;
            MemoryPool m_pool;
            Array<Token*> m_tokens;
    };
};