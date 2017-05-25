CFLAGS += -std=c11 -g

BINARIES=pixd

.PHONY: all
all: $(BINARIES)

.PHONY: clean
clean:
	rm -f $(BINARIES)

pixd: pixd.c
	$(CC) $(CFLAGS) -o $@ $^
