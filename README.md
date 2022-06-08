# Linux_Commands
More and ls Commands code written in C

#More:
      A version of linux "more" command. It takes one or more file names as command line argument. Display the file page by page according to size of terminal. On pressing "q" the file will quit. On enter next line will be displayed. On space next page will be displayed. On '\' search can be done and on pressing "v" file will open in vim. The commands are taken as input in non-echo and non-canonical mode. The size of terminal can be changed even while the program is running.
Command to run: gcc more.c
                ./a.out <space separated file names>
