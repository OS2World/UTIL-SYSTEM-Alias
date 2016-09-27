/* alias - the program loader for the alias subsystem
 *
 */

#include <doscalls.h>
#include <subcalls.h>
#include <stdio.h>
#include <malloc.h>

#define ADD		1
#define CLEAR	2
#define LIST	3
#define LOAD	4

#define ADD_NAME "ADDSYN"
#define CLEAR_NAME "CLEARSYN"
#define LIST_NAME "LIST_SYN"
#define MOD_NAME  "ALIAS"
#define PROC_NAME "KEYSTRIN"

#define TRUE	1
#define FALSE	0

char	buffer[80];

main(argc, argv)
int argc;
char **argv;
{
	int	rc;
	char	proc_name[64];
	char	filename[64];
	char	opt;
	struct KbdStringInLength length;
	char	inbuffer[5];
	char	* getline(FILE *);
	FILE	*file_handle;
	int	fromfile, action;
	unsigned (far pascal *proc_addr)();
	char module_name[64];
	unsigned module_handle;
	char objbuff[64];
	unsigned long reg_func = 0x20;

	if (argc == 1)
		{
		if (rc = KBDREGISTER(MOD_NAME, PROC_NAME, reg_func))
			{
			printf("Error: unable to load Alias subsystem.\n");
			DOSEXIT(1,1);
			}
		printf("Alias subsystem loaded for this session.\n");
		DOSEXIT(1,0);
		}
	argv++;
	opt = **argv;
	if (opt == '-' || opt == '/')
		{
		opt = *(++*argv);
		switch (opt)
			{
			case 'f':
			case 'F':
				fromfile=TRUE;
				action = ADD;
				argv++;
				strcpy(filename,*argv);
				argc--;
				break;
			case 'c':
			case 'C':
				action = CLEAR;
				break;
			case 'l':
			case 'L':
				action = LIST;
				break;
			default:
				usage();
				DOSEXIT(1,0);
				break;
			}
		}
	else
		{
		fromfile=FALSE;
		action = ADD;
		--argc;
		strcpy(buffer, *argv);
		while (--argc)
			{
			strcat(buffer," ");
			strcat(buffer, *++argv);
			}
		}
	if (rc = KBDSTRINGIN(buffer, &length, 1, 0))
 		{
 		printf("Error Alias system is not loaded in this session.\n");
 		DOSEXIT(1,1);
 		}
 
	if (rc = DOSGETMODHANDLE(MOD_NAME, &module_handle))
		{
		printf("Error Alias system is not loaded in this session.\n");
		DOSEXIT(1,1);
		}
	switch (action)
		{
		case ADD:
			if (rc = DOSGETPROCADDR(module_handle, ADD_NAME,
					(unsigned long far *) &proc_addr))
				{
				printf("Alias subsystem is not loaded in this session.\n");
				DOSEXIT(1,1);
				}
			break;
		case CLEAR:
			if (rc = DOSGETPROCADDR(module_handle, CLEAR_NAME,
					(unsigned long far *) &proc_addr))
				{
				printf("Alias subsystem is not loaded in this session.\n");
				DOSEXIT(1,1);
				}
			break;
		case LIST:
			if (rc = DOSGETPROCADDR(module_handle, LIST_NAME,
					(unsigned long far *) &proc_addr))
				{
				printf("Alias subsystem is not loaded in this session.\n");
				DOSEXIT(1,1);
				}
			break;
		}
	switch (action)
		{
		case ADD:
			if (!fromfile)
				{
				if (rc = (*proc_addr)(buffer))
					{
					printf("Error adding alias %s.\n", buffer);
					DOSEXIT(1,1);
					}
				}
			else
				{
				if ((file_handle = fopen(filename, "r")) != NULL)
					{
					while (getline(file_handle) != NULL)
						{
						if (rc = (*proc_addr)(buffer))
							{
							printf("Error adding alias %s.\n", buffer);
							fclose(file_handle);
							DOSEXIT(1,1);
							}
						}
					fclose(file_handle);
					}
				else
					{
					printf("Error, unable to open %s.\n", filename);
					DOSEXIT(1,1);
					}
				}
			break;
		case CLEAR:
			if (rc = (*proc_addr)())
				{
				printf("Error clearing alias list.\n");
				DOSEXIT(1,1);
				}
			break;
		case LIST:
			if (rc = (*proc_addr)())
				{
				printf("No alias loaded.\n");
				DOSEXIT(1,1);
				}
		}
	DOSEXIT(1,0);
}

char *
getline(hand)			 
FILE *hand;
{
	char inbuffer[80];
	if (fgets(inbuffer, 80, hand) == NULL)
		return(NULL);
	else
		{
		strcpy(buffer,inbuffer);
		buffer[strlen(buffer)-1] = '\0';
		return(buffer);
		}
}

usage()
{
	printf("ALIAS: add alias strings to KEYSTRIN\n");
	printf("Usage\tALIAS [-fcl] [cmd alias]\n");
	printf("\t\t-f filename: load aliases from filename\n");
	printf("\t\t-l         : list aliases\n");
	printf("\t\t-c         : clear aliases\n");
}
