#include <stdio.h>
#include<unistd.h>
#include <dirent.h>
#include <string.h>
#include<sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include<pwd.h>
#include<grp.h>
#include<sys/ioctl.h>
#include<math.h>

int COLS;

extern int errno;

void display(char *);
void display_l(char *);
void display_R(char *);
void show_stat_info(char *,char *);
void update_windows_dimension();
int cmpfunc (const void * a, const void * b);
int isdir(char *name);

int main(int argc, char * argv[]){
	update_windows_dimension();
	if(argc == 1){
		display(".");
		return 0;
	}
	else if(strcmp(argv[1], "-l") == 0){
		int i = 1;
		while(++i < argc){
			printf("contents of %s:\n", argv[i]);
			display_l(argv[i]);
			printf("\n");
		}
	}
	else if(strcmp(argv[1], "-R") == 0){
		int i = 1;
		while(++i < argc){
			printf("contents of %s:\n", argv[i]);
			display_R(argv[i]);
			printf("\n");
		}
	}
	else{
		int i = 0;
		while(++i < argc){
			printf("contents of %s:\n", argv[i]);
			display(argv[i]);
			printf("\n");
		}
	}

	return 0;
}

void show_stat_info(char *fname,char * filename){
	struct stat info;
	int rv = lstat(fname, &info);
	if(rv == -1){
		perror("stat failed");
		exit(1);

	}

	//file type
	if ((info.st_mode &  0170000) == 0010000) 
		printf("p");
   	else if ((info.st_mode &  0170000) == 0020000) 
		printf("c");
   	else if ((info.st_mode &  0170000) == 0040000) 
		printf("d");
   	else if ((info.st_mode &  0170000) == 0060000) 
		printf("b");
   	else if ((info.st_mode &  0170000) == 0100000) 
		printf("-");
   	else if ((info.st_mode &  0170000) == 0120000) 
		printf("l");
   	else if ((info.st_mode &  0170000) == 0140000) 
		printf("s");

	//handling permissions
	int mode = info.st_mode;
	char str[10];
	strcpy(str, "---------");
	//owner  permissions
   	if((mode & 0000400) == 0000400) str[0] = 'r';
   	if((mode & 0000200) == 0000200) str[1] = 'w';
   	if((mode & 0000100) == 0000100) str[2] = 'x';
	//group permissions
   	if((mode & 0000040) == 0000040) str[3] = 'r';
   	if((mode & 0000020) == 0000020) str[4] = 'w';
   	if((mode & 0000010) == 0000010) str[5] = 'x';
	//others  permissions
   	if((mode & 0000004) == 0000004) str[6] = 'r';
   	if((mode & 0000002) == 0000002) str[7] = 'w';
   	if((mode & 0000001) == 0000001) str[8] = 'x';
	//special  permissions
   	if((mode & 0004000) == 0004000) str[2] = 's';
   	if((mode & 0002000) == 0002000) str[5] = 's';
   	if((mode & 0001000) == 0001000) str[8] = 't';
   	printf("%s", str);

   	//link count
   	printf(" %ld", info.st_nlink);

   	//user id to user name
   	struct passwd * pwd = getpwuid(info.st_uid);
  	errno = 0;
   	if (pwd == NULL){
      	if (errno == 0)
        	printf("Record not found in passwd file.\n");
      	else
        	perror("getpwuid failed");
   	}
   	else
       printf(" %s",pwd->pw_name);   

   	//group id to group name
   	struct group *grp = getgrgid(info.st_gid);
   	errno = 0;
   	if (grp == NULL){
      	if (errno == 0)
        	printf("Record not found in /etc/group file.\n");
      	else
        	perror("getgrgid failed");
   	}
   	else
      	printf(" %s", grp->gr_name); 

	//size
   	printf(" %ld", info.st_size);
   	//time in proper format
   	long secs = info.st_mtime;
   	char *no_new_line_time;
   	no_new_line_time = strtok(ctime(&secs), "\n");
   	printf(" %s", no_new_line_time);
   	//name
   	printf(" %s", filename);


}

