PREFIX = /usr/local
INSTALLDIR = $(DESTDIR)$(PREFIX)

name=agrab
flags=`pkg-config --cflags --libs cairo x11`

$(name): $(name).cpp
	$(CXX) $(name).cpp -o $(name) $(flags)

clean:
	rm -f $(name)

install: $(name)
	mkdir -p $(INSTALLDIR)/bin
	cp $(name) $(INSTALLDIR)/bin/$(name)

uninstall:
	rm -f $(INSTALLDIR)/bin/$(name)

.PHONY: clean install uninstall
