# Target library
lib := libfs.a

all: $(lib)

objs    := fs.o disk.o
CC      := gcc
CFLAGS  := -Wall -Werror
CFLAGS  += -g

ifneq ($(V),1)
Q = @
endif

# Target library
all:  $(lib)

deps :=  $(patsubst %.o,%.d,$(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

libfs.a: $(objs)
	@echo "CC $@"
	ar rcs $(lib) $(objs)  
	
%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)
	
clean:
	@echo "clean"
	$(Q)rm -f $(targets) $(objs) $(deps)

