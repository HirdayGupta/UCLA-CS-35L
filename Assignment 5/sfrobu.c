#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>

char unfrobByte(const char a);
int frobcmp(char const* a, char const* b);
int checkForIOError(ssize_t IOStatus)
{
  if (IOStatus < 0)
  {
    fprintf(stderr, "Read/Write sys call error! Exiting...\n");
    return 0;
  }
  return 1;
}

int f_flag = 0; // Default to false.

// Generic Function to call frbcmp from void* type data, to send to qsort.
int genericFrbCmp(const void* a, const void* b)
{
  // Typecasting to double pointers, as thats the datatype of allTheWords
  // and allTheWords is going to be sorted.
  const char* inp1 = *(const char**)a;
  const char* inp2 = *(const char**)b;
  return frobcmp(inp1, inp2);
}

int main(int argc, char** argv)
{
  if (argc > 2)
  {
    fprintf(stderr, "Error! Incorrect no. of args. Only option is -f.\n");
    exit(1);
  }

  char* currentWord; // Its a pointer so that it can store a variably sized string.
  char** allTheWords; // Double pointer to store pointers to "currentWords".

  const char* opt = argv[1];
  const char* f_opt = "-f";

  if (argc == 2 && strcmp(opt, f_opt) == 0)
  {
    // if -f is given, set f_flag to true
    f_flag = 1;
  } else if (argc == 2 && strcmp(opt, f_opt) != 0) {
    // Checking if additional option is NOT '-f'
    fprintf(stderr, "Incorrect option provided! Only -f allowed.\n");
    exit(1);
  }

  struct stat fileData;
  if (fstat(0, &fileData) < 0)
  {
    fprintf(stderr, "fstat encountered an error! Exiting... ");
    exit(1);
  }

  char* fileBuffer;
  int fileBufferCounter = 0; // counts how many bytes are remaining in fileBuffer
  int fileBufferMaxSize = 0;
  if (S_ISREG(fileData.st_mode))
  {
    fileBuffer = (char*)malloc(fileData.st_size * sizeof(char)); // allocating mem to store entire inp
    if (fileBuffer == NULL)
    {
      fprintf(stderr, "Memory Allocation Error! Exiting...\n");
      exit(1);
    }
    if (read(0, fileBuffer, fileData.st_size) < 0) // storing inp
    {
      fprintf(stderr, "Read Error! Exiting...\n");
      exit(1);
    }
    fileBufferMaxSize = fileData.st_size;
  }
  
  currentWord = (char*)malloc(sizeof(char)); // give space for 1 character.
  if (currentWord == NULL) // checking for memalloc erorr
  {
    fprintf(stderr, "Memory Allocation Error! Exiting...");
    exit(1);
  }
  allTheWords = NULL; // no words yet, so null.
  int n = 0; // n indicates how many letters are in currentWword.
  int m = 0; // m indicates how many words there are in allTheWords

  ssize_t IOState;
  if (fileBufferCounter >= fileBufferMaxSize)
  {
    IOState = read(0, (currentWord), 1);
    if (!checkForIOError(IOState)) // getting the first character from stdin.
    {
      free(currentWord);
      exit(1);
    }
  }
  else {
    currentWord[n] = fileBuffer[fileBufferCounter++];
    IOState = 1;
  }
  
  while (IOState > 0) // while char isnt EOF and no IOError
  {
    char next;

    if (fileBufferCounter >= fileBufferMaxSize)
    {
      IOState = read(0, &(next), 1);
      if (!checkForIOError(IOState)) // getting the first character from stdin.
      {
	free(currentWord);
	for (int i = 0; i < m; i++)
	  free(allTheWords[i]);
	free(allTheWords);
	exit(1);
      }
    }
      else
	next = fileBuffer[fileBufferCounter++];
        
    if (currentWord[n] == ' ') // if space, then new word
    {
      // reserve space for m (current no) of words + 1
      char** temp = realloc(allTheWords, m*sizeof(char*) + sizeof(char*));
      if (temp != NULL)
      {
	allTheWords = temp; // if realloc succesful, then point allTheWords to new space
	allTheWords[m] = currentWord; // point last element of allTheWords to currentWord
	m++;
	n = -1; // -1 so that the future ++ makes it zero.
	currentWord = (char*)malloc(sizeof(char)); // make space for one char.
      } else {
	free (currentWord); // free all memory and exit if realloc unsuccesful.
	for (int i = 0; i < m; i++)
	  free(allTheWords[i]);
	free (allTheWords);
	fprintf(stderr, "Memory Allocation Error! Exiting...\n");
	exit(1);
      }
      if (IOState == 0) // if current is space and there is no next char, then exit program.
	break;
      while (next == ' ')  // if current is space, while next is space, consume next chars
      {
	if (fileBufferCounter >= fileBufferMaxSize)
	{
	  IOState = read(0, &(next), 1);
	  if (!checkForIOError(IOState)) // getting the first character from stdin.
	  {
	    free(currentWord);
	    for (int i = 0; i < m; i++)
	      free(allTheWords[i]);
	    free(allTheWords);
	    exit(1);
	  }
	}
	  else
	    next = fileBuffer[fileBufferCounter++];
      }
    } else if (IOState == 0) { // if current is NOT space, and next is EOF, then add space.
      next = ' ';
      IOState = 1;
    }
    
    n++; // increase char count in currentWord.
    char* temp = realloc(currentWord, n*sizeof(char*) + sizeof(char*));
    if (temp != NULL)
    {
      currentWord = temp; // if realloc successful, then point currentWord to new mem
      currentWord[n] = next; // point last element of currWord to the next character.
    }
    else
    {
      free (currentWord); // free all memory and exit if realloc unsuccesful.
      for (int i = 0; i < m; i++)
	free(allTheWords[i]);
      free (allTheWords);
      fprintf(stderr, "Memory Allocation Error! Exiting...\n");
      exit(1);
    }
  }

  qsort(allTheWords, m, sizeof(char*), genericFrbCmp);

  // Write sorted list to stdout.
  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; allTheWords[i][j] != ' '; j++)
    {
      write(1, &allTheWords[i][j], 1);
    }
    write(1, " ", 1);
  }

  // free all allocated memory and exit with code 0.
  for (size_t i = 0; i < m; i++)
    free (allTheWords[i]);
  free (allTheWords);
  free (currentWord);
  return 0;
}

char unfrobByte(const char a)
{
  // New info: ^ is C's XOR operator!
  // Also, 00101010 is 42 in int.
  // And the ^ operator accepts two int operands.
  // and yes, a is a char but is also int when convenient
  // because typecasting.
  return toupper((a^42));
}

int frobcmp(char const* a, char const* b)
{
  for(; ;a++, b++)
  {
    while (*a == '\0') // Null Bytes are allowed and DO NOT contribute
      a++;
    
    while (*b == '\0')
      b++;

    if (*a == ' ' && *b == ' ') // Space indicates end of word
    {
      return 0; // if both words ended, then return zero.
    }
    
    if (*a == ' ' || unfrobByte(*a) < unfrobByte(*b))
    {
      // Either a ends before b, or a < b.
      return -1;
    }

    if (*b == ' ' || unfrobByte(*b) < unfrobByte(*a))
    {
      return 1; // if b ends before a, or b < a.
    }
  }
}
