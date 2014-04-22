#ifndef _EVENT_H__
#define _EVENT_H__

#include <set>
#include <functional>

namespace sigslot {
    namespace internal {

        template<int event_id, class... args>
        class event_base
        {
            typedef event_base<event_id, args...> this_type;
            typedef std::function<void(args...)> function_type;
            typedef std::set<function_type *> sink_set;
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

            void publish(args... a)
            {
                for (function_type * f : sinks_) {
                    (*f)(a...);
                }
            }

            void add_sink(function_type * s)
            {
                sinks_.insert(s);
            }
            void remove_sink(function_type * s)
            {
                sinks_.erase(s);
            }
        private:
            static this_type * instance_;
            sink_set sinks_;
        };

        template<int event_id, class... args> event_base<event_id, args...>* event_base<event_id, args...>::instance_ = nullptr;
    }

    template<int event_id, class... args>
    class event_sink
    {
        typedef std::function<void(args...)> function_type;
        typedef internal::event_base<event_id, args...> event_base_type;
    public:
        event_sink(function_type fun)
            : f_(fun)
        {
            event_base_type::instance().add_sink(&f_);
        }

        ~event_sink()
        {
            event_base_type::instance().remove_sink(&f_);
        }

    public:
        function_type f_;
    };

    template<int event_id>
    class event_publisher
    {
    public:
        template<class... args>
        static void publish(args... a)
        {
            typedef internal::event_base<event_id, args...> event_base_type;
            if (!event_base_type::has_instance()) {
                return;
            }

            event_base_type::instance().publish(a...);
        }
    };
}
#endif
