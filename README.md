#sigslot - C++11 Signal/Slot library

##1. How to compile

	qmake *.pro CONFIG+=SIGSLOT
	qmake *.pro CONFIG+=EVENT

##2. History

+ Originally written by [Sarah Thompson](http://sigslot.sourceforge.net/).
+ C++11-erization by Dave Cridland:
+ This is public domain; no copyright is claimed or asserted. No warranty is implied or offered either.
+ Add repeater. Use std::lock instead of platform specific locks. By yurunsun@gmail.com
+ Signal producer and consumer do not need to know each other. See `test_event.cpp`. By everwanna@gmail.com

