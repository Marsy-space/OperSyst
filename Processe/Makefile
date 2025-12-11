CXX = g++
CXXFLAGS = -Wall
SOURSES =  KillerL.cpp UserL.cpp
TARGETS = KillerL UserL

all: $(TARGETS)

KillerL: KillerL.cpp
	$(CXX) $(CXXFLAGS) -o KillerL KillerL.cpp

UserL: UserL.cpp
	$(CXX) $(CXXFLAGS) -o UserL UserL.cpp

clean:
	rm -f $(TARGETS)

.PHONY: all clean

