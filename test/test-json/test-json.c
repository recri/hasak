#include <unistd.h>
#include "../../json.h"
int main(int argc, char *argv[]) { write(1, json_string, sizeof(json_string)-1); }
