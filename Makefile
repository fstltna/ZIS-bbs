RELEASE=0.99.5# @RELEASE@

EXES=zis

CFLAGS = -gstabs -Wno-return-type
LDFLAGS =  -lmenu -lpanel -lcurses -lm

LN_S = ln -s

ifneq ($(DJGPP),)
EXES += set50

CFLAGS += -Idosfix
LDFLAGS += -Ldosfix

LN_S = cp -r
endif

ALL=$(EXES) TAGS

MAKENSIS=C:\Hack\NSIS\makensis.exe

all: $(ALL)

zis: game.o ai.o ui.o aa.o
	gcc $^ -o $@ $(LDFLAGS)

set50: set50.o

%.o: %.c zis.h
	gcc -c $*.c $(CFLAGS)

clean:
	-rm *.o
	-rm $(addsuffix .exe,$(ALL))
	-rm $(ALL)

distclean: clean
	-rm zis-*.tar.gz
	-rm install-zis.exe

TAGS: game.c ai.c ui.c aa.c zis.h set50.c Makefile
	etags $^

DIST=zis-$(RELEASE)

DISTFILES=game.c ai.c ui.c zis.h aa.c set50.c Makefile help.html install-zis.nsi	\
	release.py LICENSE README INSTALL TERMS

dist: $(DIST).tar.gz

$(DIST).tar.gz: $(DISTFILES)
	$(LN_S) . $(DIST)
	tar -czvf $(DIST).tar.gz --dereference $(addprefix $(DIST)/,$(DISTFILES))
	rm -r $(DIST)

WINDIST=install-$(DIST).exe

windist: $(WINDIST)

$(WINDIST): all
	strip $(addsuffix .exe,$(EXES))
	$(MAKENSIS) install-zis.nsi
	-rm dosfix-$(RELEASE).zip
	zip -9r dosfix-$(RELEASE).zip dosfix

tag:
	cvs tag release-$(subst .,-,$(RELEASE))
