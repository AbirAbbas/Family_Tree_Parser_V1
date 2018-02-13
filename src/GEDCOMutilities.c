#include <stdio.h>
#include <ctype.h>
#include <strings.h>

#include "GEDCOMutilities.h"
#include "GEDCOMparser.h"
#include "LinkedListAPI.h"

GEDCOMobject ** initObject (GEDCOMobject ** obj) {	
	//creates object
	(*obj) = calloc(1, sizeof(GEDCOMobject));
	//initializes List
	(*obj)->individuals = initializeList(printIndividual, deleteIndividual, compareIndividuals);
	(*obj)->families = initializeList(printFamily, deleteFamily, compareFamilies);

	return obj;
}

bool fileExist (const char * fileName) {
	
	FILE * f = NULL;
	if ((f = fopen(fileName, "r"))) {
		fclose(f);
		return true;
	}
	
	return false;
}

GEDCOMerror createError (ErrorCode type, int line) {
	GEDCOMerror errorContainer;
	errorContainer.type = type;
	errorContainer.line = line;
	
	return errorContainer;
}

arrayList initializeArray(FILE * inFile) {
	arrayList a;
	char temp[1024];
	int size = 0;
	
	if (inFile == NULL) {
		return a;
	}
	
	a.allLines = calloc(1, sizeof(char*));
	
	while (fgets(temp, 257, inFile) != NULL) {
		a.allLines[size] = calloc(strlen(temp) + 1, sizeof(char));
		strcpy(a.allLines[size], temp);
		size++;
		a.allLines = realloc(a.allLines, sizeof(char*) * (size + 1));
	}
	
	for (int i = 0; i < strlen(temp); i ++) {
		if (temp[i] == '\r' && temp[i+1] == '\n') {
			strcpy(a.lineTerminator, "\r\n");
		}
		else if (temp[i] == '\n') {
			strcpy(a.lineTerminator, "\n");
		}
	}
	
	a.size = size;
	
	return a;
}

tagList * initializeTags(arrayList a) {
	tagList * t = calloc(a.size, sizeof(tagList));
	char temp[1024];
	char ** array = a.allLines;
	bool isAddress = false;
	
	char * level = NULL;
	char * tag = NULL;
	char * value = NULL;
	char * address = NULL;
	
	for (int i = 0; i < a.size; i++) {
		isAddress = false;
		level = NULL;
		tag = NULL;
		value = NULL;
		
		strcpy(temp, array[i]);
		t[i].length = strlen(temp);
		if(temp[0] == ' '|| temp[0] == '\n' || temp[0] == '\t') {
			t[i].format = 1;
		}
		else {
			t[i].format = 0;
		}
		
		temp[strcspn(temp, "\r\n")] = '\0';
		
		stringInformation lines = stringTokenizer(temp);
		char ** tokenizedString = lines.tokenizedString;
		
		if (tokenizedString[0] != NULL) {
			level = calloc(2, strlen(tokenizedString[0]) * sizeof(char));
			strcpy(level, tokenizedString[0]);
			t[i].level = level;
		}
		if (tokenizedString[1] != NULL) {
			if (tokenizedString[1][0] != '@') {
				tag = calloc(2, strlen(tokenizedString[1]) * sizeof(char));
				strcpy(tag, tokenizedString[1]);
				t[i].tag = tag;
			}
			else {
				address = calloc(2, strlen(tokenizedString[1]) * sizeof(char));
				strcpy(address, tokenizedString[1]);
				t[i].address = address;
				isAddress = true;
			}
		}
		if (tokenizedString[2] != NULL) {
			if (tokenizedString[2][0] == '@') {
				address = calloc(2, strlen(tokenizedString[2]) * sizeof(char));
				strcpy(address, tokenizedString[2]);
				t[i].address = address;
				if (tokenizedString[3] != NULL) {
					value = combineString(tokenizedString, 4, lines.size);
					t[i].value = value;
				}
			}
			else if (isAddress == true) {
				tag = calloc(2, strlen(tokenizedString[2]) * sizeof(char));
				strcpy(tag, tokenizedString[2]);
				t[i].tag = tag;
				if (tokenizedString[3] != NULL) {
					value = combineString(tokenizedString, 4, lines.size);
					t[i].value = value;
				}
			}
			else {
				value = combineString(tokenizedString, 3, lines.size);
				t[i].value = value;
			}
		}		
		
		freeStringArray(tokenizedString, lines.size);
	}
	
	for (int i = 0; i < a.size; i++) {
		free(array[i]);
	}
	free(array);
	
	return t;
}

