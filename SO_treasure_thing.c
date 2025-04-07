#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

struct stat st = {0};

typedef struct treasure
{
  int id;
  char nume[50];
  float latitude;
  float longitude;
  char clue[100];
  int value;
}treasure_t;

void add_hunt(const char *id)  // argv: something like hunt001
{
  char path[200], binary_file[200], log_path[200], log_symlink[200];
  int f;

  // create names for each directory or path
  strcpy(path, id);

  strcpy(binary_file, path);
  strcat(binary_file, "/treasure.txt");

  strcpy(log_path, path);
  strcat(log_path, "/logged_hunt");

  strcpy(log_symlink, "logged_hunt-");
  strcat(log_symlink, id);
  //__________________________________
  // Create a new directory with the "path" name for the new hunt
  if(stat(path, &st) == -1)  // does the directory exist?
    {
      if(errno == ENOENT)  // the directory doesn't exist | ENOENT - error no entry
	{
	  if(mkdir(path, 0755) == -1)  // 0: no special settings | 7: rwx | 5: rx
	    {
	      perror("mkdir failed");
	      exit(-1);
	    }
	}
      else
	{
	  perror("stat function failed");
	  exit(-1);
	}
    }
  //__________________________________
  // Create binary file named "binary_file"
  if((f = open(binary_file, O_CREAT | O_WRONLY, 0644)) == -1)
    {
      // 0644 - only owner can read & write, others can only read the file.
      perror("Error opening binary file");
      exit(-1);
    }
  // ------------------------------------------------------------------------------ some shit will happen here too
  close(f);
  //__________________________________
  // Create log file for the hunt
  if((f = open(log_path, O_CREAT | O_WRONLY, 0644)) == -1)
    {
      perror("Error opening log file");
      exit(-1);
    }
  // ------------------------------------------------------------------------------ some shit might happen here
  close(f);
  //__________________________________
  // Create symbolic link for the log file
  if(symlink(log_path, log_symlink) == -1)
    {
      perror("Error creating symbolic link");
      exit(-1);
    }
  //__________________________________
}


int main(int argc, char **argv)
{
  // ARGV : exe | OP | huntName
  if(argc != 3)
    {
      printf("argc != 3\n");
      exit(-1);
    }

  if(strcmp(argv[1], "--add") == 0)
    {
      add_hunt(argv[2]);
    }

  return 0;
}
