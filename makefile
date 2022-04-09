CC		= g++
DEST	= /usr/local/dptran
BIN		= /usr/local/bin
LIBS	= -lcurl
OBJS	= main.o processes.o connect.o
PROGRAM = dptran

all:		
		$(PROGRAM)

$(PROGRAM): $(OBJS)
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:		
		rm -f *.o *~ $(PROGRAM)

install:	$(PROGRAM)
		mkdir $(DEST)
		install -s $(PROGRAM) $(DEST)
		ln -s $(DEST)/$(PROGRAM) $(BIN)

uninstall:
		rm -f $(BIN)/$(PROGRAM)
		rm -rf $(DEST)
		
