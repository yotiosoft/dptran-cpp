SHELL	= /bin/bash
CC		= g++
DEST	= $(HOME)/.dptran
LIBS	= -lcurl
OBJS	= main.o processes.o connect.o settings.o
PROGRAM = dptran

$(PROGRAM): $(OBJS)
		$(CC) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:		
		sudo rm -f *.o *~ $(DEST)

install:	$(PROGRAM)
		mkdir -p $(DEST)
		install -s $(PROGRAM) $(DEST)
		export PATH=$(PATH):$(DEST)
		source $(HOME)/.bashrc

uninstall:
		export -n PATH=$PATH:$(DEST)
		rm -rf $(DEST)
		
