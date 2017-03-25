## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in

INCLUDES 	= @IMLIB2_CFLAGS@ \
-Wall -g \
-I/usr/local/include \
-I$(top_srcdir)/lib

bin_PROGRAMS = etu

etu_INCLUDES = etu.h

etu_SOURCES = \
etu.c \
$(etu_INCLUDES)

etu_LDADD = @IMLIB2_LIBS@ 
