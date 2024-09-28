#include "include/IO.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_file_contents(const char* filepath)
{
  char* buffer = 0;
  long length;

  FILE* f = fopen(filepath, "rb");
  int len = strlen(filepath);

  if (len < 4  || 
    filepath[len - 3] != 'e' || 
    filepath[len - 2] != 'p' || 
    filepath[len - 1] != 'p' )
  {
    printf("File must be of type .epp\n");
    exit(1);
  }

  if(f)
  {
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = calloc(length, length);

    if(buffer)
    {
      fread(buffer, 1, length, f);
    }
    fclose(f);
    return buffer;
  }

  printf("Error reading file %s\n", filepath);
  exit(2);
}
