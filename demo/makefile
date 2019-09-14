# test.out : test.c test2.c
# 	cc test.c test2.c -o test.out -lm
# clean:
# 	-rm test.out


CFLAGS = -w -lm
COMPILE = $(CC) $^ $(CFLAGS) -o $@

test.out : test.c test2.c
	$(COMPILE)

.PHONY : clean
clean:
	-rm test.out