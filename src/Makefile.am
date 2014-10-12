## Process this file with automake to produce Makefile.in
rotini_LDADD   = @ROTINI_LIBS@ $(top_builddir)/libengine/librotiniengine.la
rotini_SOURCES = main.c \
			   engine_loader.c
rotini_program = rotini

INCLUDES = @ROTINI_CFLAGS@ -I$(top_srcdir)/include \
		   -DLOCALEDIR="\"@datadir@/locale"\"\
		   -DENGINE_DIR=\"$(enginesdir)\"

bin_PROGRAMS = $(rotini_program)
