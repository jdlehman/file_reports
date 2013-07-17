#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char *argv[], char *envp[])
{

	if(argc > 1){
		fprintf(stderr, "\ntotalsize: There should be no arguments for this function.\n");
		exit(1);
	}

	//get environment variables
	char* delay = getenv("TDELAY");
	char* parent = getenv("TPARENT");

	int delayNum = -1;
	int parentNum = -1;

	char* invalChar;
	long arg;

	//check that delay has a value and is a valid integer
	if(delay != NULL)
	{
		//check for overflow of delay
		if((arg = strtol(delay, &invalChar, 10)) >= INT_MAX)
		{
			fprintf(stderr, "\nreport: Overflow. Invalid environment variable TDELAY.\n");
			exit(1);
		}

		//check for first argument is a valid nonzero positive integer and check underflow
		if(!(arg > 0) || (*invalChar))
		{
			fprintf(stderr, "\nreport: Invalid environment variable TDELAY.\n");
			exit(1);
		}
		else
		{
			delayNum = arg;
		}
	}

	//check that parent has a value and is a valid integer
	if(parent != NULL)
	{
		//reset
		invalChar = NULL;
		arg = -1;
		//check for overflow of delay
		if((arg = strtol(parent, &invalChar, 10)) >= INT_MAX)
		{
			fprintf(stderr, "\nreport: Overflow. Invalid environment variable TPARENT.\n");
			exit(1);
		}

		//check for first argument is a valid nonzero positive integer and check underflow
		if(!(arg > 0) || (*invalChar))
		{
			fprintf(stderr, "\nreport: Invalid environment variable TPARENT.\n");
			exit(1);
		}
		else
		{
			parentNum = arg;
		}
	}

  //using adapted simple linked list program provided by Kearns' class notes
	//int LINE_MAX = 2048;
  	char tmpString[LINE_MAX];

  	struct snode
  	{
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
  		fprintf(stderr, "totalsize: stdin error because:\n");
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

  			//sleep for TDELAY seconds if environment TDELAY variable is set before inputting a file name
  			if(delayNum > 0)
  			{
  				sleep(delayNum);
  			}

  			if((newNode = (struct snode *)malloc(sizeof(struct snode))) == NULL)
			{
				fprintf(stderr, "malloc failure for newNode\n");
				exit(1);
			}

			if (stat(filePtr, &newNode->info) != 0) {
				//fprintf(stderr,"totalsize: cannot get stat info for the file, %s, because:\n", filePtr);
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


  	int size = 0;

  	if(listHead == NULL){
//  		fprintf(stderr, "\ntotalsize: Error, there were no valid file names in standard input, calculations could not be made.\n");
//  		exit(1);
  		fprintf(stdout, "%d", size);
  	}
  	else
  	{
  		for(ptr = listHead; ptr != NULL; ptr = ptr->next){
			size = size + ((int)ptr->info.st_size);
			//for testing
			//printf("(device,i_number)=(%d/%d,%ld)", major(ptr->info.st_dev), minor(ptr->info.st_dev), (long)ptr->info.st_ino);
			//printf("\n");
		}

		//check if want in kilobytes
		char* units = getenv("UNITS");

		if(units != NULL){

			if(strcmp(units, "k") == 0 || strcmp(units, "K") == 0){
				//print output in kilobytes, formatted as an integer
				fprintf(stdout, "%d", size / 1024);
			}
			else{
				//print output in bytes, formatted as an integer
				fprintf(stdout, "%d", size);
			}
		}
		else{
			//print output in bytes, formatted as an integer
			fprintf(stdout, "%d", size);
		}
  	}

	if(parentNum > 0)
	{
		 kill(parentNum, SIGUSR1);
	}
	else
	{
		fprintf(stdout, "\n");
	}

  exit(0);
}

