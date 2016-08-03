###########################################
# Weather station
#
# Jakub Kaderka 2016
###########################################
#dirs
OBJDIR=obj
SRCDIRS=src

#compiler and linker config
CC=gcc
LD=gcc
MYSQL_LIBS=$(shell mysql_config --libs)
CFLAGS=-Wall -O2 -I $(SRCDIRS) -I /usr/include/mysql
LDFLAGS=-lsqlite3 -lm $(MYSQL_LIBS)


#resulting bin
TARGET=weather_daemon

C_SRC = $(shell find $(SRCDIRS) -name '*.c')
OBJS = $(addprefix $(OBJDIR)/, $(C_SRC:.c=.o))
DEPS = $(addprefix $(OBJDIR)/, $(C_SRC:.c=.d))
DIRS=$(addprefix $(OBJDIR)/, $(shell find $(SRCDIRS) -type d))

.PHONY : clean all hierarchy

all: hierarchy $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(TARGET)

#create file hierarchy
hierarchy: $(OBJDIR) $(DIRS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(DIRS):
	mkdir -pv $(DIRS)

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) -MM -MT '$@ $(basename $@).d' $< > $(basename $@).d

-include $(DEPS)

clean:
	-rm -r $(TARGET) $(OBJDIR)
