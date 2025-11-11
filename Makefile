# Author: Edwin Baiden
# Version: 1.0
# Description: Creating a Makefile for our DND Dungeon Master Project in C++.
#              This make file will help compile and link the source files as well as
#              raylib files to create a final executable file. This make file will also
#              take into consideration the OS the user is using to ensure compatibility 
#              (Windows, MacOS, or Linux). In order to use this make file,
#              the user (Professor Helsing) must have raylib installed.
#
# How to use: Run the "make" command in the terminal in the directory where the 
#             Makefile is located.

CXX := g++ # Compiler

# Compiler flags: C++17 standard, include warnings, let compiler know that there are header files in the src folder too
#    			  These standards could be overridden when calling the make command in the terminal. 
#                 This is just to insure that if the user (Professor Helsing) does not have c++17 he can still compile with
#                 the following "make CXXFLAGS="-std=c++11 -Wall -I./src""
CXXFLAGS ?= -std=c++17 -Wall -I./src

TARGET_NAME  := TheLastLift
SRC_DIR := src
TARGET:= $(SRC_DIR)/$(TARGET_NAME) # The final executable we want to create

# Defining all the source files we want to compile and link togeter in our final executable
# May grow or shrink as we add more features and polish stuff up
SRCS := \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/screenManager.cpp \
	$(SRC_DIR)/characters.cpp \
	$(SRC_DIR)/rng.cpp 

OBJS := $(SRCS:.cpp=.o) # The object files we want to create from the src files (just replacing .cpp with .o from what i understand)

LDFLAGS := # default linker flags (will be set based on OS later)
LDLIBS  := # default libraries for linking (this will also be set based on OS later)
RM := # Command to remove files (OS dependent, will be set later)

# Detecting the OS and setting the appropriate flags and libraries for raylib
# On Windows, $(OS) is already set to Windows_NT so we don't need to call uname
ifeq ($(OS),Windows_NT)     # This is for the windows case (OS is a free environment variable in windows)
	CXXFLAGS  += -IC:/msys64/ucrt64/include 
	LDFLAGS   += -LC:/msys64/ucrt64/lib 
	LDLIBS    += -lraylib -lopengl32 -lgdi32 -lwinmm
	RM        := del /Q
	OBJSTOCLEAN := $(subst /,\,$(OBJS)) # Apparently windows uses "\" instead of "/" for file paths
else
	# Non-Windows (Linux or macOS), here we can safely call uname
	UNAME_S := $(shell uname -s)  # Detecting the OS (Linux, MacOS (Darwin))
	RM	   := rm -f 
	OBJSTOCLEAN := $(OBJS)
	ifeq ($(UNAME_S),Darwin)
		CXXFLAGS += -I/opt/homebrew/include -I/usr/local/include
		LDFLAGS  += -L/opt/homebrew/lib -L/usr/local/lib
		LDLIBS += -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	else
		# Linux
		LDLIBS += -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
	endif
endif

all: $(TARGET) # Default to target(the executable) when "make" command is run
	

# Link it all toghether and make the executable
$(TARGET): $(OBJS)#Basically saying that to make the target we need all the object files
	$(CXX) $(OBJS) $(LDFLAGS) -o $@ $(LDLIBS)
	$(RM) $(OBJSTOCLEAN) # Cleaning up the object files after they are linked
	

# Rule for compiling .cpp files to .o files (but inside src/)
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	

run: $(TARGET) # Run the executable
	./$(TARGET) # Execute the target file
	

clean:           # Clean up the build files
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean run # Phony targets (not files)


