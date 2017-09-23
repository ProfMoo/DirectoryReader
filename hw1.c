#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

typedef struct word {
	char* wordString;
	int num;
}word;

typedef struct charpp {
	char** data;
	int num;
}charpp;

typedef struct highlevel {
	word* wordarray;
	int uniquewords;
	int currentMAX;
}highlevel;

void getFiles(char*** dataFiles, char* location, int* numFiles) {
	DIR* dir = opendir(location);
	struct dirent* file;

	if (dir == NULL) {
		perror( "opendir() failed" );
	}

	int ret;
	ret = chdir(location);

	if (ret == -1) {
		perror( "chdir() failed" );
	}

	int i = 0;
	while((file = readdir(dir)) != NULL) {
		#if DEBUG_MODE
			printf( "found %s", file->d_name );
			fflush(NULL);
		#endif

		struct stat buf;

		int rc = lstat( file -> d_name, &buf );

		if(rc == -1) {
			perror("lstat() failed");
		}

		#if DEBUG_MODE
			printf( " (%d bytes)", (int)buf.st_size );
			fflush(NULL);
		#endif
		

		if (S_ISREG(buf.st_mode)) {
			#if DEBUG_MODE
				printf( " -- regular file \n" );
				fflush(NULL);	
			#endif

			if (i == 0) {
				*dataFiles = (char**)calloc((i+1), sizeof(char*));
				//(*files).data = (char**)malloc( (i+1)*sizeof(char*) );
			}
			else {
				*dataFiles = realloc(*dataFiles, (i+1)*sizeof(char*));
			}

			(*dataFiles)[i] = (char*)calloc(80, sizeof(char));
			//(*dataFiles)[i] = (char*)malloc(80*sizeof(char));	
			(*numFiles)++;
			strcpy((*dataFiles)[i], file->d_name);
			i += 1;
		}
		else if (S_ISDIR(buf.st_mode)) {
			#if DEBUG_MODE
				printf( " -- directory \n");
				fflush(NULL);
			#endif
		}
	}
	closedir(dir);
}

char* readBuffer(char*** dataFiles, int* numFiles) {
	/*received 'files' of type charpp. has an array of strings of file locations, 
	and has the amount of files to be read in */

	int currentbuffsize = 0;
	char* totalBuffer;
	#if DEBUG_MODE
		printf("*numFiles %d\n", *numFiles);
	#endif
	for( int i = 0; i < (*numFiles); i++ ) {
		#if DEBUG_MODE
			printf("i: %d\n", i);
			fflush(NULL);
		#endif
		/* found good example code for reading in buffer from 'Michael' on stackoverflow.com */
		char* buff = NULL;
		FILE* fp = fopen((*dataFiles)[i], "r");

		if (fp == NULL) {
			perror("ERROR: fopen() failed RIP");
			//break;
			continue;
		}
		if (fp != NULL) {
			#if DEBUG_MODE
				printf("reading %s\n", (*dataFiles)[i]);
			#endif
			//go to the end of the file	
			if (fseek(fp, 0L, SEEK_END) == 0) {
				long bufsize = ftell(fp); //get size of file
				if ( bufsize == -1 ) {
					perror( "ERROR: ftell() failed" );
				}

				buff = calloc(bufsize+1, sizeof(char));

				if (i == 0) {
					totalBuffer = calloc(bufsize+1, sizeof(char));
					//totalBuffer = malloc(sizeof(char)*(bufsize+1));
					currentbuffsize = bufsize;
				}
				if (i > 0) {
					totalBuffer = realloc(totalBuffer, sizeof(char)*(bufsize+1+currentbuffsize));
					currentbuffsize += bufsize;
				}

				if ( fseek(fp, 0L, SEEK_SET) != 0 ) {
					perror( "ERROR: fseek() failed" );
				}

				size_t newLen = fread(buff, sizeof(char), bufsize, fp); //read into memory
				if ( ferror(fp) != 0 ) {
					perror(" ERROR: fread() failed" );
				}
				else {
					buff[newLen++] = '\0';
				}
			}
			fclose(fp);
		}

		#if DEBUG_MODE
			printf("adding to totalBuffer this: %s\n", buff);
		#endif
		if (i > 0) {
			strcat(totalBuffer, " ");
		}
		strcat(totalBuffer,buff);

		free(buff);
		buff = NULL;
	}
	#if DEBUG_MODE
		printf("about to return totalBuffer: %s\n", totalBuffer);
	#endif
	return totalBuffer;
}

char* clearString(char* toclear) {
	memset(toclear, 0, strlen(toclear));
	return toclear;
}

void getWordList(char*** dataWordList, char* buff, int* numWordList) {
	#if DEBUG_MODE
		printf("buff: %s\n", buff);
		printf("buff length: %lu\n", strlen(buff));
		fflush(NULL);
	#endif

	char* singleWord = (char*)calloc(80, sizeof(char));
	int i = 0;
	int j = 0;
	int wordCounter = -1;
	for(i = 0; i < strlen(buff); i++) {
		if (isalpha(buff[i]) || isdigit(buff[i])) { //begin word
			wordCounter += 1;
			j = 0;
			singleWord[0] = buff[i];
			while(1) { //reading a word
				j += 1;
				if(isalpha(buff[i+j])) {
					singleWord[j] = buff[i+j]; 
				}
				else if(isdigit(buff[i+j])) {
					singleWord[j] = buff[i+j]; 
				}
				else {
					break;
				}
			}
		}
		else {
			continue;
		}
		fflush(NULL);
		if (j > 1) {
			(*dataWordList)[wordCounter] = (char*)calloc(j+1, sizeof(char));
			((*dataWordList)[wordCounter])[j] = '\0'; 
			strcpy((*dataWordList)[wordCounter], singleWord);
			(*numWordList)++;
			#if DEBUG_MODE
				printf("word: %s\n", singleWord);
			#endif
		}
		else {
			wordCounter -= 1;
		}
		singleWord = clearString(singleWord);
		i += j;
	}
	free(singleWord);
	singleWord = NULL;
}

