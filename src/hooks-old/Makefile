include config.mk

.PHONY: all clean install uninstall

all: pahook/pahook udevhook/udevhook

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp pahook/pahook $(DESTDIR)$(PREFIX)/bin
	cp pahook/udevhook $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/pahook
	rm -f $(DESTDIR)$(PREFIX)/bin/udevhook

clean:
	rm -f pahook/pahook.o pahook/pahook
	rm -f udevhook/udevhook.o udevhook/udevhook
	rm -f y.tab.c y.tab.h lex.yy.c lex.yy.h y.tab.o lex.yy.o

pahook/pahook: pahook/pahook.o y.tab.o lex.yy.o
	$(CC) $(LDFLAGS_PAHOOK) pahook/pahook.o y.tab.o lex.yy.o -o pahook/pahook

pahook/pahook.o: pahook/pahook.c pahook/config.h y.tab.o lex.yy.o
	$(CC) $(CFLAGS_PAHOOK) -c pahook/pahook.c -o pahook/pahook.o

pahook/config.h: pahook/config.def.h
	cp pahook/config.def.h pahook/config.h

udevhook/udevhook: udevhook/udevhook.o y.tab.o lex.yy.o
	$(CC) $(LDFLAGS_UDEVHOOK) udevhook/udevhook.o y.tab.o lex.yy.o -o udevhook/udevhook

udevhook/udevhook.o: udevhook/udevhook.c udevhook/config.h y.tab.o lex.yy.o
	$(CC) $(CFLAGS_UDEVHOOK) -c udevhook/udevhook.c -o udevhook/udevhook.o

udevhook/config.h: udevhook/config.def.h
	cp udevhook/config.def.h udevhook/config.h

y.tab.o: y.tab.c y.tab.h
	$(CC) $(CFLAGS_PARSER) -c y.tab.c -o y.tab.o

lex.yy.o: lex.yy.c y.tab.h
	$(CC) $(CFLAGS_PARSER) -c lex.yy.c -o lex.yy.o

lex.yy.c lex.yy.h: lexer.l y.tab.h
	$(LEX) --header-file=lex.yy.h -o lex.yy.c lexer.l

y.tab.c y.tab.h: parser.y parser.h
	$(YACC) -o y.tab.c parser.y
