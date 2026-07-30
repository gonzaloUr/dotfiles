include config.mk

.PHONY: all clean install uninstall

all: src/pahook/pahook src/udevhook/udevhook

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp src/pahook/pahook $(DESTDIR)$(PREFIX)/bin
	cp src/pahook/udevhook $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/pahook
	rm -f $(DESTDIR)$(PREFIX)/bin/udevhook

clean:
	rm -f src/pahook/pahook.o src/pahook/pahook
	rm -f src/udevhook/udevhook.o src/udevhook/udevhook
	rm -f src/y.tab.c src/y.tab.h src/lex.yy.c src/y.tab.o src/lex.yy.o

src/pahook/pahook: src/pahook/pahook.o src/y.tab.o src/lex.yy.o
	$(CC) $(LDFLAGS_PAHOOK) src/pahook/pahook.o src/y.tab.o src/lex.yy.o -o src/pahook/pahook

src/pahook/pahook.o: src/pahook/pahook.c src/y.tab.o src/lex.yy.o
	$(CC) $(CFLAGS_PAHOOK) -c src/pahook/pahook.c -o src/pahook/pahook.o

src/udevhook/udevhook: src/udevhook/udevhook.o src/y.tab.o src/lex.yy.o
	$(CC) $(LDFLAGS_UDEVHOOK) src/udevhook/udevhook.o src/y.tab.o src/lex.yy.o -o src/udevhook/udevhook

src/udevhook/udevhook.o: src/udevhook/udevhook.c src/udevhook/config.h src/y.tab.o src/lex.yy.o
	$(CC) $(CFLAGS_UDEVHOOK) -c src/udevhook/udevhook.c -o src/udevhook/udevhook.o

src/udevhook/config.h: src/udevhook/config.def.h
	cp src/udevhook/config.def.h src/udevhook/config.h

src/y.tab.o: src/y.tab.c src/y.tab.h
	$(CC) $(CFLAGS_PARSER) -c src/y.tab.c -o src/y.tab.o

src/lex.yy.o: src/lex.yy.c src/y.tab.h
	$(CC) $(CFLAGS_PARSER) -c src/lex.yy.c -o src/lex.yy.o

src/lex.yy.c src/lex.yy.h: src/lexer.l src/y.tab.h
	$(LEX) --header-file=src/lex.yy.h -o src/lex.yy.c src/lexer.l

src/y.tab.c src/y.tab.h: src/parser.y src/parser.h
	$(YACC) -o src/y.tab.c src/parser.y
