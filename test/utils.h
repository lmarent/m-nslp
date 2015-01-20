/*
 * Utilities to make testing easier.
 *
 * $Id: utils.h 4118 2015-01-05 13:55:00Z amarentes $
 * $HeadURL: https://./test/utils.h $
 */
#ifndef TESTSUITE_UTILS_H
#define TESTSUITE_UTILS_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "protlib_types.h"
#include "dispatcher.h"
#include "gist_conf.h"

class mock_mnslp_config;
class mock_dispatcher;
class mock_policy_rule_installer;


#define ASSERT_STATE(session, state) \
  CPPUNIT_ASSERT_MESSAGE("state mismatch", session.get_state() == state)

#define ASSERT_NO_MESSAGE(d) \
  CPPUNIT_ASSERT_MESSAGE("message has been sent", d->get_message() == NULL)

#define ASSERT_NO_TIMER(d) \
  CPPUNIT_ASSERT_MESSAGE("timer has been started", d->get_timer() == 0)

#define ASSERT_CONFIGURE_MESSAGE_SENT(d) \
  CPPUNIT_ASSERT_MESSAGE("no create message sent", \
    ( d->get_message() != NULL && dynamic_cast<mnslp::msg::mnslp_configure *>( \
	d->get_message()->get_mnslp_msg()) != NULL ) )

#define ASSERT_OPTIONS_MESSAGE_SENT(d) \
  CPPUNIT_ASSERT_MESSAGE("no options message sent", \
    ( d->get_message() != NULL && dynamic_cast<mnslp::msg::mnslp_options *>( \
	d->get_message()->get_mnslp_msg()) != NULL ) )

#define ASSERT_NOTIFY_MESSAGE_SENT(d) \
  CPPUNIT_ASSERT_MESSAGE("no notify message sent", \
    ( d->get_message() != NULL && dynamic_cast<mnslp::msg::mnslp_notify *>( \
	d->get_message()->get_mnslp_msg()) != NULL ) )

#define ASSERT_REFRESH_MESSAGE_SENT(d) \
  CPPUNIT_ASSERT_MESSAGE("no refresh message sent", \
    ( d->get_message() != NULL && dynamic_cast<mnslp::msg::mnslp_refresh *>( \
	d->get_message()->get_mnslp_msg()) != NULL ) )

#define ASSERT_TIMER_STARTED(d, timer) \
  CPPUNIT_ASSERT_MESSAGE("no timer started", d->get_timer() == timer.get_id())


void checkResponse(mock_dispatcher *d, protlib::uint8 severity,
	CppUnit::SourceLine line);

#define ASSERT_RESPONSE_MESSAGE_SENT(d, severity) \
  checkResponse(d, severity, CPPUNIT_SOURCELINE())


using namespace mnslp;

/**
 * A fake mnslp_config class for testing.
 */
class mock_mnslp_config : public mnslp_config {
  public:
	mock_mnslp_config() { 
		
		repository_init(); 
		setRepository();	
		// register all GIST configuration parameters at the registry
		ntlp::gconf.setRepository();
		
	}
};


/**
 * A fake dispatcher class for testing.
 */
class mock_dispatcher : public dispatcher {
  public:
	mock_dispatcher(session_manager *m=NULL, 
					policy_rule_installer *p=NULL, 
					mnslp_config *conf=NULL)
		: dispatcher(m, p, conf ? conf : new mock_mnslp_config()),
		  message(NULL), timer(0), next_timer_id(1) { }
	~mock_dispatcher() { clear(); }

	/*
	 * Inherited from parent:
	 */
	virtual void send_message(msg::ntlp_msg *msg) throw ();
	virtual id_t start_timer(const session *s, int secs) throw ();

	/*
	 * New methods:
	 */
	void clear() throw ();
	msg::ntlp_msg *get_message() throw ();
	id_t get_timer() throw ();

  private:
	msg::ntlp_msg *message;

	id_t timer;
	id_t next_timer_id;
};


#endif // TESTSUITE_UTILS_H
