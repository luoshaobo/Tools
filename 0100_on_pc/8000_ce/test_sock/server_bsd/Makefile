CC = g++
CFLAGS = -MD -Wall -O2 -g 
CPP := $(CC) -E
CPPFLAGS =
CXX := $(CC)
CXXFLAGS := $(CFLAGS)

DES = sock
OBJ := $(DES).o

all : $(DES) depend

$(DES) : $(OBJ)
	$(CC) -o $(DES) $(OBJ)

clean :
	@rm -f $(DES)
	@rm -f *.o
	@rm -f *.d
	@rm -f *~
	@rm -f .depend

run :
	./$(DES)

depend : $(OBJ)
	@rm -f .depend
	@rm -f __tmp.d
	@touch __tmp.d
	@cat *.d >> .depend
	@rm -f __tmp.d
