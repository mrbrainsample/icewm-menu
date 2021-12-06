LIBS= `pkg-config --libs glib-2.0 gtk+-3.0 libmenu-cache`
CFLAGS+= -g -Wall `pkg-config --cflags glib-2.0 gtk+-3.0 libmenu-cache`
CC=gcc
#-DG_DISABLE_DEPRECATED

# Comment this line if you don't want icons to appear in menu
CFLAGS+=-DWITH_ICONS
# Uncomment this line if IceWM can display SVG icons
# Check SVG support with '$ ldd /usr/bin/icewm | grep svg', librsvg must appear.
# CFLAGS+=-DWITH_SVG

prefix= /usr/local
DESTDIR ?= $(prefix)
BINDIR= ${DESTDIR}/bin

SRC= $(shell ls src/*.c 2> /dev/null)
OBJ= $(SRC:.c=.o)

all: $(OBJ) check icewm-menu

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


icewm-menu: $(OBJ)
	$(CC) $(OBJ) -o icewm-menu $(LDFLAGS) $(LIBS)

.PHONY: clean install doc changelog check xmllint

clean:
	@rm -f $(OBJ) $(TEST_OBJ) icewm-menu check
	@rm -rf doc

install:
	@strip -s icewm-menu
	@install -Dm 755 icewm-menu $(BINDIR)/icewm-menu

doc:
	robodoc --src . --doc doc/ --multidoc --index --html --cmode

xmllint: openbox-menu
	./openbox-menu > test.xml
	xmllint test.xml
	rm test.xml

changelog:
	@hg log --style changelog > ChangeLog
