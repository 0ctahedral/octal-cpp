.PHONY: clean

all: engine testbed

testbed: engine
	$(MAKE) -C ./testbed

engine:
	$(MAKE) -C ./octal

run: all
	./bin/testbed

