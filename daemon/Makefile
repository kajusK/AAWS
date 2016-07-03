###########################################
# Weather station
#
# Jakub Kaderka 2016
###########################################
#compiler and linker config
CC=gcc
LD=gcc
CFLAGS=-Wall -O2
LDFLAGS=-lsqlite3 -lm

#dirs
OBJDIR=obj
SRCDIRS=src

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
