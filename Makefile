include config.mk

.POSIX:

.PHONY: all clean install uninstall

all: musgit

clean:
	rm -f musgit.o musgit

install: musgit
	# musgit
	mkdir -p "$(DESTDIR)$(PREFIX)/bin"
	cp -f musgit "$(DESTDIR)$(PREFIX)/bin"
	chmod 644 "$(DESTDIR)$(PREFIX)/bin/musgit"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/musgit"

musgit: musgit.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

musgit.o: musgit.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