void getFinalAnswer(word** wordarray, int* uniquewords, int* currentMAX, char*** dataWordList, int* numWordList) {
	int i = 0;
	while (i < (*numWordList)) { //loops thru all the words
		if ((*uniquewords) >= (*currentMAX)) { //checks if there needs to be more memory allocated
			(*wordarray) = realloc((*wordarray), sizeof(struct word)*((*currentMAX)+16));
			int a = (*currentMAX);
			#if DEBUG_MODE
				printf("(*currentMAX): %d\n", (*currentMAX));
			#endif
			for( ; a < (*currentMAX)+16; a++ ) {
				(*wordarray)[a].wordString = calloc(80, sizeof(char));
			}
			(*currentMAX) += 16;
			printf("Re-allocated parallel arrays to be size %d.\n", (*currentMAX));
			fflush(NULL);
		}
		int j = 0;
		int wordAdd = 1;
		while (j < (*uniquewords)) { //loops through all the words already added to (*answer) 
			if (strcmp((*dataWordList)[i], (*wordarray)[j].wordString) == 0) { //word is already in (*answer)s
				wordAdd = 0; //don't need to add the word to (*answer) array
				(*wordarray)[j].num++; //increment (*answer) counter
				break;
			}
			j++;
		}
		if (wordAdd == 1) { //need to add it to (*answer)
			strcpy((*wordarray)[(*uniquewords)].wordString, (*dataWordList)[i]);
			(*wordarray)[(*uniquewords)].num = 1;
			(*uniquewords)++;
		}
		i++;
	}
}

void printAnswer(word** answer, int wordListNum, int printNum, int exclusivePrint, int toPrint) {
	int i = 0;
	printf("All done (successfully read %d words; %d unique words).\n", wordListNum, toPrint);
	fflush(NULL);
	if (exclusivePrint == 1) {
		printf("First %d words (and corresponding counts) are:\n", printNum);\
		fflush(NULL);
	}
	else {
		printf("All words (and corresponding counts) are:\n");
		fflush(NULL);
	}
	while (i < printNum) {
		printf("%s -- %d\n", (*answer)[i].wordString, (*answer)[i].num);
		i++;
	}
}

int main(int argc, char* argv[]) {
	int i, a;

	int printNum = 0;
	if (argc < 2) {
		perror( "ERROR: no directory entered\n" );
		return EXIT_FAILURE;
	}
	if (argc == 3) {
		printNum = atoi(argv[2]);
	}
	#if DEBUG_MODE
		printf( "argc is %d\n", argc);
	#endif

	#if DEBUG_MODE
		printf( "argv[0] is %s\n", argv[0]);
		printf( "argv[1] is %s\n", argv[1]); //potential directory 
		printf( "argv[2] is %s\n", argv[2]); //potential output list num
		printf( "converted argv[2] is %d\n", printNum);
	#endif

	//charpp files;
	char** dataFiles;
	int numFiles = 0;

	getFiles(&dataFiles, argv[1], &numFiles);

	#if DEBUG_MODE
		printf("Printing result of getting .txt file names\n");
		fflush(NULL);
		i = 0;
		printf("files.num: %d\n", numFiles);
		while (i < numFiles) {
			printf("%s\n", dataFiles[i]);
			i += 1;
		}
	#endif

	char* buff;
	buff = readBuffer(&dataFiles, &numFiles);

	//struct charpp wordListPP;
	char** filesWordList;
	int numWordList;
	filesWordList = (char**)calloc(15000, sizeof(char*));
	numWordList = 0;

	getWordList(&filesWordList, buff, &numWordList);

	#if DEBUG_MODE
		printf("Total number of words: %d\n", numWordList);
	#endif
	
	//struct highlevel answerPP;
	word* wordarray;
	int uniquewords;
	int currentMAX;
	wordarray = (word*)calloc(16, sizeof(struct word));
	uniquewords = 0;
	currentMAX = 16;

	printf("Allocated initial parallel arrays of size 16.\n");
	fflush(NULL);

	a = 0;
	for( a = 0; a < 16; a++ ) {
		wordarray[a].wordString = (char*)calloc(80, sizeof(char));
	}

	getFinalAnswer(&wordarray, &uniquewords, &currentMAX, &filesWordList, &numWordList);
	//getFinalAnswer(&answerPP, &wordListPP);

	if (printNum != 0) {
		printAnswer(&wordarray, numWordList, printNum, 1, uniquewords);
	}
	else {
		printAnswer(&wordarray, numWordList, uniquewords, 0, uniquewords);
	}

	/*free all memory*/
	#if DEBUG_MODE
		printf("answer.currentMAXMAIN: %d\n", currentMAX);
	#endif
	for ( i = 0; i < currentMAX; i++ ) {
		free(wordarray[i].wordString);
		wordarray[i].wordString = NULL;
	}
	free(wordarray);
	wordarray = NULL;

	for ( i = 0; i < numWordList; i++ ) {
		free(filesWordList[i]);
		filesWordList[i] = NULL;
	}
	free(filesWordList);
	filesWordList = NULL;

	for ( i = 0; i < numFiles; i++ ) {
		free(dataFiles[i]);
		dataFiles[i] = NULL;
	}
	free(dataFiles);
	dataFiles = NULL;

	free(buff);
	buff = NULL;

	return EXIT_SUCCESS;
}