INCLUDES = $(all_includes) $(PYTHONINC) $(SOUNDSERVER_INC) $(ESD_INC) $(OPENSSL_INC)
METASOURCES = AUTO
noinst_LIBRARIES =  libssh.a
#libssh_la_LDFLAGS = $(OPENSSL_LIB) -static
libssh_a_SOURCES =  crc32.cpp crc32.h types.h getput.h Makefile.am qtermsshauth.cpp qtermsshauth.h qtermsshbuffer.cpp qtermsshbuffer.h qtermsshcipher.h qtermsshconst.h qtermsshdes.cpp qtermsshdes.h qtermsshhash.h qtermsshkex.cpp qtermsshkex.h qtermsshlogin.cpp qtermsshlogin.h qtermsshloginui.cpp qtermsshloginui.h qtermsshmd5.cpp qtermsshmd5.h qtermsshpacket.cpp qtermsshpacket.h qtermsshrsa.cpp qtermsshrsa.h qtermsshsession.cpp qtermsshsession.h qtermsshsocket.cpp qtermsshsocket.h
#libssh_a_LIBADD = $(LIB_OPENSSL)
