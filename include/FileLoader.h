#pragma once


#include <cstddef>

namespace flash {

namespace filesystem {

    class FileLoader {
    public:

        ~FileLoader();

        void load(const char* path);

        void* data() const noexcept { return m_data; }

        std::size_t size() const noexcept { return m_size; }

        void clear();

    private:
        std::size_t m_size{0};
        void* m_data;
    };

}

}
