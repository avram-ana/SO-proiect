#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

struct stat st = {0};

typedef struct treasure
{
  char id[50];
  char nume[50];
  char latitude[50];
  char longitude[50];
  char clue[100];
  char value[50];
}treasure_t;

void add_hunt(const char *id)  // argv: something like hunt001
{
  char path[200], binary_file[200], log_path[200], log_symlink[200];
  int f;

  // create names for each directory or path
  strcpy(path, id);

  strcpy(binary_file, path);
  strcat(binary_file, "/treasure.bin");
  
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
  if((f = open(binary_file, O_CREAT | O_WRONLY | O_APPEND, 0644)) == -1)
    {
      // 0644 - only owner can read & write, others can only read the file.
      perror("Error opening binary file");
      exit(-1);
    }
  
  // get info using stdin:
  
  treasure_t new_treasure;
  
  printf("ID: ");
  scanf("%49s", new_treasure.id);
  strcat(new_treasure.id, " ");    // ,_, to signal end of string
  printf("Name: ");
  scanf("%49s", new_treasure.nume);
  strcat(new_treasure.nume, " ");
  printf("Latitude: ");
  scanf("%49s", new_treasure.latitude);
  strcat(new_treasure.latitude, " ");
  printf("Longitude: ");
  scanf("%49s", new_treasure.longitude);
  strcat(new_treasure.longitude, " ");
  printf("Treasure clue: ");
  scanf("%99s", new_treasure.clue);
  strcat(new_treasure.clue, " ");
  printf("Treasure value: ");
  scanf("%49s", new_treasure.value);
  strcat(new_treasure.value, " ");
  
  // write info in binary file:

  write(f, new_treasure.id, strlen(new_treasure.id));
  write(f, new_treasure.nume, strlen(new_treasure.nume));
  write(f, new_treasure.latitude, strlen(new_treasure.latitude));
  write(f, new_treasure.longitude, strlen(new_treasure.longitude));
  write(f, new_treasure.clue, strlen(new_treasure.clue));
  write(f, new_treasure.value, strlen(new_treasure.value));
  write(f, "\n", 1);
  
  close(f);  // done!!!
  
  //__________________________________
  // Create log file for the hunt
  if((f = open(log_path, O_CREAT | O_WRONLY | O_APPEND, 0644)) == -1)  // we are going to append every single operation that happened.
    {
      perror("Error opening log file");
      exit(-1);
    }
  // activity goes to the log:
  const char *add_text = "Added in ";
  write(f, add_text, strlen(add_text));
  write(f, id, strlen(id));
  write(f, "\n", 1);

  const char *treasure = "- Added treasure: ";
  write(f, treasure, strlen(treasure));
  write(f, new_treasure.nume, strlen(new_treasure.nume));
  write(f, "\n", 1);

  close(f);  // done with the log file
  //__________________________________
  // Create symbolic link for the log file
  struct stat link_stat;
  if (lstat(log_symlink, &link_stat) == -1)
    {
      if (errno == ENOENT)
	{
	  // symlink doesn't exist — safe to create it
	  if (symlink(log_path, log_symlink) == -1)
	    {
	      perror("Error creating symbolic link");
	      exit(EXIT_FAILURE);
	    }
	}
      else
	{
	  perror("Error checking symbolic link");
	  exit(EXIT_FAILURE);
	}
    }
  
  //__________________________________
}




void list_treasures(const char *id)  // list all the treasures in a certain hunt.
{
  char path[200], binary_file[200], log_path[200], line[500];  // we consider 500 to be the max_letter_number/line in the log file.
  int f, bin, nr_bytes;
  bool flag = false;  // were any treasures listed?
  
  // file paths
  strcpy(path, id);
  strcpy(binary_file, path);
  strcat(binary_file, "/treasure.bin");
  strcpy(log_path, path);
  strcat(log_path, "/logged_hunt");

  //____________________________________________________________________
  // open logs and search for previously-added treasures:
  if((f = open(log_path, O_RDWR | O_APPEND, 0644)) == -1)
    {
      perror("Error opening log file");
      exit(-1);
    }
  
  // navigate through the logs and see where a new_treasure was added + list it:
  
  while((nr_bytes = read(f, line, sizeof(line) - 1)) > 0) // while we can still navigate the logs:
    {
      flag = true;
      line[nr_bytes] = '\0';  // line ends with "\0"
      if(strstr(line, "Added treasure: "))  // found a treasure:
	{
	  char name[100];
	  if(sscanf(line, "Added treasure: %99s", name) == 1)  // scan from line the binary_file name:
	    {
	      // find binary_file:
	      strcpy(binary_file, id);
	      strcat(binary_file, "/");
	      strcat(binary_file, name);
	      strcat(binary_file, ".bin");
	      
	      // open binary_file:
	      if((bin = open(binary_file, O_RDONLY)) == -1)
		{
		  perror("Error opening file in list_treasures() function");
		  exit(-1);
		}
	      
	      // read data from binary_file:
	      treasure_t t;
	      while(read(bin, &t, sizeof(t)) == sizeof(t))
		{
		  printf("Treasure ID: %s\n", t.id);
		  printf("Name: %s\n", t.nume);
		  printf("Latitude: %s\n", t.latitude);
		  printf("Longitude: %s\n", t.longitude);
		  printf("Clue: %s\n", t.clue);
		  printf("Value: %s\n", t.value);
		  printf("______________________\n");
		}
	      close(bin);
	    }
	}
    }
  //____________________________________________________________________

  
  // we are going to append the "list treasures" OP.
  if(flag)  // there has been at least one treasure listed
    {
      const char *add_text = "Listed the treasures.\n";
      write(f, add_text, strlen(add_text));
    }

  close(f);
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
      // argv[2] is currently the name of the directory. 
      list_treasures(argv[2]);
    }

  return 0;
}
