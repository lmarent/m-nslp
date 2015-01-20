/*
 * Test suite utilities (custom assertions, etc.)
 *
 * $Id: utils.cpp 4118 2015-01-05 13:57:00Z amarentes $
 * $HeadURL: https://./test/utils.cpp $
 */
#include <cppunit/TestAssert.h>
#include <cppunit/SourceLine.h>

#include "utils.h"
#include "msg/mnslp_msg.h"


using namespace CppUnit;
using namespace protlib;
using namespace mnslp;
using namespace mnslp::msg;

namespace ntlp {
// configuration class
gistconf gconf;
}

/**
 * Test if a RESPONSE message with the given severity class has been sent.
 */
void checkResponse(mock_dispatcher *d, protlib::uint8 severity,
	SourceLine line) 
{

	mnslp_response *r = dynamic_cast<mnslp_response *>(
		d->get_message()->get_mnslp_msg());

	Asserter::failIf(r->get_severity_class() != severity,
		Message("severity class does not match"), line);
}

/*
 * mock_dispatcher
 */


/**
 * For testing: Store the message instead of sending it.
 */
void mock_dispatcher::send_message(msg::ntlp_msg *msg) throw () {
	clear();
	message = msg;
}


/**
 * For testing: Store the timer instead of sending it.
 */
id_t mock_dispatcher::start_timer(const session *s, int secs)
		throw () {

	timer = next_timer_id++;

	return timer;
}


void mock_dispatcher::clear() throw () {
	if ( message != NULL ) {
		delete message;
		message = NULL;
	}
	timer = 0;
}


msg::ntlp_msg *mock_dispatcher::get_message() throw () {
	return message;
}

id_t mock_dispatcher::get_timer() throw () {
	return timer;
}


// EOF
