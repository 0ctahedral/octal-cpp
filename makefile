.PHONY: clean

all: engine testbed

testbed: ./bin/testbed ./bin/
	$(MAKE) -C ./testbed

engine:
	$(MAKE) -C ./engine

clean:
	$(MAKE) -C ./testbed clean
	$(MAKE) -C ./engine clean

run: all
	./bin/testbed
