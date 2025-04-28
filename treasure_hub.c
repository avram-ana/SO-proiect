#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

pid_t monitor_pid = -1;
int monitor_stopped = 1;  // 1 - monitor is stopped | 0 - monitor running

void print_details(void)
{
  printf("\n___TREASURE HUB COMMANDS___\n\n");
  printf("- start_monitor  // starts a separate background process\n");
  printf("- list_hunts  // asks the monitor to list the hunts and the total number of treasures in each\n");  // not implemented yet!!!!!!!!!!!!!!!!!!!!!!
  printf("- list_treasures  // tells the monitor to show the information about all treasures in a hunt\n");
  printf("- view_treasure  // tells the monitor to show the information about a treasure in hunt\n");
  printf("- stop_monitor  // asks the monitor to end then returns to the prompt\n");
  printf("- exit  // if the monitor still runs, prints an error message, otherwise ends the program\n\n");
}

void sigchild_handler(int sig)
{
  // handles the end of the monitor process
  // (or child process) and updates the status.
  (void)sig;
  int status;
  pid_t pid;
  
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
      if (pid == monitor_pid)
        {
	  printf("[hub] monitor terminated with status %d\n", WEXITSTATUS(status));
	  // change status to "stopped monitor":
	  monitor_pid = -1;
	  monitor_stopped = 1;
        }
    }
}

void start_monitor(void)
{
  // if the monitor is already running we stop:
  if (monitor_pid != -1)
    {
      printf("[hub] Monitor is already running with PID %d.\n", monitor_pid);
      return;
    }

  // else, actually start the monitor:
  //create a child process
  pid_t pid = fork();
  if(pid == -1)
    {
      perror("Error with the fork");
      exit(-1);
    }
  else if(pid == 0)  // we are inside the child process:
    {
      while(1)
        {
	  pause();  // the process waits for signals
        }
    }
  else  // we are inside the parent process:
    {
      monitor_pid = pid;
      // set status to "monitor running":
      monitor_stopped = 0;
      printf("[hub] Monitor started with PID %d.\n", pid);
    }
}

void stop_monitor(void)
{
  // can't stop a "stopped" monitor
  if (monitor_pid == -1 || monitor_stopped == 1)
    {
      printf("[hub] No monitor running.\n");
      return;
    }
  // send  kill signal to stop monitor:
  kill(monitor_pid, SIGKILL);

  // change status:
  monitor_stopped = 1;
  monitor_pid = -1;
  printf("[hub] Monitor stopped.\n");
}

void execute_list_treasures(void)
{
  char hunt_id[20];
  printf("[hub] Enter the ID for the desired hunt: ");
  if(!fgets(hunt_id, sizeof(hunt_id), stdin))
    {
      perror("Error reading hunt_id for list_treasures function");
      exit(-1);
    }
  hunt_id[strcspn(hunt_id, "\n")] = 0;  // Remove the newline character
  
  char command[300];
  // treasure_manager.c requires strict args: --list for this function and the ID for the desired hunt:
  snprintf(command, sizeof(command), "./treasure_manager --list %s", hunt_id);

  // system - if a child process could not be created, or its status could not be retrieved,
  // the return value is -1 and errno is set to indicate the error.
  if (system(command) == -1)
    {
      perror("Error executing treasure_manager");
      exit(-1);
    }
}

void execute_view_treasure()
{
  char hunt_id[20];
  printf("[hub] Enter the ID for the desired hunt: ");
  if(!fgets(hunt_id, sizeof(hunt_id), stdin))
    {
      perror("Error reading hunt_id for view_treasure function");
      exit(-1);
    }
  hunt_id[strcspn(hunt_id, "\n")] = 0;  // Remove the newline character

  // why did I choose char for ID? because it's defined the same way in "treasure_manager.c". easier to work with
  char id[50];
  printf("[hub] Enter treasure ID: ");
  if(!fgets(id, sizeof(id), stdin))
    {
      perror("Error reading ID for view_treasure function");
      exit(-1);
    }

  char command[300];
  // -- view requires 4 args: exe, "--view", hunt, treasure_ID
  snprintf(command, sizeof(command), "./treasure_manager --view %s %s", hunt_id, id);

  if(system(command) == -1)
    {
      perror("Error executing treasure_manager");
      exit(-1);
    }
}

void process_command(const char *input)
{
  if(strcmp(input, "list_treasures") == 0)
    {
      if(monitor_pid != -1 && monitor_stopped == 0)
        {
	  execute_list_treasures();
        }
      else
        {
	  printf("[hub] cannot execute command, no monitor running.\n");
        }
    }
  else if(strcmp(input, "view_treasure") == 0)
    {
      if(monitor_pid != -1 && monitor_stopped == 0)
        {
	  execute_view_treasure();
        }
      else
        {
	  printf("[hub] cannot execute command, no monitor running.\n");
        }
    }
  else if(strcmp(input, "start_monitor") == 0)
    {
      start_monitor();
    }
  else if(strcmp(input, "stop_monitor") == 0)
    {
      stop_monitor();
    }
  else if(strcmp(input, "exit") == 0)
    {
      printf("[hub] Exiting...\n");
      exit(0);
    }
  else
    {
      printf("[hub] Unknown command!\n");
    }
}

int main(void)
{
  print_details();
  
  struct sigaction sa;
  sa.sa_handler = sigchild_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  
  if(sigaction(SIGCHLD, &sa, NULL) == -1)
    {
      perror("Error with sigaction");
      exit(-1);
    }
  
  char input[200];
  
  while(1)
    {
      printf("[hub] > ");
      fflush(stdout);
      
      if(fgets(input, sizeof(input), stdin) == NULL)
        {
	  printf("\n");
	  break;
        }
      
      input[strcspn(input, "\n")] = 0; // Remove newline character
      
      // Process command:
      process_command(input);
    }
  
  return 0;
}
