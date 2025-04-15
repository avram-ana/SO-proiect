void remove_dir(const char *hunt_name)
{
  DIR *dir;
  struct dirent *entry;
  char full_path[200];

  // does the directory exist?
  struct stat st;
  if(stat(hunt_name, &st) == -1 || !S_ISDIR(st.st_mode))  // S_ISDIR - checks whether it's a dir or not
    {
      const char *msg = "Error: Hunt directory doesn't exist.\n";
      write(STDOUT_FILENO, msg, strlen(msg));
      return;
    }

  if(!(dir = opendir(hunt_name)))
    {
      perror("Error: Could not open hunt directory");
      exit(-1);
    }

  // create path for symlink:
  char log_symlink[200];
  strcpy(log_symlink, "logged_hunt-");
  strcat(log_symlink, hunt_name);

  // remove symbolic link:
  if(lstat(log_symlink, &st) == 0 && S_ISLNK(st.st_mode))
    {
      if(unlink(log_symlink) != 0)  // unlink "unlinks" the dir before deleting (remove symlink)
        {
	  perror("Error removing symbolic link");
	  exit(-1);
        }
    }
  
  // remove the content from given dir
  while((entry = readdir(dir)) != NULL)
    {
      if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;

      strcpy(full_path, hunt_name);
      strcat(full_path, "/");
      strcat(full_path, entry->d_name);

      if(remove(full_path) != 0)
        {
          printf("Couldn't remove a file.\n");
	  exit(-1);
        }
    }

  closedir(dir);

  // remove the given dir
  if(remove(hunt_name) != 0)
    {
      printf("Error removing given dir.\n");
      exit(-1);
    }
}
