/** 
Davide Chen - 19207819 - davide.chen@ucdconnect.ie

C program to print the first 10 lines of a file with some optional parameters:
* -n K output the first K lines
* -V output Version info
* -h display and exit 
* -e print even lines

The sintax of the command line must be in the following format: ./assign1-19207819 [FILE NAME] [-option]

 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    FILE *fp;             // file pointer
    char *line = NULL;    // pointer to the current line
   	size_t   len  = 0;	  // size of the input buffer
   	int opt;			  // the option value
   	int parity = 2;		  // used for print the even lines
   	int n_row = 10;		  // number of row to be printed
 
    int cnt = 0;    
 	
 	//checking if the number of the argument is less than the correct format, if so display a warning
    if( argc < 2) { 
        printf("Insufficient Arguments!!!\n");
        printf("Please use \"[program_name] [file_name] [-option]\" format.\n");
        return -1;
    }

    // open file
    fp = fopen(argv[1],"r");

    // checking for file is exist or not
    if( fp == NULL ) {
        printf("\n%s file can not be opened !!!\n",argv[1]);
        return 1;   
    }

    //using getopt to accept optional parameters
   	opt = getopt(argc, argv, "nVhe");
 	
 	switch(opt) {
 		case 'n':	//set the requested number of rows then read and print lines from file one by one
 			n_row = atoi(argv[3]);
 			while ((getline(&line, &len, fp) != -1) && !(cnt >= n_row)) {
    				cnt++;
 					printf("%s",line); fflush(stdout);
    		}
    		break;

 		case 'V':	//print the version info
 			puts("-- Version info --");
 			puts("Name: Davide Chem\nEmail: davide.chen@ucdconnect.ie\nStudent number: 19207819");
 			break;

 		case 'h':	//display info about the program and exit
 			puts("This program is used for print the first 10 lines of FILE to standart output.");
 			puts("Use this following format to run the program: [program_name] [file_name] [-option]\n");
 			puts("Options:");
 			puts("-n K Output the first K lines");
 			puts("-V   Output version info");
 			puts("-h   Display info about the program and exit");
 			puts("-e   Print even lines");
 			break;

 		case 'e':	//read the lines from file one by one and print only the even lines
 			while ((getline(&line, &len, fp) != -1) && !(cnt >= n_row*2)) {
 				cnt++;
 				if(cnt % parity == 0) {
 					printf("%s",line); fflush(stdout);
 				}
    		}
 			break;

 		case '?':	//in case of invalid option display a warning
 			puts("Invalid option");
 			break;

 		default:	// read and print the first lines from file one by one
    		while ((getline(&line, &len, fp) != -1) && !(cnt >= n_row)) {
    				cnt++;
 					printf("%s",line); fflush(stdout);
    		}
    		break;
	}
 
   
     
    // close file
    fclose(fp);
 
    return 0;
}