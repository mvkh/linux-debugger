#ifndef SDB_BREAKPOINT_HPP
#define SDB_BREAKPOINT_HPP

#include <cstdint>
#include <cstddef>
#include <string>
#include <filesystem>
#include <libsdb/stoppoint_collection.hpp>
#include <libsdb/breakpoint_site.hpp>
#include <libsdb/types.hpp>

namespace sdb
{
    class target;

    class breakpoint
    {
        public:

            using id_type = std::int32_t;

            virtual ~breakpoint() = default;
            
            breakpoint() = delete;
            breakpoint(const breakpoint&) = delete;
            breakpoint& operator=(const breakpoint&) = delete;

            id_type id() const { return id_; }
            void enable();
            void disable();

            bool is_enabled() const { return is_enabled_; }
            bool is_hardware() const { return is_hardware_; }
            bool is_internal() const { return is_internal_; }

            virtual void resolve() = 0;

            stoppoint_collection<breakpoint_site, false>& breakpoint_sites() { return breakpoint_sites_; }
            const stoppoint_collection<breakpoint_site, false>& breakpoint_sites() const { return breakpoint_sites_; }

            bool at_address(virt_addr addr) const { return breakpoint_sites_.contains_address(addr); }
            bool in_range(virt_addr low, virt_addr high) const { return !breakpoint_sites_.get_in_region(low, high).empty(); }

        protected:

            friend target;

            breakpoint(target& tgt, bool is_hardware = false, bool is_internal = false);

            id_type id_;
            target* target_;
            bool is_enabled_ = false;
            bool is_hardware_ = false;
            bool is_internal_ = false;
            stoppoint_collection<breakpoint_site, false> breakpoint_sites_;
            breakpoint_site::id_type next_site_id_ = 1;
    };

    class function_breakpoint: public breakpoint
    {
        private:

            friend target;

            std::string function_name_;

            function_breakpoint(target& tgt, std::string function_name, bool is_hardware = false, bool is_internal = false):
                breakpoint(tgt, is_hardware, is_internal), function_name_(std::move(function_name))
            {
                resolve();
            }

        public:

            void resolve() override;

            std::string_view function_name() const { return function_name_; }
    }

    class line_breakpoint: public breakpoint
    {
        private:

            friend target;

            std::filesystem::path file_;
            std::size_t line_;

            line_breakpoint(target& tgt, std::filesystem::path file, std::size_t size, bool is_hardware = false, bool is_internal = false):
                breakpoint(tgt, is_hardware, is_internal), file_(std::move(file)), line_(line)
            {
                resolve();
            }

        public:

            void resolve() override;

            const std::filesystem::path file() const { return file_; }
            std::size_t line() const { return line_; }
    }

    class address_breakpoint: public breakpoint
    {
        private:

            friend target;

            virt_addr address_;

            address_breakpoint(target& tgt, virt_addr address, bool is_hardware = false, bool is_internal = false):
                breakpoint(tgt, is_hardware, is_internal), address_(address)
            {
                resolve();
            }

        public:

            void resolve() override;

            virt_addr address() const { return address_; }
    }
}

#endif