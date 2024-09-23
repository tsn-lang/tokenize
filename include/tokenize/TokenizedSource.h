#pragma once
#include <tokenize/types.h>
#include <utils/Array.h>

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
            const Array<Token*>& getTokens() const;
        
        protected:
            void createNewPool();
            Token* getNewToken();

            struct TokenPool {
                u8* mem;
                u32 used;
                u32 capacity;
            };

            Resource* m_src;
            Array<Token*> m_tokens;
            Array<TokenPool> m_pools;
    };
};