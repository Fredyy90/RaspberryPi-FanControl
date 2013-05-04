TARGET   = FanControl

SRCDIR   = src
OBJDIR   = obj
BINDIR   = .

#DEBUG  = -g -O0
DEBUG    = -O3
CC       = gcc
INCLUDE  = -I$(SRCDIR)
CFLAGS   = $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS  = -L./wiringPi/wiringPi/
LDLIBS   = -lwiringPi

DESTDIR  = /usr
PREFIX   = /local

###############################################################################
# May not need to  alter anything below this line
###############################################################################

SRCS     := $(foreach dir, $(SRCDIR), $(notdir $(wildcard $(dir)/*.c)))
INCLUDES :=  $(addprefix -include, $(addprefix $(SRCDIR)/, $(foreach dir, $(SRCDIR), $(notdir $(wildcard $(dir)/*.h)))))
OBJS     := $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, $(SRCS)))
TBIN      = $(addprefix $(BINDIR)/, $(TARGET))
DESTPATH  = $(DESTDIR)$(PREFIX)
TINSTALL  = $(addprefix $(DESTPATH)/bin/, $(TARGET))
SUBDIRS   = $(SRCDIR) $(OBJDIR) $(BINDIR)

all: subdirs wiringPI $(TBIN)
		@echo "Everything build! Have Fun with \""$(TBIN)"\" or \"make install\"!"


$(addprefix $(OBJDIR)/, %.o): $(addprefix $(SRCDIR)/, %.c)
		@$(CC) $(CFLAGS) -c $< -o $@
		@echo "Compiled \"$(<)\" -> \"$(@)\" successfully!"


$(TBIN): $(OBJS)
		@$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) $(INCLUDES) -o $(TBIN)
		@echo "Linking \"$(TBIN)\" complete!"


.PHONY: subdirs
subdirs:
		@for dir in $(SUBDIRS); do				\
			test -d $$dir || mkdir -p $$dir; 	\
		done


.PHONY: install
install: all install-wiringPI
		@sudo install -m 0775 -d				$(DESTPATH)/bin
		@sudo install -m 0755 -s -b $(TBIN)		$(TINSTALL)
		@echo "Installed \"$(TARGET)\" to \"$(TINSTALL)\" complete!"


.PHONY: clean
clean: clean-wiringPI
		@$(RM) $(OBJS)
		@echo "Cleaning up \"$(OBJDIR)\" complete!"


.PHONY: dist-clean
dist-clean: clean
		@$(RM) $(TBIN)
		@echo "Executable \"$(TBIN)\" removed!"


###############################################################################
# Handling wiringPi Makefile
###############################################################################


.PHONY: wiringPI
wiringPI:
		@$(MAKE) -C ./wiringPi/wiringPi/ all
		@echo "Making \"wiringPi (Dynamic)\" complete!"


.PHONY: clean-wiringPI
clean-wiringPI:
		@$(MAKE) -C ./wiringPi/wiringPi/ clean
		@echo "Cleaning up \"wiringPi\" complete!"

.PHONY: install-wiringPI
install-wiringPI:
		@sudo $(MAKE) -C ./wiringPi/wiringPi/ install
		@echo "Installing \"wiringPi (Dynamic)\" complete!"
