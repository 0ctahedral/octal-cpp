.PHONY: clean

all:
	$(MAKE) -C ./engine
	$(MAKE) -C ./testbed

clean:
	$(MAKE) -C ./testbed clean
	$(MAKE) -C ./engine clean

run: all
	./bin/testbed
