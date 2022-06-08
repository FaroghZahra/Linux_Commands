#include <stdio.h>
#include <stdlib.h>
#include<termios.h>
#include<sys/ioctl.h>
#include<string.h>
#include<sys/wait.h>
#include <unistd.h>

#define LINELEN 512
int PAGELEN;

void do_more(FILE *f, int t, int linespast, char * arg);
int takeinput();
void calculate_terminal_size();

int main(int argc, char * argv[]){
	if(argc == 1){  		//if no file name given
		printf("--help");
		exit(0);
	}
	FILE *f;
	int i = 0;

	char buff [LINELEN];
	int total_lines = 0;

	while(++i < argc){			//reading one or multiple files
		f = fopen(argv[i], "r");
		if(f == NULL){
			printf("file not exits");
			exit(1);
		}
		else{
			while(fgets(buff,LINELEN,f))
				total_lines++;
			fseek(f,0,SEEK_SET);
			if(argc > 2){
				printf(":::::::::::::::::::\n");
				printf("%s\n",argv[i]);
				printf(":::::::::::::::::::\n");
			}
			do_more(f,total_lines,0, argv[i]);

		}
	}
	fclose(f);
	return 0;
}


void do_more(FILE *f, int total_num_lines, int lines_past_already, char * argv){
	calculate_terminal_size();
	char buff[LINELEN];
	int lines = 0;
	int var;
	int percent;
	while(fgets(buff,LINELEN,f)){
		fputs(buff,stdout);
		lines++;
		lines_past_already++;
		if(lines == PAGELEN){
			percent = (lines_past_already/(float)total_num_lines)*100;
			printf("\033[7m--more--(%d%%)\033[m",percent);
			var = takeinput();
			if(var == 1){			// to quit
				printf("\033[1A\033[2K\033[1G");
				break;
			}
			if(var == 2){		//when pressed enter
				printf("\033[1A\033[2K\033[1G");
				lines = lines - 1;
				calculate_terminal_size();
			}
			if(var == 3){		//when pressed space bar
				printf("\033[1A\033[2K\033[1G");
				lines = PAGELEN - lines;
				calculate_terminal_size();
			}
			if(var == 4){			//to search
				printf("\033[1A\033[2K\033[1G");
				char str[LINELEN];
				scanf("%s",str);
				int length = strlen(str);
				str[length- 1] = '\0';
				while(fgets(buff,LINELEN,f)){
					lines_past_already++;
					if(strstr(buff,str)){
						printf("\033[2A \033[2K \033[1G...........Skipping .......\n");
						printf("%s",buff);
						lines = 2;
						break;
					}
				}
			}
			if(var == 5){			//to open file in vim
				printf("\033[1A\033[2K\033[1G");
				int position = ftell(f);
				int cpid = fork();
				if(cpid == 0){
					execl("/usr/bin/vim","/usr/bin/vim",argv,NULL);
				}
				else{
					wait(NULL);
				}
				f = fopen(argv,"r");
				fseek(f,position,SEEK_SET);
				do_more(f,total_num_lines,lines_past_already,argv);
			}
			if(var == 6){
				break;
			}
		}
	}
}

int takeinput(){
	struct termios info;
	ioctl(0, TCGETS, &info);
	struct termios save = info;
	info.c_lflag &= ~(ECHO|ICANON);
	ioctl(0,TCSETS,&info);
	int c = getchar();
	printf("\n");
	ioctl(0,TCSETS,&save);
	if(c == 'q')		
		return 1;
	else if(c == '\n')
		return 2;
	else if (c == ' ')
		return 3;
	else if(c == '/')
		return 4;
	else if(c == 'v')
		return 5;
	else
		return 6;
	return 0;
}

void calculate_terminal_size(){
	struct winsize info;
	if(ioctl(0,TIOCGWINSZ,&info) == -1){
		printf("error in ioctl");
		exit(1);
	}
	PAGELEN = info.ws_row;
}
