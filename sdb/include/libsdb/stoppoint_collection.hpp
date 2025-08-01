#ifndef SDB_STOPPOINT_COLLECTION_HPP
#define SDB_STOPPOINT_COLLECTION_HPP

#include <vector>
#include <memory>
#include <algorithm>
#include <libsdb/types.hpp>
#include <libsdb/error.hpp>

namespace sdb
{
    template <class Stoppoint>
    class stoppoint_collection {
        private:
            using points_t = std::vector<std::unique_ptr<Stoppoint>>;
            points_t stoppoints_;

            typename points_t::iterator find_by_id(typename Stoppoint::id_type id);
            typename points_t::const_iterator find_by_id(typename Stoppoint::id_type id) const;
            typename points_t::iterator find_by_address(virt_addr address);
            typename points_t::const_iterator find_by_address(virt_addr address) const;

        public:
            Stoppoint& push(std::unique_ptr<Stoppoint> bs)
            {
                stoppoints_.push_back(std::move(bs));
                return *stoppoints_.back();
            }

            bool contains_id(typename Stoppoint::id_type id) const;
            bool contains_address(virt_addr address) const;
            bool enabled_stoppoint_at_address(virt_addr address) const;

            Stoppoint& get_by_id(typename Stoppoint::id_type id);
            const Stoppoint& get_by_id(typename Stoppoint::id_type id) const;
            Stoppoint& get_by_address(virt_addr address);
            const Stoppoint& get_by_address(virt_addr address) const;
            std::vector<Stoppoint*> get_in_region(virt_addr low, virt_addr high) const;

            void remove_by_id(typename Stoppoint::id_type id);
            void remove_by_address(virt_addr address);

            template <class F>
            void for_each(F f);
            template <class F>
            void for_each(F f) const;

            std::size_t size() const { return stoppoints_.size(); }
            bool empty() const { return stoppoints_.empty(); }
    };

    template <class Stoppoint>
    auto stoppoint_collection<Stoppoint>::find_by_id(typename Stoppoint::id_type id) -> typename points_t::iterator
    {
        return std::find_if(begin(stoppoints_), end(stoppoints_), [=](auto& point) { return (point->id() == id); });
    }

    template <class Stoppoint>
    auto stoppoint_collection<Stoppoint>::find_by_id(typename Stoppoint::id_type id) const -> typename points_t::const_iterator
    {
        return const_cast<stoppoint_collection*>(this)->find_by_id(id);
    }

    template <class Stoppoint>
    auto stoppoint_collection<Stoppoint>::find_by_address(virt_addr address) -> typename points_t::iterator
    {
        return std::find_if(begin(stoppoints_), end(stoppoints_), [=](auto& point) { return point->at_address(address); });
    }

    template <class Stoppoint>
    auto stoppoint_collection<Stoppoint>::find_by_address(virt_addr address) const -> typename points_t::const_iterator
    {
        return const_cast<stoppoint_collection*>(this)->find_by_address(address);
    }

    template <class Stoppoint>
    bool stoppoint_collection<Stoppoint>::contains_id(typename Stoppoint::id_type id) const
    {
        return (find_by_id(id) != end(stoppoints_));
    }

    template <class Stoppoint>
    bool stoppoint_collection<Stoppoint>::contains_address(virt_addr address) const
    {
        return (find_by_address(address) != end(stoppoints_));
    }

    template <class Stoppoint>
    bool stoppoint_collection<Stoppoint>::enabled_stoppoint_at_address(virt_addr address) const
    {
        return (contains_address(address) and get_by_address(address).is_enabled());
    }

    template <class Stoppoint>
    Stoppoint& stoppoint_collection<Stoppoint>::get_by_id(typename Stoppoint::id_type id)
    {
        auto it = find_by_id(id);
        if (it == end(stoppoints_)) error::send("Invalid stoppoint id");
        return **it;
    }

    template <class Stoppoint>
    const Stoppoint& stoppoint_collection<Stoppoint>::get_by_id(typename Stoppoint::id_type id) const
    {
        return const_cast<stoppoint_collection*>(this)->get_by_id(id);
    }

    template <class Stoppoint>
    Stoppoint& stoppoint_collection<Stoppoint>::get_by_address(virt_addr address)
    {
        auto it = find_by_address(address);
        if (it == end(stoppoints_)) error::send("Stoppoint with given address not found");
        return **it;
    }

    template <class Stoppoint>
    const Stoppoint& stoppoint_collection<Stoppoint>::get_by_address(virt_addr address) const
    {
        return const_cast<stoppoint_collection*>(this)->get_by_address(address);
    }

    template <class Stoppoint>
    std::vector<Stoppoint*> stoppoint_collection<Stoppoint>::get_in_region(virt_addr low, virt_addr high) const
    {
        std::vector<Stoppoint*> ret;
        for (auto& site: stoppoints_)
        {
            if (site->in_range(low, high))
            {
                ret.push_back(&*site);
            }
        }
        return ret;
    }

    template <class Stoppoint>
    void stoppoint_collection<Stoppoint>::remove_by_id(typename Stoppoint::id_type id) 
    {
        auto it = find_by_id(id);
        (**it).disable();
        stoppoints_.erase(it);
    }

    template <class Stoppoint>
    void stoppoint_collection<Stoppoint>::remove_by_address(virt_addr address) 
    {
        auto it = find_by_address(address);
        (**it).disable();
        stoppoints_.erase(it);
    }

    template <class Stoppoint>
    template <class F>
    void stoppoint_collection<Stoppoint>::for_each(F f)
    {
        for (auto& point: stoppoints_)
        {
            f(*point);
        }
    }

    template <class Stoppoint>
    template <class F>
    void stoppoint_collection<Stoppoint>::for_each(F f) const
    {
        for (const auto& point: stoppoints_)
        {
            f(*point);
        }
    }
}

#endif