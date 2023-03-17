# Target library
lib := libfs.a
objs := fs.o disk.o
CFLAGS := -Wall -Wextra -Werror -MMD
CC := gcc

ifneq ($(V),1)
	Q := @
endif

all: $(lib)

deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)

$(lib): $(objs)
	@echo "Building static library $(lib)"
	$(Q)ar rcs $@ $^

%.o: %.c
	@echo "Building object file $(@)"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "Cleaning files $^"
	$(Q)rm -f $(lib) $(objs) $(deps)
