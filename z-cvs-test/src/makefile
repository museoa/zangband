# File: Makefile


##
## This is my compiler of choice, it seems to work most everywhere
##
CC := gcc-3.0

COMMONOBJS := \
		artifact.o avatar.o bind.o birth-tnb.o birth.o bldg.o \
		canv-widget.o cave.o cmdinfo-dll.o icon-dll.o\
		cmd1.o cmd2.o cmd3.o cmd4.o cmd5.o cmd6.o \
		const.o describe.o dungeon.o effects.o \
		files.o flavor.o generate.o grid.o icon1.o icon2.o init1.o \
		init2.o interp1.o interp2.o load.o struct-dll.o\
		main-tnb.o map.o melee1.o tcltk.o map-dll.o\
		melee2.o mind.o monster1.o monster2.o mspells1.o mspells2.o \
		mutation.o notes.o obj_kind.o object1.o object2.o plat.o \
		qebind-dll.o r_info.o racial.o rooms.o save.o savefile-z.o \
		scores.o setting.o spells1.o \
		spells2.o spells3.o store.o streams.o struct.o tables.o \
		town.o util-tnb.o util.o variable.o widget-iso.o widget.o \
		widget1-dll.o widget2-dll.o \
		wild.o wizard1.o wizard2.o xtra1.o xtra2.o \
		util-dll.o plat-dll.o memory-dll.o\
		z-form.o z-rand.o z-term.o z-util.o z-virt.o

CFLAGS := -Wall -O2 -g -fno-strength-reduce



##
## Hack -- "install_game" dependancy as the target
## to get them to compile properly.
##
install: zangband
	cp zangband ../zangband

##
## Variation -- All the ports that work in Linux at once.
## (Use the -m option to start the one you want.)
##
##
## Graphics under X11 in linux.  (Run with the -g option)
## Note: Get the 16x16.bmp file, and put in [Z]directory/lib/xtra/graf
##       to get 256 colours.
##
CFLAGS += -D"USE_X11" -pedantic -W -Wmissing-prototypes -Wmissing-declarations \
				 -Wwrite-strings -Wpointer-arith \
				 -Wbad-function-cast -Wsign-compare\
				 -Waggregate-return -Wstrict-prototypes \
				 -I/usr/include/tcl8.3 \
				 -I/usr/include/tcl8.3/tk-private/unix \
				 -I/usr/include/tcl8.3/tk-private/generic \
				 -I/usr/include/tcl8.3/tcl-private/generic \
				 -Wredundant-decls -D"PLATFORM_X11"
 
LIBS := -lX11 -lcurses -lncurses -L/usr/X11R6/lib\
               -lXaw -lXext -lSM -lICE -lXmu -lXt -ltcl8.3 -ltk8.3 -lz


##
## Main list of objects and source files.
##

OBJS = $(COMMONOBJS)

HDRS := h-basic.h \
	h-define.h h-type.h h-system.h h-config.h

INCS := angband.h \
	z-config.h defines.h types.h externs.h \
	z-term.h z-rand.h z-util.h z-virt.h z-form.h $(HDRS)

$(OBJS): $(INCS)
$(BOOTOBJS):

zangband: $(OBJS)
	$(CC) $(CFLAGS) -o zangband $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o


