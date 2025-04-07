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
  
  // get info using stdin:
  
  treasure_t new_treasure;
  
  printf("ID: ");
  scanf("%d", &new_treasure.id);
  printf("Name: ");
  scanf("%s", new_treasure.nume);
  printf("Latitude: ");
  scanf("%f", &new_treasure.latitude);
  printf("Longitude: ");
  scanf("%f", &new_treasure.longitude);
  printf("Treasure clue: ");
  scanf("%s", new_treasure.clue);
  printf("Treasure value: ");
  scanf("%d", &new_treasure.value);

  // write info in binary file:
  if(write(f, &new_treasure, sizeof(new_treasure)) == -1)
    {
      perror("Error writing info in binary file");
      exit(-1);
    }
  
  close(f);  // done!!!
  //__________________________________
  // Create log file for the hunt
  if((f = open(log_path, O_CREAT | O_WRONLY | O_APPEND, 0644)) == -1)  // we are going to append every single operation that happened.
    {
      perror("Error opening log file");
      exit(-1);
    }
  // activity goes to the log:
  const char *add_text = "Added ";
  write(f, add_text, strlen(add_text));
  write(f, id, strlen(id));
  write(f, "\n", 1);
  close(f);  // done with the log file
  //__________________________________
  // Create symbolic link for the log file
  if(symlink(log_path, log_symlink) == -1)
    {
      // On success, zero is returned.  On error, -1 is returned.
      perror("Error creating symbolic link");
      exit(-1);
    }
  //__________________________________
}

void list_treasures()
{
  // does something
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
  
  if(strcmp(argv[1], "--list") == 0)
    {
      list_treasures(argv[2]);
    }

  return 0;
}

