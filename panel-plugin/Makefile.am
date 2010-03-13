# $Id$

INCLUDES =								\
	-I$(top_srcdir)							\
	-DG_LOG_DOMAIN=\"xfce4-calculator-plugin\"			\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"				\
	$(PLATFORM_CPPFLAGS)

BACKEND_SRC = 								\
	eval.c								\
	eval.h								\
	lexer.c								\
	lexer.h								\
	parser.c							\
	parser.h							\
	parsetree.c							\
	parsetree.h							\
	constants.h

plugin_PROGRAMS =							\
	xfce4-calculator-plugin

plugindir =								\
	$(libexecdir)/xfce4/panel-plugins

check_PROGRAMS = calctest

xfce4_calculator_plugin_SOURCES =					\
	calculator.c							\
	$(BACKEND_SRC)

calctest_SOURCES =							\
	calctest.c							\
	$(BACKEND_SRC)

xfce4_calculator_plugin_CFLAGS =					\
	$(LIBXFCE4UTIL_CFLAGS)						\
	$(LIBXFCEGUI4_CFLAGS)						\
	$(LIBXFCE4PANEL_CFLAGS)						\
	$(PLATFORM_CFLAGS)

xfce4_calculator_plugin_LDADD =						\
	$(LIBXFCE4UTIL_LIBS)						\
	$(LIBXFCEGUI4_LIBS)						\
	$(LIBXFCE4PANEL_LIBS)

calctest_CFLAGS = $(xfce4_calculator_plugin_CFLAGS)
calctest_LDADD = $(xfce4_calculator_plugin_LDADD)

desktopdir =								\
	$(datadir)/xfce4/panel-plugins

desktop_in_in_files =							\
	calculator.desktop.in.in

desktop_in_files =							\
	$(desktop_in_in_files:.desktop.in.in=.desktop.in)

%.desktop.in: %.desktop.in.in
	sed -e "s,\@libexecdir\@,$(libexecdir),g" < $< > $@

desktop_DATA =								\
	$(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST =								\
	$(desktop_in_in_files)						\
	$(TESTS)							\
	grammar.txt

CLEANFILES =								\
	$(desktop_in_files)						\
	$(desktop_DATA)

TESTS = 								\
	test-simple-expr.awk						\
	test-minus.awk							\
	test-pow.awk

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
