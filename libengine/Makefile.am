AM_CPPFLAGS = @ROTINI_CFLAGS@ -I$(top_srcdir)/include\
              -DENGINE_DIR=\"$(enginesdir)\" \
              -DDEFTHEMEDIR='"@datadir@/rotini/theme/"' \
              -DDEFSETTINGSFILE='"@datadir@/rotini/settings.ini"'
librotiniengine_la_LIBADD = @ROTINI_LIBS@
librotiniengine_la_SOURCES = rotini.c themer.c
lib_LTLIBRARIES = librotiniengine.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = rotiniengine.pc
