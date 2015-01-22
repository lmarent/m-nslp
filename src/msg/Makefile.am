# -----------------------------------*- mode: Makefile; -*--
# Makefile.am - Makefile.am for Metering NSLP message lib
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
include ../../../Makefile.inc

LDADD = $(PROTLIB_LIB)

MNSLPMSG_LLIB   = mnslp_msg

lib_LIBRARIES 	= libmnslp_msg.a

API_INC		= $(top_srcdir)/include
INC_DIR 	= ../../include/msg

libmnslp_msg_a_CPPFLAGS = -I. -I$(API_INC) -I$(MNSLP_INC) -I$(NTLP_INC) -I$(PROTLIB_INC) -I$(FQUEUE_INC)


libmnslp_msg_a_SOURCES = ie_object_key.cpp \
						 ie_store.cpp \
						 mnslp_object.cpp \
					     msg_sequence_number.cpp \
					     selection_metering_entities.cpp \
						 information_code.cpp \
						 message_hop_count.cpp \
					     mnslp_msg.cpp	\
						 mnslp_configure.cpp \
						 mnslp_notify.cpp \
						 mnslp_options.cpp \
						 mnslp_refresh.cpp \
						 ntlp_msg.cpp \
						 session_lifetime.cpp \
					     mnslp_ie.cpp \
					     mnslp_response.cpp \
					     ipfix_t.cpp \
					     mnslp_mspec_object.cpp \
					     mnslp_ipfix_message.cpp \
					     mnslp_field.cpp \
					     mnslp_field_key.cpp \
					     mnslp_ipfix_field.cpp \
					     mnslp_ipfix_data_record.cpp \
					     mnslp_ipfix_template.cpp 



include_HEADERS = 	$(INC_DIR)/ie_store.h $(INC_DIR)/mnslp_object.h			\
					$(INC_DIR)/msg_sequence_number.h $(INC_DIR)/mnslp_ie.h $(INC_DIR)/mnslp_object.h		\
					$(INC_DIR)/mnslp_msg.h $(INC_DIR)/mnslp_configure.h $(INC_DIR)/mnslp_notify.h \
					$(INC_DIR)/mnslp_options.h	$(INC_DIR)/mnslp_refresh.h $(INC_DIR)/session_lifetime.h \
					$(INC_DIR)/information_code.h $(INC_DIR)/mnslp_response.h $(INC_DIR)/message_hop_count.h\
					$(INC_DIR)/selection_metering_entities.h $(INC_DIR)/ntlp_msg.h $(INC_DIR)/mnslp_ipfix_message.h \
					$(INC_DIR)/mnslp_ipfix_data_record.h $(INC_DIR)/mnslp_ipfix_exception.h \
					$(INC_DIR)/mnslp_ipfix_field.h $(INC_DIR)/mnslp_ipfix_template.h \
					$(INC_DIR)/ipfix_t.h $(INC_DIR)/mnslp_mspec_object.h


if NSIS_NO_WARN_HASHMAP
AM_CXXFLAGS = -Wno-deprecated
else !NSIS_NO_WARN_HASHMAP
AM_CXXFLAGS = 
endif


# end of Makefile.am
