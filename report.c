#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

void usr1handler();

int flag = 1;

void usr1handler()
{
	flag = 0;
}

int main(int argc, char *argv[])
{
	//attach handler to method to handle signal
	signal(SIGUSR1, usr1handler);

	int useKilo = 0;
	int useD = 0;
	char* dVal = NULL;
	int numDays;

	//check that there are at least two arguments and no more than 4 arguments not including file location
	if(argc < 2 || argc > 5)
	{
		fprintf(stderr, "\nreport: Invalid arguments for report. The correct invocation is:\nreport [integer] {-k} {-d integer}\n");
		exit(1);
	}

	char* invalChar;
	long arg;

	//check for overflow of first argument
	if((arg = strtol(argv[1], &invalChar, 10)) >= INT_MAX)
	{
		fprintf(stderr, "\nreport: Overflow. Invalid first argument for report, '%s'. The first argument must be a valid non-zero positive integer less than %d.\n", argv[1], INT_MAX);
		exit(1);
	}

	//check for first argument is a valid nonzero positive integer and check underflow
	if(!(arg > 0) || (*invalChar))
	{
		fprintf(stderr, "\nreport: Invalid first argument for report, '%s'.  The first argument must be a valid non-zero positive integer.\n", argv[1]);
		exit(1);
	}
	else
	{
		numDays = arg;
	}

	//check optional arguments

	if(argc > 2)
	{
		if((abs(strcmp(argv[2], "-d")) && abs(strcmp(argv[2], "-k"))))
		{
			fprintf(stderr, "\nreport: Invalid second argument for report, '%s'. The second argument must be '-d' or '-k'.\n", argv[2]);
			exit(1);
		}
		else
		{
			//set to word mode if first argument is w
			if(abs(strcmp(argv[2], "-k")) == 0)
			{
				//arg = -k
				useKilo = 1;
			}
			else
			{
				//arg = -d
				useD = 1;
			}
		}
	}

	//check if there is a third argument
	if(argc > 3)
	{
		if(useD)//last argument was -d, next must be integer
		{
			//reset
			arg = -1;
			invalChar = NULL;

			//check for overflow of third argument
			if((arg = strtol(argv[3], &invalChar, 10)) >= INT_MAX)
			{
				fprintf(stderr, "\nreport: Overflow. Invalid third argument for report, '%s'. The third argument must be a valid non-zero positive integer less than %d because the preceding argument was '-d'.\n", argv[3], INT_MAX);
				exit(1);
			}

			//check that third argument is a valid nonzero positive integer and check underflow
			if(!(arg > 0) || (*invalChar))
			{
				fprintf(stderr, "\nreport: Invalid third argument for report, '%s'.  The third argument must be a valid non-zero positive integer because the preceding argument was '-d'.\n", argv[3]);
				exit(1);
			}
			else
			{
				dVal = argv[3];
			}
		}
		else//last argument was -k, next argument must be -d or it is invalid
		{
			if(abs(strcmp(argv[3], "-d")))
			{
				fprintf(stderr, "\nreport: Invalid third argument for report, '%s'. The third argument must be '-d'.\n", argv[3]);
				exit(1);
			}
			else
			{
				useD = 1;
			}
		}
	}
	else
	{
		if(useD)
		{
			fprintf(stderr, "\nreport: There must be a third argument (a non-zero positive integer) for report since the second argument was '-d'.\n");
			exit(1);
		}
	}

	//check if there is a fourth argument
	if(argc > 4)
	{
		if(useKilo && useD)//last argument was -d, argument must be a valid non-zero positive integer
		{
			//reset
			arg = -1;
			invalChar = NULL;

			//check for overflow of third argument
			if((arg = strtol(argv[4], &invalChar, 10)) >= INT_MAX)
			{
				fprintf(stderr, "\nreport: Overflow. Invalid fourth argument for report, '%s'. The fourth argument must be a valid non-zero positive integer less than %d because the preceding argument was '-d'.\n", argv[4], INT_MAX);
				exit(1);
			}

			//check that third argument is a valid nonzero positive integer and check underflow
			if(!(arg > 0) || (*invalChar))
			{
				fprintf(stderr, "\nreport: Invalid fourth argument for report, '%s'.  The fourth argument must be a valid non-zero positive integer because the preceding argument was '-d'.\n", argv[4]);
				exit(1);
			}
			else
			{
				dVal = argv[4];
			}
		}
		else//this argument must be -k
		{
			if(abs(strcmp(argv[4], "-k")) != 0)
			{
				fprintf(stderr, "\nreport: Invalid fourth argument for report, '%s'. The fourth argument must be '-k'.\n", argv[4]);
				exit(1);
			}
			else
			{
				useKilo = 1;
			}
		}
	}
	else
	{
		if(useD && useKilo)
		{
			fprintf(stderr, "\nreport: There must be a fourth argument (a non-zero positive integer) for report since the third argument was '-d'.\n");
			exit(1);
		}
	}

	char* enviro[4];
	int envCtr = 0;
	char* accArgs[4];
	accArgs[0] = "fileNamePlaceHolder";
	accArgs[1] = "-a";
	sprintf(accArgs[2]=malloc(sizeof(numDays)), "%d", numDays);
	//accArgs[2] = itoa(numDays, 10);
	accArgs[3] = NULL;

	char* accArgs2[4];
	accArgs2[0] = "fileNamePlaceHolder";
	accArgs2[1] = "-a";
	sprintf(accArgs2[2]=malloc(sizeof(numDays*-1)), "%d", (numDays * -1));
	//accArgs2[2] = itoa((numDays * -1), 10);
	accArgs2[3] = NULL;

	if(useKilo)
	{
		//set environment variable, UNITS to kilobytes
		enviro[envCtr] = "UNITS=k";
		envCtr += 1;
	}

	char *tDel = NULL;
	if(useD)
	{
		tDel = malloc(sizeof(char) * (strlen("TDELAY=") + strlen(dVal) + 1));
		//set environment variable, TDELAY

		tDel[strlen("TDELAY=") + strlen(dVal)] = '\0';
		strcat(tDel, "TDELAY=");
		strcat(tDel, dVal);

		enviro[envCtr] = tDel;
		envCtr += 1;
	}

	char* pidString;
	//convert to string
	sprintf(pidString=malloc(sizeof(getpid())), "%d", getpid());

	char *tPar = malloc(sizeof(char) * (strlen("TPARENT=") + strlen(pidString) + 1));
	tPar[strlen("TPARENT=") + strlen(pidString)] = '\0';
	strcat(tPar, "TPARENT=");

	strcat(tPar, pidString);

	//add to environment variable array
	enviro[envCtr] = tPar;
	envCtr += 1;

	enviro[envCtr] = NULL;

	struct stat stats;

	if(fstat(0, &stats) == -1)
	{
		fprintf(stderr, "report: stdin error because:\n");
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

	char currLine[LINE_MAX];
	char stdinString[LINE_MAX * LINE_MAX];
	while(validInput && (fgets(currLine, LINE_MAX, stdin) == currLine))
	{
		strcat(stdinString, currLine);
	}

	pid_t pid;
	int r2a1[2], a2t1[2], t2r1[2], r2a2[2], a2t2[2], t2r2[2];
	pipe(r2a1); pipe(a2t1); pipe(t2r1); pipe(r2a2); pipe(a2t2); pipe(t2r2);

	//report to accessed1
	if((pid = fork()) < 0)
	{
		fprintf(stderr, "Creation of child process one failed");
		perror(NULL);
		exit(1);
	}
	else if(pid == 0)//child
	{
		close(0); dup(r2a1[0]); close(r2a1[0]); //move read of pipe r2a1 to stdin
		close(r2a1[1]); //close write to pipe r2a1

		close(1); dup(a2t1[1]); close(a2t1[1]); //move write of pipe a2t1 to stdout
		close(a2t1[0]);

		execve("accessed", accArgs, NULL);//run accessed in child
		fprintf(stderr, "Error running child, accessed.c\n");
		perror(NULL);
		exit(1);
	}
	else//parent
	{
		close(r2a1[0]); //close read from pipe r2a1

		//write report's stdin to pipe r2a1's write
		if((write(r2a1[1], stdinString, strlen(stdinString))) != strlen(stdinString))
		{
			fprintf(stderr, "\nreport: Error when parent writing to pipe r2a1");
		}

		close(r2a1[1]); //close write to pipe r2a1

		wait(NULL);
	}

	//accessed1 to totalsize1
	if((pid = fork()) < 0)
	{
		fprintf(stderr, "Creation of child process two failed");
		perror(NULL);
		exit(1);
	}
	else if(pid == 0)//child
	{
		close(0); dup(a2t1[0]); close(a2t1[0]); //move read of pipe a2t1 to stdin
		close(a2t1[1]); //close write to pipe a2t1

		close(1); dup(t2r1[1]); close(t2r1[1]); //move write of pipe t2r1 to stdout
		close(t2r1[0]);

		execve("totalsize", NULL, enviro);
		fprintf(stderr, "Error running child, totalsize.c\n");
		perror(NULL);
		exit(1);
	}
	else//parent
	{
		close(0); dup(t2r1[0]); close(t2r1[0]); //move read of pipe t2r1 to stdin
		close(t2r1[1]); //close write to pipe t2r1

		close(a2t1[1]); close(a2t1[0]);//close write and read to pipe a2t1
		while(flag)
		{
			sleep(1);
			printf("*");
		}
		flag = 1;
		wait(NULL);
	}

	//get first value from pipe
	char value1[LINE_MAX];
	fgets(value1, LINE_MAX, stdin);

	//report to accessed1
	if((pid = fork()) < 0)
	{
		fprintf(stderr, "Creation of child process three failed");
		perror(NULL);
		exit(1);
	}
	else if(pid == 0)//child
	{
		close(0); dup(r2a2[0]); close(r2a2[0]); //move read of pipe r2a2 to stdin
		close(r2a2[1]); //close write to pipe r2a2

		close(1); dup(a2t2[1]); close(a2t2[1]); //move write of pipe a2t2 to stdout
		close(a2t2[0]);

		execve("accessed", accArgs2, NULL);//run accessed in child
		fprintf(stderr, "Error running child, accessed.c\n");
		perror(NULL);
		exit(1);
	}
	else//parent
	{
		close(r2a2[0]); //close read from pipe r2a2

		//write report's stdin to pipe r2a2's write
		if((write(r2a2[1], stdinString, strlen(stdinString))) != strlen(stdinString))
		{
			fprintf(stderr, "\nreport: Error when parent writing to pipe r2a2");
		}

		close(r2a2[1]); //close write to pipe r2a2

		wait(NULL);
	}

	//accessed1 to totalsize1
	if((pid = fork()) < 0)
	{
		fprintf(stderr, "Creation of child process four failed");
		perror(NULL);
		exit(1);
	}
	else if(pid == 0)//child
	{
		close(0); dup(a2t2[0]); close(a2t2[0]); //move read of pipe a2t2 to stdin
		close(a2t2[1]); //close write to pipe a2t2

		close(1); dup(t2r2[1]); close(t2r2[1]); //move write of pipe t2r2 to stdout
		close(t2r2[0]);

		execve("totalsize", NULL, enviro);
		fprintf(stderr, "Error running child, accessed.c\n");
		perror(NULL);
		exit(1);
	}
	else//parent
	{
		close(0); dup(t2r2[0]); close(t2r2[0]); //move read of pipe t2r2 to stdin
		close(t2r2[1]); //close write to pipe t2r2

		close(a2t2[1]); close(a2t2[0]);//close write and read to pipe a2t2
		while(flag)
		{
			sleep(1);
			printf("*");
		}
		flag = 1;
		wait(NULL);
	}

	//get second value from pipe
	char value2[LINE_MAX];
	fgets(value2, LINE_MAX, stdin);

	if(useKilo)
	{
		printf("\nA total of %s kilobytes are in regular files not accessed for %d days.\n", value1, numDays);
		printf("-------------------------\n");
		printf("A total of %s kilobytes are in regular files accessed within %d days.\n", value2, numDays);
	}
	else
	{
		printf("\nA total of %s bytes are in regular files not accessed for %d days.\n", value1, numDays);
		printf("-------------------------\n");
		printf("A total of %s bytes are in regular files accessed within %d days.\n", value2, numDays);
	}

	//free memory
	free(tDel);
	free(tPar);
	free(accArgs[2]);
	free(accArgs2[2]);
	free(pidString);

	exit(0);
}
