CC		= g++
DEST	= /usr/local/dptran
BIN		= /usr/local/bin
LIBS	= -lcurl
OBJS	= main.o processes.o connect.o settings.o
PROGRAM = dptran

$(PROGRAM): $(OBJS)
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:		
		sudo rm -f *.o *~ $(PROGRAM)

install:	$(PROGRAM)
		sudo mkdir -p $(DEST)
		sudo install -s $(PROGRAM) $(DEST)
		sudo ln -f -s $(DEST)/$(PROGRAM) $(BIN)

uninstall:
		sudo rm -f $(BIN)/$(PROGRAM)
		sudo rm -rf $(DEST)
		
