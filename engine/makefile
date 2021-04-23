src=$(shell find ./src/octal \( -name \*.cpp \) -print)
obj=$(src:%.cpp=%.o)
bin_dir=../bin
obj_dir=obj
obj_files=$(foreach f,$(obj), $(obj_dir)/$(notdir $f))
inc=-I./src/
targ=octal
cflags=-g -shared -fdeclspec -fPIC
ldflags=
defines="-D_DEBUG -DEXPORT"

# defines for platform
# if linux
platform=linux

.PHONY:
	clean

all: engine

clean:
	rm -rf $(obj_dir)/*.o
	rm -rf $(bin_dir)/*.so

engine: $(obj) $(platform)
	clang++ -o $(bin_dir)/lib$(targ).so $(obj_dir)/*.o $(cflags) $(defines) $(ldflags) 

linux:
	clang++ -c src/platform/linux/linux.cpp -o $(obj_dir)/$@.o $(inc)

%.o : %.cpp
	clang++ -c $< -o $(obj_dir)/$(notdir $@) $(inc) 
