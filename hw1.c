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
			#endif
			strcpy(files[i], file->d_name);
			i += 1;
		}
		else if (S_ISDIR(buf.st_mode)) {
			#if DEBUG_MODE
				printf( " -- directory \n");
			#endif
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

charpp getWordList(charpp wordList, char* buff) {
	#if DEBUG_MODE
		printf("buff: %s\n", buff);
		printf("buff length: %lu\n", strlen(buff));
		fflush(NULL);
	#endif

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
		fflush(NULL);
		if (j > 1) {
			strcpy(wordList.data[wordCounter], singleWord);
			wordList.num++;
			//printf("word: %s\n", singleWord);
		}
		else {
			wordCounter -= 1;
		}
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

highlevel getFinalAnswer(highlevel answer, charpp wordList) {
	
	//some function here to get answerLengthCurren

	int i = 0;
	while (i < wordList.num) { //loops thru all the words
		if (answer.uniquewords >= answer.currentMAX) { //checks if there needs to be more memory allocated
			answer.wordarray = realloc(answer.wordarray, sizeof(struct word)*(answer.currentMAX+16));
			int a = answer.currentMAX;
			for( ; a < answer.currentMAX+16; a++ ) {
				answer.wordarray[a].wordString = (char*)malloc(50*sizeof(char));
			}
			answer.currentMAX += 16;
			printf("Re-allocated parallel arrays to be size %d.\n", answer.currentMAX);
		}
		//printf("wordList[i]: %s\n", wordList[i]);
		int j = 0;
		int wordAdd = 1;
		while (j < answer.uniquewords) { //loops through all the words already added to answer 
			if (strcmp(wordList.data[i], answer.wordarray[j].wordString) == 0) { //word is already in answers
				//printf("here1");
				wordAdd = 0; //don't need to add the word to answer array
				answer.wordarray[j].num++; //increment answer counter
				break;
			}
			j++;
		}
		if (wordAdd == 1) { //need to add it to answers
			answer.wordarray[answer.uniquewords].wordString = wordList.data[i];
			answer.wordarray[answer.uniquewords].num = 1;
			answer.uniquewords++;
		}
		i++;
	}
	return answer;
}

void printAnswer(highlevel answer, int wordListNum) {
	int i = 0;
	printf("ALl done (succesfully read %d words; %d unique words).\n", wordListNum, answer.uniquewords);
	printf("All words (and corresponding counts are: \n");
	while (i < answer.uniquewords) {
		printf("%s -- %d\n", answer.wordarray[i].wordString, answer.wordarray[i].num);
		i++;
	}
}

int main(int argc, char* argv[]) {
	#if DEBUG_MODE
		printf( "argc is %d\n", argc);
	#endif

	#if DEBUG_MODE
		printf( "argv[0] is %s\n", argv[0]);
		printf( "argv[1] is %s\n", argv[1]); //potential directory 
		printf( "argv[2] is %s\n", argv[2]); //potential output list num
	#endif

	//char** files = NULL;
	char** files = (char**)malloc(5*sizeof(char*));

	int i = 0;
	for(i = 0; i < 5; i++) {
		files[i] = (char*)malloc(50*sizeof(char));
	}

	files = getFiles(files, argv[1]);

	#if DEBUG_MODE
		i = 0;
		while (i < 5) {
			printf("%s\n", files[i]);
			i += 1;
		}
	#endif

	char* buff = NULL;

	buff = readBuffer(files[0]);
	//printf("buff: %s\n", buff);

	struct charpp wordListPP;
	wordListPP.data = (char**)malloc(200*sizeof(char*));
	wordListPP.num = 0;
	i = 0;
	for(i = 0; i < 200; i++) {
		wordListPP.data[i] = (char*)malloc(30*sizeof(char));
	}

	//char** wordList = (char**)malloc(200*sizeof(char*));


	wordListPP = getWordList(wordListPP, buff);
	#if DEBUG_MODE
		printf("Total number of words: %d\n", wordListPP.num);
	#endif

	// char** answer = (char**)malloc(16*sizeof(char*));
	// int* answerNums = (int*)malloc(16*sizeof(int*));
	

	//making the actual structs and array list for the answer
	//int answerLength = 200;
	struct highlevel answerPP;
	answerPP.wordarray = malloc(sizeof(struct word)*16);
	answerPP.uniquewords = 0;
	answerPP.currentMAX = 16;

	//struct word* answer = malloc(sizeof(struct word)*answerLength);

	printf("Allocated parallel arrays to be size 16\n");

	int a = 0;
	for( a = 0; a < 16; a++ ) {
		answerPP.wordarray[a].wordString = (char*)malloc(50*sizeof(char));

		//answer[a].wordString = (char*)malloc(50*sizeof(char));
		//strcpy( answer[a].wordString, "");
		//answer[a].num = a;
	}

	answerPP = getFinalAnswer(answerPP, wordListPP);

	printAnswer(answerPP, wordListPP.num);
	/*
	code to free at end:
	for(i = 0, i < 5, i++) {
		free(files[i]);
	}
	free(files);
	*/

	return EXIT_SUCCESS;
}