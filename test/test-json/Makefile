# a simple test that the c string containing the keyer json file
# is in fact a json string when read out of memory
# ie, does it have enough backslashes
test: test-json
	./test-json | json_pp > /dev/null && ./test-json 2 | json_pp > /dev/null

test-json: test-json.c ../../json.h
	cc -o test-json test-json.c
