## Process this file with automake to produce Makefile.in
rotini_theme_manager_SOURCES = main.c
rotini_theme_manager_LDADD = $(ROTINI_LIBS) ../libengine/librotiniengine.la

bin_PROGRAMS = rotini-theme-manager

INCLUDES = @ROTINI_CFLAGS@ \
		   -DPIXMAPS_DIR=\""$(datadir)/pixmaps"\"\
		   -DDATA_DIR=\""$(datadir)"\"\
		   -DLOCALEDIR="\"@datadir@/locale"\"\
		   -I$(top_srcdir)/include
