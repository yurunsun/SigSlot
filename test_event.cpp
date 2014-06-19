#include "event.h"
#include <iostream>
using namespace std;

class event_listener
{
public:
    void update(bool got)
    {
        cout << "listener got ? " << got << endl;
    }
};

void got_bool(bool got)
{
    cout << "got_bool got ? " << got << endl;
}

int main(int argc, char ** argv)
{
    enum
    {
        TEST_EVENT = 1,
    };

    event_listener listener;

    sigslot::event_sink<uint32_t, bool> sink0(TEST_EVENT, std::bind(got_bool, std::placeholders::_1));
    sigslot::event_sink<uint32_t, bool> sink1(TEST_EVENT, std::bind(&event_listener::update, &listener, std::placeholders::_1));

    sigslot::event_publisher<uint32_t>::publish(TEST_EVENT, true);
}
