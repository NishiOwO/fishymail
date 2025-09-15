# $Id$
# This is the default config

# Enable SASL, requires Cyrus version 2.x.
C_CFLAGS += -DUSE_CYRUS_SASL2
C_LDFLAGS +=
C_LIBS += -lsasl2

# Enable SSL, requires OpenSSL.
C_CFLAGS += -DUSE_SSL
C_LDFLAGS +=
C_LIBS += -lssl -lcrypto
