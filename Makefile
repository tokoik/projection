TARGET	= projection
SOURCES	= $(wildcard *.cpp)
HEADERS	= $(wildcard *.h)
OBJECTS	= $(patsubst %.cpp,%.o,$(SOURCES))
CXXFLAGS	= --std=c++0x -Wall -DX11
LDLIBS	= -lGL -lglfw3 -lXrandr -lXinerama -lXcursor -lXxf86vm -lX11 -lopencv_highgui -lopencv_core -lpthread -lm

.PHONY: clean

$(TARGET): $(OBJECTS)
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(TARGET).dep: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -MM $(SOURCES) > $@

clean:
	-$(RM) $(TARGET) *.o *~ .*~ a.out core

-include $(TARGET).dep
