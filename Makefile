CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2 -I./include
LDFLAGS = -ldl

SRCS = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)
OBJS = $(patsubst src/%.c,obj/%.o,$(SRCS))

# Compilation de l'EDR complet.
all: esgi_edr

esgi_edr: $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

obj/%.o: src/%.c $(HEADERS)
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

# Petit programme de demonstration du module d'alertes.
obj/test_alert: tests/test_alert.c obj/alert.o obj/state_table.o $(HEADERS)
	$(CC) $(CFLAGS) tests/test_alert.c obj/alert.o obj/state_table.o -o $@ $(LDFLAGS)

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
