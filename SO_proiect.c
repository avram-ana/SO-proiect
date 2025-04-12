#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>

#define MAX 500

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

long get_file_size(char *filename)
{
  struct stat file_status;
  if (stat(filename, &file_status) < 0)
    {
      return -1;
    }
  
  return file_status.st_size;
}

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


//______________________________________________________________________________________________


void list_treasures(const char *id)  // list all the treasures in a certain hunt.
{
  char path[200], binary_file[200], log_path[200], line[MAX];  // we consider 500 to be the max_letter_number/line in the log file.
  int f, bin, nr_bytes;
  bool flag = false;  // were any treasures listed?

  // file paths
  strcpy(path, id);
  strcpy(binary_file, path);
  strcat(binary_file, "/treasure.bin");
  strcpy(log_path, path);
  strcat(log_path, "/logged_hunt");

  //____________________________________________________________________
  // Binary file size:
  printf("\nBinary file size: %ld\n", get_file_size(binary_file));
  //____________________________________________________________________
  // Date last modified:

  struct stat attrib;
  if (stat(binary_file, &attrib) == -1)
    {
      perror("stat");
    }
  
  char date[100];
  strftime(date, sizeof(date), "%d-%m-%Y %H:%M:%S", localtime(&attrib.st_mtime));
  printf("Last modified: %s\n", date);
  
    //____________________________________________________________________
  // open logs:
  if((f = open(log_path, O_RDWR | O_APPEND, 0644)) == -1)
    {
      perror("Error opening log file");
      exit(-1);
    }

  //____________________________________________________________________
  // open binary_file:
  if((bin = open(binary_file, O_RDONLY, 0644)) == -1)
    {
      perror("Error opening binary file");
      exit(-1);
    }


  //____________________________________________________________________
  // navigate through the treasure.bin:
  int count = 0;
  char buffer[MAX];
  int i = 0;

  while((nr_bytes = read(bin, buffer, sizeof(line))) > 0)
    {
      for (int j = 0; j < nr_bytes; j++)
        {
          if (buffer[j] == '\n')  // end of line
            {
              line[i] = '\0';
              i = 0; // reset for next line
              flag = true;

              //printf("\n\nLine is: %s:\n\n ", line);

              printf("\nTreasure %d: ", ++count);

              char *token= strtok(line, " ");  // ID
              printf("ID: %s | ", token);
              token = strtok(NULL, " ");  // NAME
              printf("Name: %s | ", token);

              token = strtok(NULL, " ");  // LATITUDE
              printf("Latitude: %s | ", token);

              token = strtok(NULL, " ");  // LONGITUDE
              printf("Longitude: %s | ", token);

              token = strtok(NULL, " ");  // CLUE
              printf("Clue: %s | ", token);

              token = strtok(NULL, "\n");  // VALUE
              printf("Value: %s\n\n", token);

            }
            else
              {
                if (i < MAX - 1)
                  {
                    line[i++] = buffer[j];  // add to line what we have in buffer (char by char)
                  }
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


//______________________________________________________________________________________________

void view_details(const char *hunt_id, const char *treasure_id)
{
  char path[200], binary_file[200], log_path[200], line[MAX];
  int f, bin, nr_bytes;
  bool flag = false;  // did we find the desired treasure?

  // file paths
  strcpy(path, hunt_id);
  strcpy(binary_file, path);
  strcat(binary_file, "/treasure.bin");
  strcpy(log_path, path);
  strcat(log_path, "/logged_hunt");

  //____________________________________________________________________
  // open logs:
  if((f = open(log_path, O_RDWR | O_APPEND, 0644)) == -1)
    {
      perror("Error opening log file");
      exit(-1);
    }

  //____________________________________________________________________
  // open binary_file:
  if((bin = open(binary_file, O_RDONLY, 0644)) == -1)
    {
      perror("Error opening binary file");
      exit(-1);
    }

  //____________________________________________________________________
  
  char buffer[MAX];
  int i = 0;
  // navigate through treasure.bin:
  while((nr_bytes = read(bin, buffer, sizeof(line))) > 0)
    {
      for (int j = 0; j < nr_bytes; j++)
        {
          if (buffer[j] == '\n')  // end of line
            {
	      line[i] = '\0';
              i = 0; // reset for next line (might be needed or might be not)

	      char *token = strtok(line, " ");  // current ID
	      if(strcmp(token, treasure_id) == 0)  // we found the treasure we are looking for
		{
		  flag = true;
		  printf("ID: %s | ", token);
		  
		  token = strtok(NULL, " ");  // NAME
		  printf("Name: %s | ", token);
		  
		  token = strtok(NULL, " ");  // LATITUDE
		  printf("Latitude: %s | ", token);
		  
		  token = strtok(NULL, " ");  // LONGITUDE
		  printf("Longitude: %s | ", token);
		  
		  token = strtok(NULL, " ");  // CLUE
		  printf("Clue: %s | ", token);
		  
		  token = strtok(NULL, "\n");  // VALUE
		  printf("Value: %s\n\n", token);
		  break;
		}
	      else
		{
		  continue;  // keep searching for the treasure
		}
	    }
            else
              {
                if (i < MAX - 1)
                  {
                    line[i++] = buffer[j];  // add to line what we have in buffer (char by char)
                  }
              }

        }
      if(flag)
	      {
	        break;  // no need to keep the while going; break
    	  }
    }

  if(!flag)
    {
      printf("Couldn't find treasure.\n");
    }
  else
    {
      const char *add_text = "Viewed a certain treasure.\n";
      write(f, add_text, strlen(add_text));
    }

  close(f);
}


int main(int argc, char **argv)
{
  // ARGV : exe | OP | huntName | treasureID (optional)
  if(argc < 3)
    {
      printf("Argc should be 3 or 4, depending on the desired function.\n");
      printf("add - 3 | list - 3 | view - 4\n");
      exit(-1);
    }

  if(strcmp(argv[1], "--add") == 0)
    {
      add_hunt(argv[2]);
    }
  else if(strcmp(argv[1], "--list") == 0)
    {
      list_treasures(argv[2]);
    }
  else if(strcmp(argv[1], "--view") == 0 && argc == 4)
    {
      if(argc == 4)
	view_details(argv[2], argv[3]);
      else
	{
	  printf("For this you need argv[4].\n");
	  exit(-1);
	}
    }
  
  return 0;
}
