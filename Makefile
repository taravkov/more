CC = gcc
CFLAGS = -Wall -m64
LDFLAGS = -m64
SRC = main.c commands.c terminal.c output.c
OBJ = $(SRC:.c=.o)
EXE = mymore

all: $(OBJ) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(EXE)
