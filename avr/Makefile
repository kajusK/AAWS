###########################################
# Weather station
#
# Jakub Kaderka 2016
###########################################
#used MCU
MCU=atmega32
#used frequency
F_CPU=8000000UL

#program name
PROGRAM=weather

#dirs
OBJDIR=obj
SRCDIRS=src

#compiler and linker config
CC=avr-gcc
LD=avr-gcc
CFLAGS=-Wall -Os -I $(SRCDIRS) -DF_CPU=$(F_CPU) -mmcu=$(MCU) -std=gnu99 -Wstrict-prototypes -ffunction-sections -fdata-sections
LDFLAGS=-mmcu=$(MCU) -Wl,--gc-sections
OBJCOPY=avr-objcopy
UPLOADER=avrdude
UPLOADER_FLAGS=-c usbtiny -p $(MCU) -B 1

#resulting bin
TARGET=$(PROGRAM).hex

C_SRC = $(shell find $(SRCDIRS) -name '*.c')
OBJS = $(addprefix $(OBJDIR)/, $(C_SRC:.c=.o) $(AS_SRC:.s=.o))
DEPS = $(addprefix $(OBJDIR)/, $(C_SRC:.c=.d))
DIRS=$(addprefix $(OBJDIR)/, $(shell find $(SRCDIRS) -type d))

.PHONY : clean all hierarchy program

all: hierarchy $(TARGET)


#create file hierarchy
hierarchy: $(OBJDIR) $(DIRS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(DIRS):
	mkdir -pv $(DIRS)

#compile sources
$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) -MM -MT '$@ $(basename $@).d' $< > $(basename $@).d

#link them together
$(OBJDIR)/$(PROGRAM).elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(OBJDIR)/$(PROGRAM).elf

$(TARGET): $(OBJDIR)/$(PROGRAM).elf
	$(OBJCOPY) -O ihex -R .eeprom $(OBJDIR)/$(PROGRAM).elf $(TARGET)
	avr-size $(TARGET)

-include $(DEPS)

clean:
	-rm -r $(TARGET) $(OBJDIR)

program:
	$(UPLOADER) $(UPLOADER_FLAGS) -U flash:w:$(TARGET)
