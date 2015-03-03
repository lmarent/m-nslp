# -----------------------------------*- mode: Makefile; -*--
# Makefile.am - Makefile.am for MNSLP NSLP
# ----------------------------------------------------------
# $Id$
# $HeadURL$
# ==========================================================
#                      
# (C)opyright, all rights reserved by
# - System and Computing Engineering, Universidad de los Andes
# ==========================================================
#
#
include ../../Makefile.inc

bin_PROGRAMS = mnslpd test_client

#dist_natfw_scripts = 


LDADD = $(PROTLIB_LIB)

MNSLP_LLIB      = mnslp

MNSLPMSG_LLIB   = mnslp_msg

lib_LIBRARIES 	= libmnslp.a

API_INC		= $(top_srcdir)/include
INC_DIR 	= ../include/
MNSLPMSG_INCDIR	= $(INC_DIR)/msg
MNSLPMSG_LIBDIR	= msg


libmnslp_a_CPPFLAGS = -I$(API_INC) -I$(MNSLP_INC) -I$(MNSLPMSG_INCDIR) -I$(NTLP_INC) -I$(PROTLIB_INC) -I$(FQUEUE_INC)
libmnslp_a_CPPFLAGS += @LIBXML_CFLAGS@ @CURL_CFLAGS@ @LIBXSLT_CFLAGS@ @LIBUUID_CFLAGS@

mnslpd_CPPFLAGS = -I$(API_INC) -I$(MNSLP_INC) -I$(MNSLPMSG_INCDIR) -I$(NTLP_INC) -I$(PROTLIB_INC) -I$(FQUEUE_INC)
mnslpd_CPPFLAGS += @LIBXML_CFLAGS@ @CURL_CFLAGS@ @LIBXSLT_CFLAGS@ @LIBUUID_CFLAGS@

test_client_CPPFLAGS = -I$(API_INC) -I$(NATFW_INC) -I$(QSPEC_INC) -I$(NTLP_INC) -I$(PROTLIB_INC) -I$(FQUEUE_INC)
test_client_CPPFLAGS += @LIBXML_CFLAGS@ @CURL_CFLAGS@ @LIBXSLT_CFLAGS@ @LIBUUID_CFLAGS@

MNSLP_SOURCEFILES = benchmark_journal.cpp gistka_mapper.cpp session_id.cpp \
  dispatcher.cpp mnslp_daemon.cpp nf_session.cpp ni_session.cpp \
  nr_session.cpp session_manager.cpp session.cpp mnslp_config.cpp mnslp_timers.cpp \
  policy_rule_installer.cpp policy_action.cpp policy_action_mapping.cpp \
  policy_action_container.cpp nop_policy_rule_installer.cpp \
  netmate_ipfix_policy_rule_installer.cpp mspec_rule_key.cpp policy_rule.cpp \
  mnslp_xml_node_reader.cpp \
  policy_application_configuration_container.cpp \
  policy_field_mapping.cpp \
  netmate_ipfix_metering_config.cpp \
  netmate_ipfix_policy_action_mapping.cpp


libmnslp_a_SOURCES = $(MNSLP_SOURCEFILES)

mnslpd_SOURCES = mnslp_daemon.cpp main.cpp

test_client_SOURCES = test_client.cpp

if USE_WITH_SCTP
LD_SCTP_LIB= -lsctp
endif

mnslpd_LDADD = -L. -l$(MNSLP_LLIB) -L$(MNSLP_MSG_LIBDIR) -l$(MNSLPMSG_LLIB) $(LD_QSPEC_LIB) $(LD_NTLP_LIB) $(LD_PROTLIB_LIB) $(LD_FQUEUE_LIB) -lnetfilter_queue -lssl -lrt $(LD_SCTP_LIB) -lpthread -lxml2
mnslpd_LDADD += @LIBXML_LIBS@ @CURL_LIBS@ @LIBXSLT_LIBS@ @LIBUUID_LIBS@

test_client_LDADD = -L. -l$(MNSLP_LLIB) -L$(MNSLP_MSG_LIBDIR) -l$(MNSLPMSG_LLIB) $(LD_QSPEC_LIB) $(LD_NTLP_LIB) $(LD_PROTLIB_LIB) $(LD_FQUEUE_LIB) -lnetfilter_queue -lssl -lrt $(LD_SCTP_LIB) -lpthread -lxml2
test_client_LDADD += @LIBXML_LIBS@ @CURL_LIBS@ @LIBXSLT_LIBS@ @LIBUUID_LIBS@

include_HEADERS = $(INC_DIR)/session_id.h \
	        	$(MNSLPMSG_INCDIR)/msg_sequence_number.h $(MNSLPMSG_INCDIR)/mnslp_ie.h $(MNSLPMSG_INCDIR)/mnslp_object.h \
	        	$(MNSLPMSG_INCDIR)/ntlp_msg.h $(MNSLPMSG_INCDIR)/ie_store.h	$(MNSLPMSG_INCDIR)/message_hop_count.h \
	       	 	$(MNSLPMSG_INCDIR)/mnslp_configure.h $(MNSLPMSG_INCDIR)/mnslp_msg.h $(MNSLPMSG_INCDIR)/mnslp_response.h	\
	        	$(MNSLPMSG_INCDIR)/session_lifetime.h \
	        	$(MNSLPMSG_INCDIR)/information_code.h $(MNSLPMSG_INCDIR)/mnslp_refresh.h $(MNSLPMSG_INCDIR)/mnslp_notify.h 


if NSIS_NO_WARN_HASHMAP
AM_CXXFLAGS = -Wno-deprecated
else !NSIS_NO_WARN_HASHMAP
AM_CXXFLAGS = 
endif


# end of Makefile.am
