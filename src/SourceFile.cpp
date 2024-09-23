#include <tokenize/SourceFile.h>

namespace tokenize {
    SourceFile::SourceFile(const char* contents, u32 resourceId) {
        m_resourceId = resourceId;
        m_length = strlen(contents);
        m_contents = nullptr;

        if (m_length > 0) {
            m_contents = new char[m_length + 1];
            memcpy(m_contents, contents, m_length * sizeof(char));
            m_contents[m_length] = 0;
        }
    }

    SourceFile::~SourceFile() {
        if (m_contents) delete [] m_contents;
        m_contents = nullptr;
    }

    u32 SourceFile::getResourceId() const {
        return m_resourceId;
    }

    const char* SourceFile::getPointerToLocation(const SourceLocation& loc) const {
        if (m_resourceId != loc.resourceId) return nullptr;
        if (loc.startBufferPosition >= m_length) return nullptr;

        return m_contents + loc.startBufferPosition;
    }

    String SourceFile::getStringAtLocation(const SourceLocation& loc) const {
        if (m_resourceId != loc.resourceId) return nullptr;
        if (loc.startBufferPosition >= m_length) return nullptr;
        if (loc.endBufferPosition >= m_length) return nullptr;
        if (loc.endBufferPosition < loc.startBufferPosition) return nullptr;

        String result;
        result.copy(m_contents + loc.startBufferPosition, loc.endBufferPosition - loc.startBufferPosition);
        return result;
    }
};