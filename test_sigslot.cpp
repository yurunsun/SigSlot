#include <sigslot.h>
#include <iostream>
#include <string>
#include <unistd.h>

#ifdef _SIGSLOT_MULTI_THREADED

class Source {
public:
	/*
	 * Signals are public fields within a class.
	 * The template args are a multithreading policy, followed by
	 * zero or more payload arguments.
	 */
	mutable sigslot::signal<sigslot::thread::mt> signal_zero;
	mutable sigslot::signal<sigslot::thread::mt, bool> signal_bool;
	
	Source() : m_toggle(true){}
	Source(Source &) = delete;
	Source(Source &&) = delete;

	void kerpling() {
		m_toggle = !m_toggle;
		/*
		 * To emit a signal, you can call its "emit" member with the arguments
		 * you declared for it.
		 */
		signal_bool.emit(m_toggle);
	}
	
	void boioing() const {
		/*
		 * Or, you can emit a signal by just calling it.
		 * This is particularly nice for passing as a functor somewhere.
		 */
		signal_zero();
	}
private:
	bool m_toggle; //{true};
};

/*
 * A class acting as a sink needs to inherit from has_slots<>.
 * The template argument (optional) is a threading policy.
 * A sink "owns" the signal connections; when it goes out of scope
 * they'll be disconnected.
 */
class Sink : public sigslot::has_slots<> {
public:
	Sink() = default;
	
	/*
	 * Slots are just void functions.
	 * This one takes a bool, obviously.
	 */
	void slot_bool(bool b) {
		std::cout << "Signalled bool(" << (b ? "true" : "false" ) << ")" << std::endl;
	}
	
	/*
	 * And this one is just void.
	 */
	void slot_void() {
		std::cout << "Signalled void." << std::endl;
	}
};

int main() {
	Source source;
	
	/*
	 * You can call unconnected signals if you want.
	 */
	source.kerpling();
	source.boioing();

	{
		Sink sink;
		using namespace std::placeholders;

		/*
		 * If you connect a signal using a pointer-to-member, it assumes
		 * you mean to call the member normally.
		 */
		source.signal_zero.connect(&sink, &Sink::slot_void);
		/*
		 * You can also connect an arbitrary functor, like something snazzy
		 * with a lambda.
		 * Note the first argument remains a has_slots<> derivative acting
		 * as the connection's owner.
		 */
		source.signal_bool.connect(&sink, [&sink](bool b) {sink.slot_bool(b);});

		/*
		 * Now those slots will be called when the signals are emitted.
		 */
		std::cout << "Bool: ";
		source.kerpling();
		std::cout << "Void: ";
		source.boioing();

		/*
		 * We can disconnect a sink from a signal in the obvious way.
		 */
		source.signal_bool.disconnect(&sink);

		std::cout << "Bool: ";
		source.kerpling();
		source.boioing();

		{
			Source source2;

			/*
			 * Multiple signals can connect to the same slot.
			 */
			source2.signal_zero.connect(&sink, &Sink::slot_void);

			source2.kerpling();
			std::cout << "Void: ";
			source2.boioing();
			source.kerpling();
			std::cout << "Void: ";
			source.boioing();

			/*
			 * If a signal is destroyed, the connections will vanish cleanly.
			 */
		}

		{
			Sink sink2;

			/*
			 * The same signal can emit to multiple slots, too.
			 */
			source.signal_zero.connect(&sink2, &Sink::slot_void);

			source.kerpling();
			std::cout << "Voidx2: ";
			source.boioing();
		}

		/*
		 * When Sinks are destroyed, the signals are disconnected.
		 */
	}

	/*
	 * These are unconnected, but you can still emit them (as a noop)
	 */
	source.kerpling();
	source.boioing();

	return 0;
}

#else

using namespace sigslot;
using namespace std;

class tcp_client
{
public:
    signal_type<>::sig connected;
    signal_type<int, const string&>::sig error;
    signal_type<const string&>::sig data;

    void run()
    {
        while (1) {
            connected();
            sleep(1);
            error(404, "host cannot be resolved!");
            sleep(1);
            data("channel 100");
            sleep(3);
        }
    }
};

class vom_client : public has_slots<>
{
public:
    signal_type<>::rep vom_connected;
    signal_type<int, const string&>::rep vom_error;


    vom_client()
    {
        c_.connected.connect(this, &vom_client::on_connected);
        c_.error.connect(this, &vom_client::on_error);
        c_.data.connect(this, &vom_client::on_data);

        vom_connected.repeat(c_.connected);
        vom_error.repeat(c_.error);
    }
    void run()
    {
        c_.run();
    }
    void on_connected()
    {
        cout << "vom_client on_connected" << endl;
    }
    void on_error(int ec, const string& em)
    {
        cout << "vom_client on_error ec:" << ec << " em:" << em << endl;
    }
    void on_data(const string& pkg)
    {
        cout << "vom_client on_data pkg:" << pkg << endl;
    }

private:
    tcp_client c_;
};

class data_center : public has_slots<>
{
public:
    void on_vom_connected()
    {
        cout << "data_center::on_vom_connected" << endl;
    }
    void on_vom_error(int ec, const string& em)
    {
        cout << "data_center::on_vom_error ec:" << ec << " em:" << em << endl;
    }
};

int main(int argc, char* argv[])
{
    vom_client vc;
    data_center db;

    vc.vom_connected.connect(&db, &data_center::on_vom_connected);
    vc.vom_error.connect(&db, &data_center::on_vom_error);

    vc.run();

    return 0;
}


#endif
