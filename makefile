OBJS = $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))
TESTS = $(patsubst test/%.c, bin/%, $(wildcard test/*.c))
LIB = lib/libvm.so
LIB_NAME = vm


$(LIB): $(OBJS)
	gcc -shared -o $@ $^

.PHONY: build_test
build_test: $(TESTS) 
	
bin/%: test/%.c $(LIB)
	gcc -o $@ $< -Iinclude/ -Llib/ -l$(LIB_NAME)

obj/%.o: src/%.c
	gcc -fPIC -c -o $@ $< -I include/

.PHONY: clean
clean:
	rm $(OBJS) $(LIB) $(TESTS)