#include <unistd.h>
#include "../../json.h"
int main(int argc, char *argv[]) { 
  if (argc == 1)
    write(1, json_string, sizeof(json_string)-1);
  else 
    write(1, json_string2, sizeof(json_string2)-1);
}

