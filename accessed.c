#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{

	int days = 0;

	if(argc != 3){
		fprintf(stderr, "\naccessed: There should be two arguments for this function, '-a' and a non-zero integer.\n");
		exit(1);
	}

	//check that first argument is -a
	if(abs(strcmp(argv[1], "-a")))
	{
		fprintf(stderr, "\naccessed: Invalid first argument, '%s'. The first argument must be '-a'.\n", argv[1]);
		exit(1);
	}

	char *invalChar;
	long arg = strtol(argv[2], &invalChar, 10);

	//check that the second error is a valid integer
	if((arg == 0) || (*invalChar))
	{
		fprintf(stderr, "\naccessed: Invalid second argument, '%s'.  The second argument should be a valid non-zero integer.\n", argv[2]);
		exit(1);
	}
	else{
		days = arg;
	}

	int isPositive = 0;

	if(days > 0){
		isPositive = 1;
	}


  //using adapted simple linked list program provided by Kearns' class notes
	int LINE_MAX = 2048;
  	char tmpString[LINE_MAX];

  	struct snode
  	{
  		char* fname;
  		struct stat info;
  		struct snode *next;
  	};

  	//stores size of list
  	int listSize = 0;
  	//boolean if already added file to list
  	int fileUsed = 0;

  	struct snode *listHead = NULL;
  	struct snode *listTail = NULL;
  	struct snode *newNode, *ptr;

  	char* delimiters = "\t\n\v\f\r ";

  	struct stat stats;

  	if(fstat(0, &stats) == -1)
	{
		fprintf(stderr, "accessed: stdin error because:\n");
		perror(NULL);
		exit(1);
	}

	int validInput = -1;

	//example from fstat page in unix manual
	if (S_ISCHR(stats.st_mode))//incorrect
	{
		// Looks like a tty, so we're in interactive mode.
		validInput = 0;
	} else if (S_ISFIFO(stats.st_mode))//correct
	{
		// Looks like a pipe, so we're in non-interactive mode.
		validInput = 1;
	}

  	//add info for each argument to linked list
  	while(validInput && (fgets(tmpString, LINE_MAX, stdin) == tmpString))
  	{
  		char* filePtr = strtok(tmpString, delimiters);

  		while(filePtr != NULL){

  			if((newNode = (struct snode *)malloc(sizeof(struct snode))) == NULL)
			{
				fprintf(stderr, "malloc failure for newNode\n");
				exit(1);
			}

  			if((newNode->fname = malloc(strlen(filePtr) + 1)) == NULL)
			{
				fprintf(stderr, "malloc failure for newNode->fname\n");
				exit(1);
			}

  			if(strncpy(newNode->fname, filePtr, strlen(filePtr) + 1) != newNode->fname)
			{
				fprintf(stderr, "string copy problem\n");
				exit(1);
			}

			if (stat(filePtr, &newNode->info) != 0) {
				//fprintf(stderr,"accessed: cannot get stat info for the file, %s, because:\n", filePtr);
				//perror(NULL);
			}
			else
			{
				//check if file is a regular file (only add regular files to list)
				if(S_ISREG(newNode->info.st_mode)){
					//check if file reference is already part of list
						//iterate through list
					for(ptr = listHead; ptr != NULL; ptr = ptr->next){
						if((major(ptr->info.st_dev) == major(newNode->info.st_dev)) && (minor(ptr->info.st_dev) == minor(newNode->info.st_dev)) && (((long)ptr->info.st_ino) == ((long)newNode->info.st_ino))){
							fileUsed = 1;
						}
					}

					//add stat info for file to linked list if it has not already been added
					if(!fileUsed){
						newNode->next = NULL;

						//first insertion is a special case, otherwise append to end of list
						if(listTail == NULL)
						{
							listHead = listTail = newNode;
							listSize = listSize + 1;
						}
						else
						{
							listTail->next = newNode;
							listTail = newNode;
							listSize = listSize + 1;
						}
					}
					else{
						//reset boolean
						fileUsed = 0;
					}
				}
			}//end else

			//increment token
  			filePtr = strtok(NULL, delimiters);
		}
  	}// end while

//  if(listHead == NULL){
//		fprintf(stderr, "\naccessed: Error, there were no valid file names in standard input, calculations could not be made.\n");
//		exit(1);
//	}


  	for(ptr = listHead; ptr != NULL; ptr = ptr->next){

  		//if argument 2 positive, print files which have not been accessed for num days
  		if(isPositive){
  			if(((time(NULL) - ptr->info.st_atime) / 60 / 60 / 24) > days){
  				fprintf(stdout, "%s\n", ptr->fname);
  			}
  		}
  		else{//argument 2 negative, print files which have been accessed within num days
  			if(((time(NULL) - ptr->info.st_atime) / 60 / 60 / 24) <= (days * (-1))){
				fprintf(stdout, "%s\n", ptr->fname);
			}
  		}
  	}

  exit(0);
}

