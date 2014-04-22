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

    sigslot::event_sink<TEST_EVENT, bool> sink0(std::bind(got_bool, std::placeholders::_1));
    sigslot::event_sink<TEST_EVENT, bool> sink1(std::bind(&event_listener::update, &listener, std::placeholders::_1));

    sigslot::event_publisher<TEST_EVENT>::publish(true);
}
