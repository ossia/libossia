NAME = OSSIA-API
SUFFIX = dylib

CC_32 = /usr/bin/clang++ -arch i386
CC_64 = /usr/bin/clang++ -arch x86_64

#########################################

SRC32 = Scenario.i386.o
SRC64 = Scenario.x64.o

#########################################

INCLUDES = -I../
INCLUDES += -IScore/library/includes/
INCLUDE_FILES := $(wildcard INCLUDES/*.h)

#########################################

LIBS = Score/library/build/JamomaScore.dylib

LIBS += /System/Library/Frameworks/Carbon.framework/Versions/A/Carbon


#########################################

OPTIMIZATION_DEBUG = -O0 -DTT_ENABLE_ASSERTIONS
OPTIMIZATION_RELEASE = -O3

OPTIONS = -shared -msse3 -mfpmath=sse -gdwarf-2 -fvisibility=hidden
OPTIONS += -std=c++11 
OPTIONS += -stdlib=libc++ # -U__STRICT_ANSI__ -D__STDC_FORMAT_MACROS
WARNINGS = -Wall -Wno-unknown-pragmas -Wno-trigraphs
DEFINES = -DTT_PLATFORM_MAC
#########################################

#DEFINES += -DOSSIA_API_EXPORTS



#########################################

CFLAGS = $(OPTIONS) $(DEFINES) $(INCLUDES) $(WARNINGS)
LDFLAGS =  -shared -mfpmath=sse $(OPTIONS) $(DEFINES) $(LIBS) $(WARNINGS)
LDFLAGS += -install_name "@loader_path/../../../../support/$(NAME).dylib" 

#########################################

Debug: OPTIMIZATION_FLAGS = $(OPTIMIZATION_DEBUG)
Debug: createdirs install

DebugWithoutTests: OPTIMIZATION_FLAGS = $(OPTIMIZATION_DEBUG)
DebugWithoutTests: createdirs notest

Release: OPTIMIZATION_FLAGS = $(OPTIMIZATION_RELEASE)
Release: createdirs install

createdirs:
	mkdir -p build

%.i386.o: %.cpp ${INCLUDE_FILES}
	$(CC_32) $(CFLAGS) $(OPTIMIZATION_FLAGS) -c $< -o $@
%.i386.mm.o: %.mm ${INCLUDE_FILES}
	$(CC_32) $(CFLAGS) $(OPTIMIZATION_FLAGS) -c $< -o $@
%.x64.o: %.cpp ${INCLUDE_FILES}
	$(CC_64) $(CFLAGS) $(OPTIMIZATION_FLAGS) -c $< -o $@
%.x64.mm.o: %.mm ${INCLUDE_FILES}
	$(CC_64) $(CFLAGS) $(OPTIMIZATION_FLAGS) -c $< -o $@

link: i386 x64 | $(SRC32) $(SRC64)

i386: $(SRC32)
	$(CC_32) $(LDFLAGS) $(OPTIMIZATION_FLAGS) -o build/$(NAME)-i386.dylib $(SRC32)
	libtool -static -o build/lib$(NAME)-i386.a $(SRC32)

x64: $(SRC64)
	$(CC_64) $(LDFLAGS) $(OPTIMIZATION_FLAGS) -o build/$(NAME)-x86_64.dylib $(SRC64)
	libtool -static -o build/lib$(NAME)-x86_64.a $(SRC64)

lipo: | link
	lipo build/$(NAME)-i386.dylib build/$(NAME)-x86_64.dylib -create -output build/$(NAME).dylib
	lipo build/lib$(NAME)-i386.a build/lib$(NAME)-x86_64.a -create -output build/lib$(NAME).a
	install_name_tool -change @loader_path/../../../../support/JamomaFoundation.dylib @loader_path/JamomaFoundation.dylib build/$(NAME).$(SUFFIX)
	install_name_tool -change @loader_path/../../../../support/JamomaDSP.dylib @loader_path/JamomaDSP.dylib build/$(NAME).$(SUFFIX)

clean:
	rm -f $(SRC32) $(SRC64)
	rm -rf build

build_and_test: | lipo 
	echo Testing 32-bit 
	if [ -f test.cpp ];   then rm -f build/test32; $(CC_32) test.cpp -g -std=c++11 -stdlib=libc++ -DTT_PLATFORM_MAC -DTT_ENABLE_ASSERTIONS ${INCLUDES} build/lib$(NAME).a ../../Foundation/library/build/libJamomaFoundation.a ../../DSP/library/build/libJamomaDSP.a /usr/lib/libxml2.dylib  -o build/test32 ; fi 
	if [ -f build/test32 ]; then build/test32 ; fi 
	echo Testing 64-bit 
	if [ -f test.cpp ];   then rm -f build/test64; $(CC_64) test.cpp -g -std=c++11 -stdlib=libc++ -DTT_PLATFORM_MAC -DTT_ENABLE_ASSERTIONS ${INCLUDES} build/lib$(NAME).a ../../Foundation/library/build/libJamomaFoundation.a ../../DSP/library/build/libJamomaDSP.a /usr/lib/libxml2.dylib  -o build/test64 ; fi 
	if [ -f build/test64 ]; then build/test64 ; fi 

notest: | lipo 
	if [ -f test.cpp ];   then rm -f build/test32; $(CC_32) test.cpp -g -std=c++11 -stdlib=libc++ -DTT_PLATFORM_MAC -DTT_ENABLE_ASSERTIONS ${INCLUDES} build/lib$(NAME).a ../../Foundation/library/build/libJamomaFoundation.a ../../DSP/library/build/libJamomaDSP.a /usr/lib/libxml2.dylib  -o build/test32 ; fi 
	if [ -f test.cpp ];   then rm -f build/test64; $(CC_64) test.cpp -g -std=c++11 -stdlib=libc++ -DTT_PLATFORM_MAC -DTT_ENABLE_ASSERTIONS ${INCLUDES} build/lib$(NAME).a ../../Foundation/library/build/libJamomaFoundation.a ../../DSP/library/build/libJamomaDSP.a /usr/lib/libxml2.dylib  -o build/test64 ; fi 
	echo Skipping Tests 

install: | build_and_test
	../../Shared/jamoma_copy.sh build/$(NAME).dylib ../../../Implementations/Max/Jamoma/support

