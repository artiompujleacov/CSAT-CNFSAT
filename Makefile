build:
	gcc -g -o sat sat.c
run:
	./sat $(INPUT) $(OUTPUT)
clean:
	rm sat