# The only thing you probably need to change is this line.
# The binary will be installed in $(PREFIX)/bin
# The application data files will be installed in $(PREFIX)/share/schematic_gui
PREFIX=$(HOME)

# You might want to add -O2 to this line. (Although it is typically
# fast enough without it and you probably want to be able to debug
# this application for the moment...)
CFLAGS=-Wall -std=c++11 -g3  $$(pkg-config --cflags gtk+-3.0) -DPREFIX='"$(PREFIX)"'


COMPONENTS=add.v bigbox.v clockedreg.v mult.v box.v mux2.v mux3.v mux4.v mux5.v mux6.v
all: schematic_gui

# FIXME: Should probably use install rather than cp
install: schematic_gui
	mkdir -p $(PREFIX)/bin
	mkdir -p $(PREFIX)/share/schematic_gui/components
	cp schematic_gui       $(PREFIX)/bin
	cp $(COMPONENTS)       $(PREFIX)/share/schematic_gui/components
	cp schematic_gui.help  $(PREFIX)/share/schematic_gui/
	cp schematic_gui.glade $(PREFIX)/share/schematic_gui/

uninstall:
	echo 'Not implemented (but it should be easy to uninstall by hand'
	false

#FIXME: Autodetect dependencies
main.o: design.hh designgui.hh main.cc 
	g++ -c $(CFLAGS)  main.cc

designgui.o: designgui.cc verilog-subset.h verilog-subset.tab.h gate.hh designgui.hh verilog-subset.yy.h design.hh
	g++ -c $(CFLAGS) designgui.cc

design.o: design.cc verilog-subset.h verilog-subset.tab.h gate.hh verilog-subset.yy.h design.hh
	g++ -c $(CFLAGS) design.cc

gate.o: gate.cc gate.hh verilog-subset.h
	g++ -c $(CFLAGS) gate.cc

schematic_gui: design.o gate.o verilog-subset.tab.o verilog-subset.yy.o main.o designgui.o
	g++ -g3 main.o design.o gate.o -lavoid -lm  $$(pkg-config --libs gtk+-3.0)  verilog-subset.tab.o verilog-subset.yy.o designgui.o -o schematic_gui


%.tab.h %.tab.c: %.y
	bison --debug --verbose -d verilog-subset.y

verilog-subset.tab.o: verilog-subset.tab.c verilog-subset.h
	g++ $(CFLAGS) -c  verilog-subset.tab.c

verilog-subset.yy.o: verilog-subset.yy.c verilog-subset.tab.h verilog-subset.h
	g++ $(CFLAGS) -c  verilog-subset.yy.c

%.yy.c %.yy.h: %.l
	flex -o $*.yy.c $*.l


parsetest:
	flex verilog-subset.l
	bison --debug --verbose -d verilog-subset.y
	g++ -g verilog-subset.tab.c verilog-subset.yy.c subsettest.c	 -o subsettest


clean:
	rm -f verilog-subset.tab.* verilog-subset.yy.* verilog-subset.output *~  logfile.txt core a.out schematic_gui *.o subsettest


