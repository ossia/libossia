NAME = OSSIA-API
SUFFIX = dylib

CC_32 = /usr/bin/clang++ -arch i386
CC_64 = /usr/bin/clang++ -arch x86_64

#########################################

SRC32 = Sources/Editor/Curve.i386.o
SRC64 = Sources/Editor/Curve.x64.o
SRC32 += Sources/Editor/CurveSegment.i386.o
SRC64 += Sources/Editor/CurveSegment.x64.o
SRC32 += Sources/Editor/CurveSegment/CurveSegmentLinear.i386.o
SRC64 += Sources/Editor/CurveSegment/CurveSegmentLinear.x64.o
SRC32 += Sources/Editor/Scenario.i386.o
SRC64 += Sources/Editor/Scenario.x64.o

#########################################

INCLUDES = -I../../Headers/
INCLUDES += -Isupport/includes/
INCLUDE_FILES := $(wildcard INCLUDES/*.h)

#########################################

LIBS = support/lib/JamomaFoundation.dylib
LIBS += support/lib/JamomaDSP.dylib
LIBS += support/lib/JamomaModular.dylib
LIBS += support/lib/JamomaScore.dylib
LIBS += /System/Library/Frameworks/Carbon.framework/Versions/A/Carbon

#########################################

OPTIMIZATION_DEBUG = -O0 -DTT_ENABLE_ASSERTIONS
OPTIMIZATION_RELEASE = -O3

OPTIONS = -msse3 -gdwarf-2
OPTIONS += -std=c++11 
OPTIONS += -stdlib=libc++ # -U__STRICT_ANSI__ -D__STDC_FORMAT_MACROS
OPTIONS += -shared -mfpmath=sse
WARNINGS = -Wall -Wno-unknown-pragmas -Wno-trigraphs
DEFINES = -DTT_PLATFORM_MAC
#########################################

#DEFINES += -DOSSIA_API_EXPORTS

#########################################

CFLAGS = $(OPTIONS) $(DEFINES) $(INCLUDES) $(WARNINGS)
LDFLAGS =  -shared -mfpmath=sse $(OPTIONS) $(DEFINES) $(LIBS) $(WARNINGS)
LDFLAGS += -install_name "@rpath/$(NAME).dylib"

#########################################

Debug: OPTIMIZATION_FLAGS = $(OPTIMIZATION_DEBUG)
Debug: createdirs build

Release: OPTIMIZATION_FLAGS = $(OPTIMIZATION_RELEASE)
Release: createdirs build

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
    
	cp build/$(NAME).dylib /usr/local/lib # temporary for test with curve.cpp

	install_name_tool -change @loader_path/../../../../support/JamomaFoundation.dylib @rpath/JamomaFoundation.dylib build/$(NAME).$(SUFFIX)
	install_name_tool -change @loader_path/../../../../support/JamomaDSP.dylib @rpath/JamomaDSP.dylib build/$(NAME).$(SUFFIX)
	install_name_tool -change @loader_path/../../../../support/JamomaModular.dylib @rpath/JamomaModular.dylib build/$(NAME).$(SUFFIX)
	install_name_tool -change @loader_path/../../../../support/JamomaScore.dylib @rpath/JamomaScore.dylib build/$(NAME).$(SUFFIX)

clean:
	rm -f $(SRC32) $(SRC64)
	rm -rf build

build: | lipo

