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

char** getFiles(char** files, char* location) {

	char directory[100];
	snprintf(directory, sizeof directory, "%s%s", "/home/drmoo/Documents/CCode/OSHW1/", location);

	DIR* dir = opendir(directory);
	struct dirent* file;

	if (dir == NULL) {
		perror( "opendir() failed" );
	}

	int ret;
	ret = chdir(directory);

	if (ret == -1) {
		perror( "chdir() failed" );
	}

	int i = 0;
	while((file = readdir(dir)) != NULL) {
		printf( "found %s", file->d_name );
		fflush(NULL);

		struct stat buf;

		int rc = lstat( file -> d_name, &buf );

		if(rc == -1) {
			perror("lstat() failed");
		}

		printf( " (%d bytes)", (int)buf.st_size );
		fflush(NULL);

		if (S_ISREG(buf.st_mode)) {
			printf( " -- regular file \n" );	
			strcpy(files[i], file->d_name);
			i += 1;
		}
		else if (S_ISDIR(buf.st_mode)) {
			printf( " -- directory \n");
		}
	}
	return files;
}

char* readBuffer(char* location) {
	/* found good example code for reading in buffer from 'Michael' on stackoverflow.com */
	char* buff = NULL;
	FILE* fp = fopen(location, "r");

	if (fp == NULL) {
		perror("ERROR: fopen() failed");
	}
	if (fp != NULL) {
		//go to the end of the file	
		if (fseek(fp, 0L, SEEK_END) == 0) {
			long bufsize = ftell(fp); //get size of file
			if ( bufsize == -1 ) {
				perror( "ERROR: ftell() failed" );
			}

			buff = malloc(sizeof(char)* (bufsize+1)); //make buffer file size

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

	// //printf("argv: %s\n", location);
	
	// char name[80];
	// strcpy(name, location);
	// int fd = open(name, O_RDONLY);

	// if (fd == -1) {
	// 	perror( "ERROR: open() failed " );
	// }

	// int i = 0;
	// while (i < 1) {
	// 	int rc = read(fd, (char*)buff[80*(i)], 79);

	// 	if (rc == 0) {
	// 		break;
	// 	}
	// 	else {
	// 		printf("buff: %s", buff);
	// 		buff = realloc(buff, (80*(i+1)*sizeof(char)));
	// 		buff[rc*(80)*(i+1)] = '\0';
	// 	}
	// }
	return buff;
}

char* clearString(char* toclear) {
	memset(toclear, 0, strlen(toclear));
	return toclear;
}

char** getWordList(char** wordList, char* buff) {
	printf("buff: %s\n", buff);
	printf("buff length: %lu\n", strlen(buff));
	fflush(NULL);

	char* singleWord = (char*)malloc(30*sizeof(char));
	int i = 0;
	int j = 0;
	int wordCounter = -1;
	for(i = 0; i < strlen(buff); i++) {
		//printf("i: %d\n", i);
		//fflush(NULL);
		if (isalpha(buff[i])) { //begin word
			wordCounter += 1;
			j = 0;
			singleWord[0] = buff[i];
			while(1) { //reading a word
				j += 1;
				if(isalpha(buff[i+j])) {
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
		printf("word: %s\n", singleWord);
		fflush(NULL);
		strcpy(wordList[wordCounter], singleWord);
		singleWord = clearString(singleWord);
		//printf("buff[i+j]: %d\n", ispunct(buff[i+j]));
		//fflush(NULL);
		if (ispunct(buff[i+j])) {
			i += (j+1);
		}
		else {
			i += j;
		}
	}
	return wordList;
}

word* getFinalAnswer(word* answer, char** wordList, int answerLength) {
	int i = 0;
	while (i < 100) {
		printf("wordList[i]: %s\n", wordList[i]);
		i++;
	}
	return answer;
}

int main(int argc, char* argv[]) {
	printf( "argc is %d\n", argc);

	printf( "argv[0] is %s\n", argv[0]);
	printf( "argv[1] is %s\n", argv[1]); //potential directory 
	printf( "argv[2] is %s\n", argv[2]); //potential output list num

	//char** files = NULL;
	char** files = (char**)malloc(5*sizeof(char*));

	int i = 0;
	for(i = 0; i < 5; i++) {
		files[i] = (char*)malloc(50*sizeof(char));
	}

	files = getFiles(files, argv[1]);

	i = 0;
	while (i < 5) {
		printf("%s\n", files[i]);
		i += 1;
	}

	char* buff = NULL;

	buff = readBuffer(files[0]);
	//printf("buff: %s\n", buff);

	char** wordList = (char**)malloc(200*sizeof(char*));
	i = 0;
	for(i = 0; i < 200; i++) {
		wordList[i] = (char*)malloc(30*sizeof(char));
	}

	wordList = getWordList(wordList, buff);

	// char** answer = (char**)malloc(16*sizeof(char*));
	// int* answerNums = (int*)malloc(16*sizeof(int*));
	

	//making the actual structs and array list for the answer
	struct word* answer = malloc(sizeof(struct word)*200);

	printf("Allocated parallel arrays to be size 200\n");

	int a = 0;
	for( a = 0; a < 16; a++ ) {
		answer[a].wordString = (char*)malloc(50*sizeof(char));
		strcpy( answer[a].wordString, "booty");
		answer[a].num = a;
	}
	for( a = 0; a < 16; a++ ) {
		printf("wordString: %s\n", answer[a].wordString);
		printf("num: %d\n", answer[a].num = a);
	}

	answer = getFinalAnswer(answer, wordList, answerLength 200);

	/*
	code to free at end:
	for(i = 0, i < 5, i++) {
		free(files[i]);
	}
	free(files);
	*/

	return EXIT_SUCCESS;
}