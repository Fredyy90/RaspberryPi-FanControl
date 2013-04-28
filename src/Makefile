TARGET   = FanControl

SRCDIR   = src
OBJDIR   = obj
BINDIR   = .

#DEBUG  = -g -O0
DEBUG    = -O3
CC       = gcc
INCLUDE  = -I$(SRCDIR)
CFLAGS   = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS  = -Lusrlocallib
LDLIBS   = -lwiringPi
# May not need to  alter anything below this line
###############################################################################
SRCS     := $(wildcard $(SRCDIR)/*.c)
#INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJS     := $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

DIRGUARD = @mkdir -p $(@D)

all: $(TARGET)

$(TARGET): $(OBJS)
		$(DIRGUARD)
		@$(CC) $(DEBUG) $(LDFLAGS) -o $(BINDIR)/$(TARGET) $(OBJS) $(LDLIBS)
		@echo "Linking "$(BINDIR)"/"$(TARGET)" complete!"

$(OBJDIR)/%.o: %.c
		$(DIRGUARD)
		@$(CC) -c $(DEBUG) $(CFLAGS) $(INCLUDES) $(SRCDIR)/$*.c -o $(OBJDIR)/$*.o
		@echo "Compiled "$(SRCDIR)"/"$<" -> "$(OBJDIR)"/"$*" successfully!"

clean:
		@$(RM) $(OBJDIR)/*.o
		@echo "Cleanup "$(OBJDIR)" complete!"

dist-clean: clean
		@$(RM) $(BINDIR)/$(TARGET)
		@echo "Executable "$(BINDIR)"/"$(TARGET)" removed!"

# DO NOT DELETE
