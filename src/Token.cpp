#include <tokenize/Token.h>
#include <tokenize/Resource.h>

namespace tokenize {
    String Token::toString() const {
        return source->getStringAtLocation(location);
    }
    
    String Token::getContentString() const {
        if (location.resourceId != source->getResourceId()) {
            return String::View(nullptr, 0);
        }
        
        if (contentBeginOffset == -1) {
            return source->getStringAtLocation(location);
        }

        return String::View(
            source->getContents() + contentBeginOffset,
            contentLength
        );
    }
};