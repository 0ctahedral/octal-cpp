src=$(shell find ./src \( -name \*.cpp \) -print)
obj=$(src:%.cpp=%.o)
obj_dir=obj
obj_files=$(foreach f,$(obj), $(obj_dir)/$(notdir $f))
bin_dir=../bin
lib_dir=../lib
inc=-I../engine/src/ -Isrc 
targ=testbed
cflags=-g
ldflags=-L$(lib_dir) -loctal -Wl,-rpath,\$$ORIGIN/../lib
defines="-D_DEBUG -DEXPORT"

.PHONY:
	clean

all: $(targ)

clean:
	rm -rf build/*.o
	rm -rf $(bin_dir)/$(targ)

$(targ): $(obj)
	clang++ -o $(bin_dir)/$(targ) $(obj_files) $(cflags) $(defines) $(ldflags) 

%.o : %.cpp
	clang++ -c $< -o $(obj_dir)/$(notdir $@) $(inc) 