void display_l(char * dir){
	struct stat info;
	int rv = lstat(dir, &info);
	if (rv == -1){
	      perror("stat failed");
	      exit(1);
	}
	if ((info.st_mode &  0170000) == 0040000) {
	struct dirent *entry;
	DIR * dp = opendir(dir);
	if(dp == NULL){
		fprintf(stderr, "can not open %s\n", dir);
		return;
	}
	errno = 0;
	while((entry = readdir(dp)) != NULL){
		if(entry == NULL & errno != 0){
			perror("readdir failed\n");
			exit(errno);
		}
		else{
			if(entry->d_name[0] == '.')
				continue;
			char buffer[1024];
			strcpy(buffer, dir);
			strcat(buffer, "//");
			strcat(buffer, entry->d_name);		
			show_stat_info(buffer,entry->d_name);
			printf("\n");
		}
	}
	closedir(dp);
	}
	else if ((info.st_mode &  0170000) == 0100000) 
		show_stat_info(dir,dir);
}

void display(char * dir){
	struct stat info;

	char* names[1000] = {};
	int idx = 0;
  	int max_name_length = -1;

	int rv = lstat(dir, &info);
	if (rv == -1){
	      perror("stat failed");
	      exit(1);
	}
	if ((info.st_mode &  0170000) == 0040000) {
		

		struct dirent *entry;
		DIR * dp = opendir(dir);
		if(dp == NULL){
			fprintf(stderr, "can not open %s\n", dir);
			return;
		}
		errno = 0;
		while((entry = readdir(dp)) != NULL){
			if(entry == NULL & errno != 0){
				perror("readdir failed\n");
				exit(errno);
			}
			else{
				if(entry->d_name[0] == '.')
					continue;
				else{
					names[idx++] = entry->d_name;
					if ((int)strlen(entry->d_name) > max_name_length)
          				max_name_length = (int)strlen(entry->d_name);
				}
			}
		}
		closedir(dp);
	}
	else if ((info.st_mode &  0170000) == 0100000) 
		printf("%s",dir);

	qsort(names, idx, sizeof(names[0]), cmpfunc);

	int cols = COLS / (max_name_length + 2);
  	int offset = ceil((double)idx / cols);
  	for (int i = 0; i < offset; i++) {
      for (int j = i; j < idx; j += offset) {
      		//printf("%s",names[j]);
      		printf("%-*s", max_name_length + 2, names[j]);

      }
      puts("");
    }
}



void display_R(char * dir){


	char* names[1000] = {};
	char* dirs_array[1000] = {};
	int idx = 0;
  int max_name_length = -1;
  int dir_count = 0;

	struct dirent *entry;
	DIR * dp = opendir(dir);
	if(dp == NULL){
		fprintf(stderr, "can not open %s\n", dir);
		return;
	}
	errno = 0;
	while((entry = readdir(dp)) != NULL){
		if(entry == NULL & errno != 0){
			perror("readdir failed\n");
			exit(errno);
		}
		else{
			if(entry->d_name[0] == '.')
				continue;
			else{
				names[idx++] = entry->d_name;
				if ((int)strlen(entry->d_name) > max_name_length)
          	max_name_length = (int)strlen(entry->d_name);
        char buffer[1024];
				strcpy(buffer, dir);
				strcat(buffer, "//");
				strcat(buffer, entry->d_name);
        if((isdir(buffer)) == 0){
        	dirs_array[dir_count++] = entry->d_name;
        }
			}
		}
	}
	closedir(dp);

	qsort(names, idx, sizeof(names[0]), cmpfunc);

	int cols = COLS / (max_name_length + 2);
  	int offset = ceil((double)idx / cols);
  	for (int i = 0; i < offset; i++) {
      for (int j = i; j < idx; j += offset) {
      		//printf("%s",names[j]);
      		printf("%-*s", max_name_length + 2, names[j]);

      }
      puts("");
    }
  for (int i = 0; i < dir_count; i++)
    {
      puts("-----------------------------");
      printf("Directory listing of %s:\n", dirs_array[i]);
      //puts("-----------------------------");
      char buffer[1024];
			strcpy(buffer, dir);
			strcat(buffer, "//");
			strcat(buffer, dirs_array[i]);
      display(buffer);
    }
}

void update_windows_dimension()
{
  struct winsize ws;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  COLS = ws.ws_col;
}

int cmpfunc (const void * a, const void * b) {
  const char **ia = (const char **)a;
  const char **ib = (const char **)b;
  return strcmp(*ia, *ib);
}

int isdir(char *name){
	struct stat info;
	int rv = lstat(name, &info);
	if(rv == -1){
		perror("stat failed");
		exit(1);
	}
	if ((info.st_mode &  0170000) == 0040000)
		return 0;
	return 1;
}

