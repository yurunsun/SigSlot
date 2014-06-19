#ifndef _EVENT_H__
#define _EVENT_H__

#include <unordered_map>
#include <set>
#include <functional>

namespace sigslot {
    namespace internal {

        template<typename event_id, class... args>
        class event_base
        {
            typedef event_base<event_id, args...> this_type;
            typedef std::function<void(args...)> function_type;
            typedef std::set<function_type*> sink_set;
            typedef std::unordered_map<event_id, sink_set> sink_map;
        public:
            static bool has_instance()
            {
                return instance_ != nullptr;
            }
            static event_base & instance()
            {
                if (!has_instance()) {
                    instance_ = new event_base;
                }
                return *instance_;
            }

            void publish(event_id id, args... a)
            {
                auto it = sinks_.find(id);
                if (it == sinks_.end()) {
                    return ;
                }

                for (function_type * f : it->second) {
                    (*f)(a...);
                }
            }

            void add_sink(event_id id, function_type * s)
            {
                sinks_[id].insert(s);
            }
            void remove_sink(event_id id, function_type * s)
            {
                sinks_[id].erase(s);
            }
        private:
            static this_type * instance_;
            sink_map sinks_;
        };

        template<typename event_id, class... args> event_base<event_id, args...>* event_base<event_id, args...>::instance_ = nullptr;
    }

    template<typename event_id, class... args>
    class event_sink
    {
        typedef std::function<void(args...)> function_type;
        typedef internal::event_base<event_id, args...> event_base_type;
    public:
        event_sink(event_id id, function_type fun)
            : id_(id)
            , f_(fun)
        {
            event_base_type::instance().add_sink(id_, &f_);
        }

        ~event_sink()
        {
            event_base_type::instance().remove_sink(id_, &f_);
        }

    public:
        event_id id_;
        function_type f_;
    };

    template<typename event_id>
    class event_publisher
    {
    public:
        template<class... args>
        static void publish(event_id id, args... a)
        {
            typedef internal::event_base<event_id, args...> event_base_type;
            if (!event_base_type::has_instance()) {
                return;
            }

            event_base_type::instance().publish(id, a...);
        }
    };
}
#endif