GEDCOMerror checkForError(tagList * arr, int size) {
	
	for (int i = 0; i < size; i ++) {
		if (arr[i].length > 255) {
			//add check if error is in header or not!
			return createError(INV_RECORD, i + 1);
		}
		
		if (arr[i].format == 1) {
			return createError(INV_RECORD, i + 1);
		}
		
		if (arr[i].tag == NULL) {
			return createError(INV_RECORD, i + 1);
		}
		
		if (arr[i].level == NULL) {
			return createError(INV_RECORD, i + 1);
		}
		
		if (strlen(arr[i].tag) > 4) {
			return createError(INV_RECORD, i + 1);
		}
		
		if (arr[i].address != NULL && strlen(arr[i].address) > 22) {
			return createError(INV_RECORD, i + 1);
		}
		
		for (int x = 0; x < strlen(arr[i].tag); x++) {
			if (!isupper(arr[i].tag[x])) {
				return createError(INV_RECORD, i + 1);
			}
		}
		
		for (int x = 0; x < strlen(arr[i].level); x++) {
			if (isalpha(arr[i].level[x])) {
				return createError(INV_RECORD, i + 1);
			}
		}
		
		if (strtol(arr[i].level, NULL, 10) < 0 && strtol(arr[i].level, NULL, 10) > 99) {
			return createError(INV_RECORD, i + 1);
		}
	}
	
	if (strcmp(arr[size-1].tag, "TRLR") != 0) {
		return createError(INV_RECORD,  size);
	}
	
	return createError(INV_RECORD, -1);
}

void freeTaglist(tagList * t, int size) {
	
	if (t == NULL) {
		return;
	}
	
	char * value;
	char * level;
	char * address;
	char * tag;
	
	for (int i = 0; i < size; i ++) {
		value = t[i].value;
		tag = t[i].tag;
		address = t[i].address;
		level = t[i].level;
		
		if (value != NULL) {
			free(value);
		}
		if (level != NULL) {
			free(level);
		}
		if (address != NULL) {
			free(address);
		}
		if (tag != NULL) {
			free(tag);
		}
	}
	
	free(t);
}

void freeStringArray(char ** stringArray, int size) {
	
	if (stringArray == NULL) {
		return;
	}
	
	for (int i = 0; i < size; i ++) {
		free(stringArray[i]);
	}
	free(stringArray);
}

stringInformation stringTokenizer(char * input) {
	stringInformation tempInfo;
	char **tokenizedString = NULL;
	int countDelim = 0;
	char * temp = NULL;
	
	temp = calloc(strlen(input) + 1, sizeof(char));
	
	strcpy(temp, input);
	
	for (char * counter = strtok(temp, " "); counter != NULL; counter = strtok(NULL, " ")) {
		countDelim++;
	}
	
	tokenizedString = calloc(countDelim + 1, sizeof(char*));
	//tokenizedString = calloc(1,(countDelim + 1));
	strcpy(temp, input);
	
	int size = countDelim;
	
	countDelim = 0;
	
	for (char * counter = strtok(temp, " "); counter != NULL; counter = strtok(NULL, " ")) {
		tokenizedString[countDelim] = calloc(strlen(counter) + 1, sizeof(char));
		//tokenizedString[countDelim] = calloc(1,sizeof(char) * strlen(counter) + 1);
		strcpy(tokenizedString[countDelim], counter);
		countDelim++;
	}
	
	free(temp);
		
	tempInfo.tokenizedString = tokenizedString;
	tempInfo.size = size;
	
	return tempInfo;
}

char * combineString (char ** string, int first, int second) {
	
	int initial = first - 1; //2
	int final = second; //4
	int size = 0;
	
	if (string[initial] == NULL || string[final - 1] == NULL || first < 1) {
		return NULL;
	}
	//used to combine tokenized strings back to their original form!
	if (first == second) {
		char * combinedString = calloc(1, sizeof(char) * (strlen(string[first-1]) + 255));
		strcpy(combinedString, string[first-1]);
		return combinedString;
	}
	
	char * combinedString = NULL;
	
	for (int i = initial; i < final; i++) {
		size += (sizeof(string[i]) + 2);
	}
	
	combinedString = calloc(size + 255, sizeof(char));
	
	strcpy(combinedString, "");
	
	for (int i = initial; i < final; i++) {
		strcat(combinedString, string[i]);
		strcat(combinedString, " ");
	}
	
	if (combinedString[strlen(combinedString) - 1] == ' ') {
		combinedString[strlen(combinedString) - 1] = '\0';
	}
	
	return combinedString;
	
}

int checkForCon(tagList * t, int size) {
	tagList * copy = malloc(sizeof(tagList));
	int counter = 0;
	
	for (int i = 0; i < size; i ++) {
		if (t[i].tag != NULL)
		
	}
	
	
}