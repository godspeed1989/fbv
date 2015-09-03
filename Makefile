#
# Makefile
#
# Makefile for fbv

include Make.conf

CC = g++ 
CFLAGS = -Wall -D_GNU_SOURCE

SOURCES	= main.c jpeg.c png.c bmp.c fb_display.c transforms.c
OBJECTS	= ${SOURCES:.c=.o}

OUT	= fbv

all: $(OUT)
	@echo Build DONE.

$(OUT): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(OUT) $(OBJECTS) $(LIBS)

clean:
	rm -f $(OBJECTS) *~ $$$$~* *.bak core config.log $(OUT)

distclean: clean
	@echo -e "error:\n\t@echo Please run ./configure first..." >Make.conf
	rm -f $(OUT) config.h

install: $(OUT)
	cp $(OUT) $(bindir)
	[ -d $(mandir)/man1 ] || mkdir -p $(mandir)/man1
	gzip -9c $(OUT).1 > $(mandir)/man1/$(OUT).1.gz

uninstall: $(bindir)/$(OUT)
	rm -f $(bindir)/$(OUT)
	rm -f $(mandir)/man1/$(OUT).1.gz

