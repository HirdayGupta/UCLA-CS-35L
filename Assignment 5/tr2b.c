#include <stdio.h> // To perform I/O ops
#include <stdlib.h> // For exit()
#include <string.h> // To perform string operations

int main(int argc, char** argv)
{
  // 3 because the program name itself also counts as an argument.
  // Hence we must check if there were two additional args
  if (argc != 3) 
  {
    fprintf(stderr, "Error! Incorrect no. of args provided. Please provide only 2 args.\n");
    exit(1);
  }

  char* from = argv[1];
  char* to = argv[2];

  // Now, checking if the two input strings are the same lenght.
  if (strlen(from) != strlen(to))
  {
    fprintf(stderr, "Error! Both arguments should be of the same length.\n");
    exit(1);
  }
  
  // Check if FROM has any duplicate chars:
  for (size_t i = 0; i < strlen(from); i++)
    for (size_t j = i + 1; j < strlen(from); j++)
    {
      if (from[j] == from[i])
      {
	fprintf(stderr, "Error! FROM contains duplicate bytes.\n");
	exit(1);
      }
    }

  // Now, transliterating from stdin to stdout using FROM and TO:
  int current = getchar();
  while (current != EOF)
  {
    for (size_t i = 0; i < strlen(from); i++)
    {
      if (current == from[i])
      {
	current = to[i];
	break;
      }
    }

    putchar(current);
    current = getchar();
  }
  return 0;
}
