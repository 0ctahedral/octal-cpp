.PHONY: clean

all: engine testbed

testbed: engine
	$(MAKE) -C ./testbed all

engine:
	$(MAKE) -C ./engine all

clean:
	$(MAKE) -C ./testbed clean
	$(MAKE) -C ./engine clean

run: all
	./bin/testbed
