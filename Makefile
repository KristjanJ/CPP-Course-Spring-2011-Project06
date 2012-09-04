# To compile, run 'make' in the same directory as this file

#######################
# RakNet 4.x location #
#######################

RAKNETDIR = C:\Users\Kristjan\Desktop\Kristjan\University\C++\raknet

# On MinGW, remove comment from the following line:
EXTRALIBS = -lws2_32 -lwinmm
# On Linux, remove comment from the following line:
#EXTRALIBS = -lpthread

#########################
# Settings for the apps # 
#########################

# Compiler flags for the pp
APP_CFLAGS = -Wall -Iinclude -I$(RAKNETDIR)/Source
# Linker flags (order the compiler to link with our library)
LFLAGS = -L$(RAKNETDIR)/Lib/LibStatic -lRakNetStatic $(EXTRALIBS)
# The object for the apps
CLIENTOBJS = obj/GuessingClient.o obj/ClientApp.o
SERVEROBJS = obj/GuessingServer.o obj/ServerApp.o
# The names of the apps
CLIENTAPP = bin/GuessingClient
SERVERAPP = bin/GuessingServer


# This is the first target. It will be built when you run 'make' or 'make all'
all: $(CLIENTAPP) $(SERVERAPP) docs
# Rules for linking the test apps
$(CLIENTAPP): $(CLIENTOBJS)
	$(CXX) $(CLIENTOBJS) -o $(CLIENTAPP) $(LFLAGS)

$(SERVERAPP): $(SERVEROBJS)
	$(CXX) $(SERVEROBJS) -o $(SERVERAPP) $(LFLAGS)

# Compile each source file of the apps
obj/GuessingClient.o: src/GuessingClient.cpp
	$(CXX) $(APP_CFLAGS) -c src/GuessingClient.cpp -o obj/GuessingClient.o

obj/ClientApp.o: src/ClientApp.cpp
	$(CXX) $(APP_CFLAGS) -c src/ClientApp.cpp -o obj/ClientApp.o

obj/GuessingServer.o: src/GuessingServer.cpp
	$(CXX) $(APP_CFLAGS) -c src/GuessingServer.cpp -o obj/GuessingServer.o

obj/ServerApp.o: src/ServerApp.cpp
	$(CXX) $(APP_CFLAGS) -c src/ServerApp.cpp -o obj/ServerApp.o

clean: 
	rm -rf bin/* docs/* obj/*

doc: 
	doxygen
