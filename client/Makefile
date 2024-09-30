#
# Makefile for Fruit Ninja game using Dragonfly
#
# Copyright Mark Claypool and WPI, 2024
#
# 'make depend' to generate new dependency list
# 'make clean' to remove all constructed files
# 'make' to build executable
#
# Variables of interest:
#   GAMESRC is the source code files for the game
#   GAME is the game main() source
#

#### Adjust these as appropriate for build setup. ###
HOME= /home/km5762
DF= $(HOME)/Documents/cs4099/dragonfly

### Uncomment only 1 of the below! ###

# 1) Uncomment below for Linux (64-bit)
DFLIB= -ldragonfly-x64-linux
SFMLLIB= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 
LINKLIB= $(DFLIB) $(SFMLLIB) 
LINKDIR= -L$(DF)/lib -L$(HOME)/src/SFML/lib   
INCDIR= -I$(DF)/include -I$(HOME)/src/SFML/include 

# 2) Uncomment below for Mac (64-bit)
#DFLIB= -ldragonfly-mac64
#SFMLLIB= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 
#LINKLIB= $(DFLIB) $(SFMLLIB) 
#LINKDIR= -L/usr/local/Cellar/sfml/2.6.1/lib/ -L$(DF)/lib
#INCDIR= -I/usr/local/Cellar/sfml/2.6.1/include/ -I$(DF)/include

######

CC= g++
CFLAGS= -std=c++17 -DCLIENT

GAMESRC= \
	../shared/Fruit.cpp \
	../shared/Kudos.cpp \
	../shared/Points.cpp \
	../shared/Splash.cpp \
	../shared/Sword.cpp \
	../shared/Message.cpp \
	GameOver.cpp \
	../shared/Timer.cpp \
	ServerEntry.cpp \
	../shared/util.cpp \
	Client.cpp \
	Ping.cpp \

GAME= game.cpp
EXECUTABLE= fruit-ninja

OBJECTS= $(addprefix $(OBJDIR), $(notdir $(GAMESRC:.cpp=.o)))
OBJDIR = obj/

all: $(EXECUTABLE) Makefile 

$(EXECUTABLE): $(OBJECTS) $(GAME) $(GAMESRC) Makefile
	mkdir -p $(OBJDIR)
	$(CC) $(GAME) $(OBJECTS) $(CFLAGS) -o $@ $(INCDIR) $(LINKDIR) $(LINKLIB)

$(OBJDIR)%.o: %.cpp
	mkdir -p $(OBJDIR)
	$(CC) -c $(INCDIR) $(CFLAGS) $< -o $@

$(OBJDIR)%.o: ../shared/%.cpp
	mkdir -p $(OBJDIR)
	$(CC) -c $(INCDIR) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) core *.log Makefile.bak *~

depend: 
	makedepend *.cpp 2> /dev/null

# DO NOT DELETE
