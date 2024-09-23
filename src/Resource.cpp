#include <tokenize/Resource.h>
#include <tokenize/TokenizedSource.h>
#include <tokenize/SourceException.h>
#include <utils/Array.hpp>

namespace tokenize {
    Resource::Resource(const char* contents, u32 resourceId) {
        m_resourceId = resourceId;
        m_length = strlen(contents);
        m_contents = nullptr;

        if (m_length > 0) {
            m_contents = new char[m_length + 1];
            memcpy(m_contents, contents, m_length * sizeof(char));
            m_contents[m_length] = 0;
        }

        const char* input = m_contents;
        u32 lastLineBegin = 0;
        while (*input) {
            bool isNewLine = false;
            u32 off = 1;
            if (*input == '\n') isNewLine = true;
            else if (*input == '\r' && *(input + 1) == '\n') {
                isNewLine = true;
                off = 2;
            }

            if (isNewLine) {
                m_lineOffsets.push({
                    lastLineBegin,
                    u32(input - m_contents) + off
                });

                lastLineBegin = m_lineOffsets.last().end;
            }

            input += off;
        }

        u32 endLoc = u32(input - m_contents);
        if (endLoc != lastLineBegin) {
            m_lineOffsets.push({
                lastLineBegin,
                endLoc
            });
        }
    }

    Resource::~Resource() {
        if (m_contents) delete [] m_contents;
        m_contents = nullptr;
    }

    u32 Resource::getResourceId() const {
        return m_resourceId;
    }
    
    const char* Resource::getContents() const {
        return m_contents;
    }

    u32 Resource::getLength() const {
        return m_length;
    }

    const char* Resource::getPointerToLocation(const SourceLocation& loc) const {
        if (m_resourceId != loc.resourceId) return nullptr;
        if (loc.startBufferPosition >= m_length) return nullptr;

        return m_contents + loc.startBufferPosition;
    }

    String Resource::getStringAtLocation(const SourceLocation& loc) const {
        if (m_resourceId != loc.resourceId) return String::View(nullptr, 0);
        if (loc.startBufferPosition > m_length) return String::View(nullptr, 0);
        if (loc.endBufferPosition > m_length) return String::View(nullptr, 0);
        if (loc.endBufferPosition < loc.startBufferPosition) return String::View(nullptr, 0);

        return String::View(
            m_contents + loc.startBufferPosition,
            loc.endBufferPosition - loc.startBufferPosition
        );
    }
    
    SourceLocation Resource::calculateSourceLocationFromRange(u32 beginOffset, u32 endOffset) const {
        SourceLocation loc;
        loc.resourceId = u32(-1);
        loc.startBufferPosition = loc.endBufferPosition = 0;
        loc.startColumn = loc.endColumn = 0;
        loc.startLine = loc.endLine = 0;

        if (beginOffset > m_length || endOffset > m_length || endOffset < beginOffset) {
            return loc;
        }

        loc.resourceId = m_resourceId;
        loc.startBufferPosition = beginOffset;
        loc.endBufferPosition = endOffset;

        while (loc.startLine < m_lineOffsets.size()) {
            const LineOffset& ln = m_lineOffsets[loc.startLine];
            if (beginOffset >= ln.begin && beginOffset < ln.end) {
                loc.startColumn = beginOffset - ln.begin;
                break;
            }
            loc.startLine++;
        }

        loc.endLine = loc.startLine;
        
        if (beginOffset == endOffset) {
            loc.endColumn = loc.startColumn;
            return loc;
        }

        while (loc.endLine < m_lineOffsets.size()) {
            const LineOffset& ln = m_lineOffsets[loc.endLine];
            if (endOffset >= ln.begin && endOffset <= ln.end) {
                loc.endColumn = endOffset - ln.begin;
                break;
            }
            loc.endLine++;
        }

        return loc;
    }
    
    String Resource::getLine(u32 index) const {
        if (index >= m_lineOffsets.size()) return String::View(nullptr, 0);
        return String::View(
            m_contents + m_lineOffsets[index].begin,
            m_lineOffsets[index].end - m_lineOffsets[index].begin
        );
    }

    TokenizedSource* Resource::tokenize(TokenSet* tokenSet) const {
        TokenizedSource* tok = new TokenizedSource();

        try {
            tok->init(const_cast<Resource*>(this), tokenSet);
        } catch (const SourceException& exc) {
            delete tok;
            throw exc;
        }

        return tok;
    }
};