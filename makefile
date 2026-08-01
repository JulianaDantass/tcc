# Name of the project
PROJ_NAME=vrp

# .cpp files
CPP_SOURCE=$(wildcard ./source/*.cpp)

# .h files
H_SOURCE=$(wildcard ./source/*.h)

# Object files
OBJ=$(subst .cpp,.o,$(subst source,objetos,$(CPP_SOURCE)))

# Compiler and linker
CC=g++

# Flags for compiler
CXXFLAGS = -Wall -g -fsanitize=address -fno-omit-frame-pointer

# Command used at clean target
RM = rm -rf

#
# Compilation and linking
#
all: objFolder $(PROJ_NAME)

$(PROJ_NAME): $(OBJ)
	@ echo 'Building binary using G++ linker: $@'
	$(CC) $^ $(CXXFLAGS) -o $@
	@ echo 'Finished building binary: $@'
	@ echo ' '

./objetos/%.o: ./source/%.cpp ./source/%.h
	@ echo 'Building target using G++ compiler: $<'
	$(CC) -c $< $(CXXFLAGS) -o $@
	@ echo ' '

./objetos/main.o: ./source/main.cpp $(H_SOURCE)
	@ echo 'Building target using G++ compiler: $<'
	$(CC) -c $< $(CXXFLAGS) -o $@
	@ echo ' '

objFolder:
	@ mkdir -p objetos

clean:
	@ $(RM) ./objetos/*.o $(PROJ_NAME) *~
	@ rmdir objetos

.PHONY: all clean
