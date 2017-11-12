CFLAGS += -Wall -std=c11

ifeq ($(DEBUG),1)
  CFLAGS += -g -DDEBUG
endif

PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
SHAREDIR=$(PREFIX)/share

.PHONY: all
all: pixd pixd.1.gz

.PHONY: clean
clean:
	rm -f pixd pixd.1.gz

.PHONY: install
install:
	install -D pixd $(DESTDIR)$(BINDIR)/pixd
	install -D pixd.1.gz $(DESTDIR)$(SHAREDIR)/man/man1/pixd.1.gz

pixd: pixd.c
	$(CC) $(CFLAGS) -o $@ $^

pixd.1.gz: pixd.1
	gzip -k pixd.1
