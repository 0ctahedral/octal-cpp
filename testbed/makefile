src=$(shell find ./src \( -name \*.cpp \) -print)
obj=$(src:%.cpp=%.o)
obj_dir=build
obj_files=$(foreach f,$(obj), $(obj_dir)/$(notdir $f))
inc=-I../octal/src/ -Isrc 
targ=testbed
cflags=-g -fdeclspec -fPIC
ldflags=-L../bin/ -lengine -Wl,-rpath,.
defines="-D_DEBUG -DEXPORT"

.PHONY:
	clean

all: $(targ)

clean:
	rm -rf build/*.o
	rm -rf ../bin/$(targ)

$(targ): $(obj)
	clang++ -o ../bin/$(targ) $(obj_files) $(cflags) $(defines) $(ldflags) 

%.o : %.cpp
	clang++ -c $< -o $(obj_dir)/$(notdir $@) $(inc) 
