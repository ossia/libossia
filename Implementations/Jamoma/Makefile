NAME = OSSIA-API
SUFFIX = dylib

CC_32 = /usr/bin/clang++ -arch i386
CC_64 = /usr/bin/clang++ -arch x86_64

#########################################

SRC32 = Sources/Editor/Automation.i386.o
SRC64 += Sources/Editor/Automation.x64.o
SRC32 += Sources/Editor/Clock.i386.o
SRC64 += Sources/Editor/Clock.x64.o
SRC32 += Sources/Editor/Curve.i386.o
SRC64 += Sources/Editor/Curve.x64.o
SRC32 += Sources/Editor/CurveSegment/CurveSegmentEmpty.i386.o
SRC64 += Sources/Editor/CurveSegment/CurveSegmentEmpty.x64.o
SRC32 += Sources/Editor/CurveSegment/CurveSegmentLinear.i386.o
SRC64 += Sources/Editor/CurveSegment/CurveSegmentLinear.x64.o
SRC32 += Sources/Editor/CurveSegment/CurveSegmentPower.i386.o
SRC64 += Sources/Editor/CurveSegment/CurveSegmentPower.x64.o
SRC32 += Sources/Editor/Domain.i386.o
SRC64 += Sources/Editor/Domain.x64.o
SRC32 += Sources/Editor/Expression.i386.o
SRC64 += Sources/Editor/Expression.x64.o
SRC32 += Sources/Editor/ExpressionAtom.i386.o
SRC64 += Sources/Editor/ExpressionAtom.x64.o
SRC32 += Sources/Editor/ExpressionComposition.i386.o
SRC64 += Sources/Editor/ExpressionComposition.x64.o
SRC32 += Sources/Editor/ExpressionNot.i386.o
SRC64 += Sources/Editor/ExpressionNot.x64.o
SRC32 += Sources/Editor/Loop.i386.o
SRC64 += Sources/Editor/Loop.x64.o
SRC32 += Sources/Editor/Mapper.i386.o
SRC64 += Sources/Editor/Mapper.x64.o
SRC32 += Sources/Editor/Message.i386.o
SRC64 += Sources/Editor/Message.x64.o
SRC32 += Sources/Editor/Scenario.i386.o
SRC64 += Sources/Editor/Scenario.x64.o
SRC32 += Sources/Editor/State.i386.o
SRC64 += Sources/Editor/State.x64.o
SRC32 += Sources/Editor/TimeConstraint.i386.o
SRC64 += Sources/Editor/TimeConstraint.x64.o
SRC32 += Sources/Editor/TimeEvent.i386.o
SRC64 += Sources/Editor/TimeEvent.x64.o
SRC32 += Sources/Editor/TimeNode.i386.o
SRC64 += Sources/Editor/TimeNode.x64.o
SRC32 += Sources/Editor/TimeProcess.i386.o
SRC64 += Sources/Editor/TimeProcess.x64.o
SRC32 += Sources/Editor/TimeValue.i386.o
SRC64 += Sources/Editor/TimeValue.x64.o
SRC32 += Sources/Editor/Value.i386.o
SRC64 += Sources/Editor/Value.x64.o
SRC32 += Sources/Network/Address.i386.o
SRC64 += Sources/Network/Address.x64.o
SRC32 += Sources/Network/Device.i386.o
SRC64 += Sources/Network/Device.x64.o
SRC32 += Sources/Network/Node.i386.o
SRC64 += Sources/Network/Node.x64.o
SRC32 += Sources/Network/Protocol.i386.o
SRC64 += Sources/Network/Protocol.x64.o
SRC32 += Sources/Network/Protocol/Local.i386.o
SRC64 += Sources/Network/Protocol/Local.x64.o
SRC32 += Sources/Network/Protocol/MIDI.i386.o
SRC64 += Sources/Network/Protocol/MIDI.x64.o
SRC32 += Sources/Network/Protocol/Minuit.i386.o
SRC64 += Sources/Network/Protocol/Minuit.x64.o
SRC32 += Sources/Network/Protocol/OSC.i386.o
SRC64 += Sources/Network/Protocol/OSC.x64.o
SRC32 += Sources/Misc/CallbackContainer.i386.o
SRC64 += Sources/Misc/CallbackContainer.x64.o

#########################################

INCLUDES = -I../../Headers/
INCLUDES += -IIncludes/
INCLUDES += -I/usr/local/jamoma/include/
INCLUDE_FILES := $(wildcard INCLUDES/*.h)

#########################################

LIBS = /usr/local/jamoma/lib/libJamomaFoundation.dylib
LIBS += /usr/local/jamoma/lib/libJamomaDSP.dylib
LIBS += /usr/local/jamoma/lib/libJamomaModular.dylib
LIBS += /usr/local/jamoma/lib/libJamomaScore.dylib
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
	install_name_tool -add_rpath /usr/local/jamoma/lib build/$(NAME).$(SUFFIX)
	cp build/$(NAME).dylib /usr/local/lib/$(NAME).dylib # useful to run Documentation/Examples projects

clean:
	rm -f $(SRC32) $(SRC64)
	rm -rf build

build: | lipo

