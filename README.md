SO - Sisteme de Operare

                Phase One - SO_proiect.c
- add_hunt (... or add a treasure in a hunt that already exists)
- - list_treasures (if there are any in the given hunt) 
- view_a_certain_treasure (search for a specific trasure and print it - if it exists)
- remove_a_certain_treasure (remove the treasure with a certain ID - if it exists)
- remove_hunt (remove everythig there is)
              
              Phase Two - treasure_hub.c
- start_monitor: starts a separate background process that monitors the hunts and prints to the standard output information about them when asked to
- list_hunts: asks the monitor to list the hunts and the total number of treasures in each
- list_treasures: tells the monitor to show the information about all treasures in a hunt, the same way as the command line at the previous stage did
- view_treasure: tells the monitor to show the information about a treasure in hunt, the same way as the command line at the previous stage did
- stop_monitor: asks the monitor to end then returns to the prompt. Prints monitor's  termination state when it ends.
- exit: if the monitor still runs, prints an error message, otherwise ends the program
