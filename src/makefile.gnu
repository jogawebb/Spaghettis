
# Prelude.

SHELL = /bin/sh

# Make paths.

VPATH = ../obj

BIN_DIR = ../bin
TCL_DIR = ../tcl

HELP_DIR   = ../resources/help
EXTRAS_DIR = ../resources/extras

# Install paths.

# /usr/local/bin/pd
# /usr/local/bin/pdsend
# /usr/local/bin/pdreceive
# /usr/local/include/m_pd.h
# /usr/local/lib/pd/bin/pdwatchdog
# /usr/local/lib/pd/tcl/*.tcl
# /usr/local/lib/pd/help/*.pdhelp

prefix          = /usr/local
exec_prefix     = $(prefix)
bindir          = $(exec_prefix)/bin
includedir      = $(prefix)/include
libdir          = $(exec_prefix)/lib
libpddir        = $(libdir)/pd
libpdbindir     = $(libpddir)/bin
libpdtcldir     = $(libpddir)/tcl
libpdhelpdir    = $(libpddir)/help
libpdextrasdir  = $(libpddir)/extras

# Warnings

WARNINGS = -Wall -Wextra -Wshadow -Wno-unused-parameter -Wno-unknown-pragmas

# Linker flags.

LIB = -ldl -lpthread -lm

LDFLAGS = -rdynamic $(ARCH)

# Preprocessor and compiler flags.

ifdef WITH_DEBUG

CPPFLAGS = -DPD_WITH_DEBUG -DPD_BUILDING_APPLICATION

CFLAGS = -ffast-math $(WARNINGS) $(ARCH)

else

CPPFLAGS = -DNDEBUG -DPD_BUILDING_APPLICATION

CFLAGS = -O3 -ffast-math -fvisibility=hidden $(WARNINGS) $(ARCH)

endif

# MIDI with ALSA.

MIDI_SRC = s_midi_alsa.c
LIB += -lasound

# Audio with JACK.

AUDIO_SRC = s_logger_jack.c s_audio_jack.c
LIB += -ljack

# Sources (application amalgamated).

SRC = amalgam.c

OBJ = $(SRC:.c=.o) $(MIDI_SRC:.c=.o) $(AUDIO_SRC:.c=.o)

# Targets.

.PHONY: all

all: $(BIN_DIR)/pd $(BIN_DIR)/pdwatchdog $(BIN_DIR)/pdsend $(BIN_DIR)/pdreceive

$(BIN_DIR):
	@test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)

$(OBJ): %.o : %.c
	@echo "Build $@ ..."
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/pd: $(OBJ) | $(BIN_DIR)
	@echo "Build pd ..."
	@$(CC) $(LDFLAGS) -o $(BIN_DIR)/pd $(OBJ) $(LIB)

$(BIN_DIR)/pdwatchdog: u_watchdog.c | $(BIN_DIR)
	@echo "Build pdwatchdog ..."
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $(BIN_DIR)/pdwatchdog u_watchdog.c

$(BIN_DIR)/pdsend: u_pdsend.c | $(BIN_DIR)
	@echo "Build pdsend ..."
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $(BIN_DIR)/pdsend u_pdsend.c

$(BIN_DIR)/pdreceive: u_pdreceive.c | $(BIN_DIR)
	@echo "Build pdreceive ..."
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $(BIN_DIR)/pdreceive u_pdreceive.c

depend: makefile.dependencies

makefile.dependencies:
	@echo "Build makefile.dependencies ..."
	@$(CC) $(CPPFLAGS) -M $(SRC) > makefile.dependencies

clean:
	@echo "Remove makefile.dependencies ..."
	@-rm -f makefile.dependencies
	@echo "Remove objects ..."
	@-rm -f $(OBJ)
	@-rm -f "s_audio_jack.o"
	@-rm -f "s_logger_jack.o"
	@echo "Remove binaries ..."
	@-rm -f $(BIN_DIR)/pd $(BIN_DIR)/pdsend $(BIN_DIR)/pdreceive $(BIN_DIR)/pdwatchdog
	@echo "Remove bin directory ..."
	@-rmdir $(BIN_DIR)

install: all
	@echo "Install binaries ..."
	install -d $(DESTDIR)$(bindir)
	install -m755 $(BIN_DIR)/pd $(DESTDIR)$(bindir)/pd
	install -m755 $(BIN_DIR)/pdsend $(DESTDIR)$(bindir)/pdsend
	install -m755 $(BIN_DIR)/pdreceive $(DESTDIR)$(bindir)/pdreceive
	@echo "Install pdwatchdog ..."
	install -d $(DESTDIR)$(libpdbindir)
	install $(BIN_DIR)/pdwatchdog $(DESTDIR)$(libpdbindir)/pdwatchdog
	@echo "Install scripts ..."
	install -d $(DESTDIR)$(libpdtcldir)
	install $(TCL_DIR)/*.tcl $(DESTDIR)$(libpdtcldir)
	@echo "Install help ..."
	install -d $(DESTDIR)$(libpdhelpdir)
	install $(HELP_DIR)/*.pdhelp $(DESTDIR)$(libpdhelpdir)
	install $(HELP_DIR)/*.txt $(DESTDIR)$(libpdhelpdir)
	install $(HELP_DIR)/*.aiff $(DESTDIR)$(libpdhelpdir)
	@echo "Install extras ..."
	install -d $(DESTDIR)$(libpdextrasdir)
	@echo "Install headers ..."
	install -d $(DESTDIR)$(includedir)
	install -m644 m_pd.h $(DESTDIR)$(includedir)/m_pd.h

uninstall:
	@echo "Uninstall binaries ..."
	rm -f $(DESTDIR)$(bindir)/pd
	rm -f $(DESTDIR)$(bindir)/pdsend
	rm -f $(DESTDIR)$(bindir)/pdreceive
	@echo "Uninstall pdwatchdog ..."
	@echo "Uninstall scripts ..."
	rm -f -r $(DESTDIR)$(libpddir)
	@echo "Uninstall help ..."
	rm -f -r $(DESTDIR)$(libpdhelpdir)
	@echo "Uninstall extras ..."
	rm -f -r $(DESTDIR)$(libpdextrasdir)
	@echo "Uninstall headers ..."
	rm -f $(DESTDIR)$(includedir)/m_pd.h

include makefile.dependencies
