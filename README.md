# Linux_Commands
More and ls Commands code written in C

# More:
      A version of linux "more" command. It takes one or more file names as command line argument. Display the file page by page according to size of terminal. On pressing "q" the file will quit. On enter next line will be displayed. On space next page will be displayed. On '\' search can be done and on pressing "v" file will open in vim. The commands are taken as input in non-echo and non-canonical mode. The size of terminal can be changed even while the program is running.
Command to run: gcc more.c
               ./a.out <space separated file names>
      
      
# ls:
      A version of linux ls command. If not given name of any directory it displays the contents of current working directory. If given the name of directory with "-l" option it displays the long listing of files. if given the name with "-R" option it shows contents of sub-directories too. The program also perform qsort3 and displays file names in sorted order. It also displays names depending on the size of terminal
Command to compile: gcc ls_command.c -lm
     
