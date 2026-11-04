CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -I./include
LDFLAGS =

SRCS = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)
OBJS = $(patsubst src/%.c,obj/%.o,$(SRCS))

# Il n'y a pas encore de main dans src : on compile les modules seuls.
all: $(OBJS)

obj/%.o: src/%.c $(HEADERS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

# Petit programme de demonstration du module d'alertes.
obj/test_alert: tests/test_alert.c obj/alert.o $(HEADERS)
	$(CC) $(CFLAGS) tests/test_alert.c obj/alert.o -o $@ $(LDFLAGS)

test: all obj/test_alert
	./obj/test_alert

clean:
	rm -f $(OBJS) obj/test_alert

fclean: clean
	rm -f esgi_edr

re:
	$(MAKE) fclean
	$(MAKE) all

.PHONY: all test clean fclean re
