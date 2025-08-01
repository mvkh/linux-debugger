#ifndef SDB_TYPES_HPP
#define SDB_TYPES_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <cassert>

namespace sdb
{
    using byte64 = std::array<std::byte, 8>;
    using byte128 = std::array<std::byte, 16>;

    enum class stoppoint_mode
    {
        write, read_write, execute
    };

    class elf;
    class file_addr;

    class virt_addr
    {
        private:
            std::uint64_t addr_ = 0;

        public:
            virt_addr() = default;;
            explicit virt_addr(std::uint64_t addr) : addr_(addr) {}

            std::uint64_t addr() const { return addr_; }

            virt_addr operator+(std::int64_t offset) const { return virt_addr(addr_ + offset); }
            virt_addr operator-(std::int64_t offset) const { return virt_addr(addr_ - offset); }

            virt_addr operator+=(std::int64_t offset) { addr_ += offset; return *this; }
            virt_addr operator-=(std::int64_t offset) { addr_ -= offset; return *this; }

            bool operator==(const virt_addr& other) const { return (addr_ == other.addr_); }
            bool operator!=(const virt_addr& other) const { return (addr_ != other.addr_); }

            bool operator<(const virt_addr& other) const { return (addr_ < other.addr_); }
            bool operator<=(const virt_addr& other) const { return (addr_ <= other.addr_); }

            bool operator>(const virt_addr& other) const { return (addr_ > other.addr_); }
            bool operator>=(const virt_addr& other) const { return (addr_ >= other.addr_); }

            file_addr to_file_addr(const elf& obj) const;
    };

    class file_addr
    {
        private:

            const elf* elf_ = nullptr;
            std::uint64_t addr_ = 0;

        public:

            file_addr() = default;
            file_addr(const elf& obj, std::uint64_t addr): elf_(&obj), addr_(addr) {}

            std::uint64_t addr() const { return addr_; }
            const elf* elf_file() const { return elf_; }

            virt_addr to_virt_addr() const;

            file_addr operator+(std::int64_t offset) const { return file_addr(*elf_, addr_ + offset); }
            file_addr operator-(std::int64_t offset) const { return file_addr(*elf_, addr_ - offset); }

            file_addr& operator+=(std::int64_t offset) { addr_ += offset; return *this; }
            file_addr& operator-=(std::int64_t offset) { addr_ -= offset; return *this; }

            bool operator==(const file_addr& other) const { return ((elf_ == other.elf_) and (addr_ == other.addr_)); }
            bool operator!=(const file_addr& other) const { return ((elf_ != other.elf_) or (addr_ != other.addr_)); }

            bool operator<(const file_addr& other) const { assert(elf_ == other.elf_); return (addr_ < other.addr_); }
            bool operator<=(const file_addr& other) const { assert(elf_ == other.elf_); return (addr_ <= other.addr_); }
            bool operator>(const file_addr& other) const { assert(elf_ == other.elf_); return (addr_ > other.addr_); }
            bool operator>=(const file_addr& other) const { assert(elf_ == other.elf_); return (addr_ >= other.addr_); }
    };

    class file_offset
    {
        private:

            const elf* elf_ = nullptr;
            std::uint64_t off_ = 0;

        public:
        
            file_offset() = default;
            file_offset(const elf& obj, std::uint64_t off): elf_(&obj), off_(off) {}

            std::uint64_t off() const { return off_; }
            const elf* elf_file() const { return elf_; }
    };

    template <class T>
    class span 
    {
        private:
            T* data_ = nullptr;
            std::size_t size_ = 0;

        public:
            span() = default;
            span(T* data, std::size_t size): data_(data), size_(size) {}
            span(T* data, T* end): data_(data), size_(end - data) {}
            template <class U>
            span(const std::vector<U>& vec): data_(vec.data()), size_(vec.size()) {}

            T* begin() const { return data_; }
            T* end() const { return data_ + size_; }
            std::size_t size() const { return size_; }
            T& operator[](std::size_t n) { return *(data_ + n); }
    };
}

#endif