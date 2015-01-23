check_PROGRAMS = test_runner

include ../../Makefile.inc

test_runner_SOURCES = test_runner.cpp basic.cpp generic_object_test.h \
			mnslp_msg.cpp session_lifetime.cpp \
			information_code.cpp mnslp_configure.cpp \
			generic_object_test.cpp message_hop_count.cpp \
		    msg_sequence_number.cpp selection_metering_entities.cpp \
		    session_lifetime.cpp utils.h mnslp_notify.cpp mnslp_refresh.cpp \
		    mnslp_options.cpp mnslp_response.cpp utils.cpp session_id.cpp \
		    session_manager.cpp ni_session.cpp nr_session.cpp nf_session.cpp \
		    mspec_rule_key.cpp policy_field_mapping.cpp


if USE_WITH_SCTP
LD_SCTP_LIB= -lsctp
endif


test_runner_CPPFLAGS = -I../src -I../src/msg -I$(MNSLP_INC) $(CPPUNIT_CFLAGS)  -I$(NTLP_INC) -I$(PROTLIB_INC) -I$(FQUEUE_INC) 
test_runner_CPPFLAGS += @LIBXML_CFLAGS@ @CURL_CFLAGS@ @LIBXSLT_CFLAGS@ @LIBUUID_CFLAGS@

test_runner_LDADD = -L../src $(LD_MNSLP_LIB) $(LD_NTLP_LIB) $(LD_PROTLIB_LIB) $(LD_FQUEUE_LIB) $(CPPUNIT_LIBS) $(LD_SCTP_LIB) -lipq -lrt -lssl -lpthread -lcrypto
test_runner_LDADD += @LIBXML_LIBS@ @CURL_LIBS@ @LIBXSLT_LIBS@ @LIBUUID_LIBS@

TESTS = $(check_PROGRAMS)

AM_CXXFLAGS = -Wall -ggdb -pedantic -Wno-long-long

if NSIS_NO_WARN_HASHMAP
AM_CXXFLAGS += -Wno-deprecated
endif

#  TEST_CXX_FLAGS=`cppunit-config --cflags`
