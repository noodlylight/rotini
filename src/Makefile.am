rotini_LDADD   = @ROTINI_LIBS@ $(top_builddir)/libengine/librotiniengine.la
rotini_SOURCES = main.c engine_loader.c
rotini_program = rotini

AM_CPPFLAGS = @ROTINI_CFLAGS@ -I$(top_srcdir)/include \
              -DLOCALEDIR="\"@datadir@/locale"\"\
              -DENGINE_DIR=\"$(enginesdir)\"

bin_PROGRAMS = $(rotini_program)
