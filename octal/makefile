src=$(shell find ./src \( -name \*.cpp \) -print)
obj=$(src:%.cpp=%.o)
obj_dir=build
obj_files=$(foreach f,$(obj), $(obj_dir)/$(notdir $f))
inc=-Isrc
targ=engine
cflags=-g -shared -fdeclspec -fPIC
ldflags=
defines="-D_DEBUG -DEXPORT"

.PHONY:
	clean

all: $(targ)

clean:
	rm -rf build/*.o
	rm -rf bin/*.so

$(targ): $(obj)
	clang++ -o ../bin/lib$(targ).so $(obj_files) $(cflags) $(defines) $(ldflags) 

%.o : %.cpp
	clang++ -c $< -o $(obj_dir)/$(notdir $@) $(inc) 
