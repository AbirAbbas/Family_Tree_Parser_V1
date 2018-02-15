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
	
	while (myfgets(temp, 257, inFile) != NULL) {
		a.allLines[size] = calloc(strlen(temp) + 1, sizeof(char));
		strcpy(a.allLines[size], temp);
		size++;
		a.allLines = realloc(a.allLines, sizeof(char*) * (size + 1));
	}
		
	a.size = size;
	
	return a;
}

tagList * initializeTags(arrayList a, int * arraySize) {
	
	if (a.allLines == NULL) {
		return NULL;
	}
	
	tagList * t = calloc(a.size, sizeof(tagList));
	char temp[1024];
	char ** array = a.allLines;
	bool isAddress = false;
	
	char * level = NULL;
	char * tag = NULL;
	char * value = NULL;
	char * address = NULL;
	char terminator[2];
	
	for (int i = 0, counter = 0; counter < a.size ; counter++) {
		isAddress = false;
		level = NULL;
		tag = NULL;
		value = NULL;
		
		strcpy(temp, array[counter]);
		t[i].length = strlen(temp);
		if(temp[0] == ' '|| temp[0] == '\n' || temp[0] == '\t') {
			t[i].format = 1;
		}
		else {
			t[i].format = 0;
		}
		
		if (temp[strlen(temp) - 1] == '\r' && temp[strlen(temp)] == '\n') {
			strcpy(terminator, "\r\n");
		}
		else if (temp[strlen(temp) - 1] == '\n') {
			strcpy(terminator, "\n");
		}
		else if (temp[strlen(temp) - 1] == '\r') {
			strcpy(terminator, "\r");
		}
		
		temp[strcspn(temp, "\r\n")] = '\0';
		
		stringInformation lines = stringTokenizer(temp);
		char ** tokenizedString = lines.tokenizedString;
		
		if (lines.size > 2 && (strcmp(tokenizedString[1], "CONT") == 0 || strcmp(tokenizedString[1], "CONC") == 0)) {
			
			if (tokenizedString[2] != NULL && strcmp(tokenizedString[1], "CONC") == 0) {
				if (i-1 != 0 && t[i-1].value != NULL) {
					
					t[i-1].value = realloc(t[i-1].value, (strlen(t[i-1].value) + 1024) * sizeof(char));
					char * combined = combineString(tokenizedString, 3, lines.size);
					strcat(t[i-1].value, combined);
					free(combined);
				}
				else if (i-1 != 0){
					t[i-1].value = combineString(tokenizedString, 3, lines.size);
				}
			}
			else if (tokenizedString[2] != NULL && strcmp(tokenizedString[1], "CONT") == 0) {
				if (i-1 != 0 && t[i-1].value != NULL) {
					
					t[i-1].value = realloc(t[i-1].value, (strlen(t[i-1].value) + 1024) * sizeof(char));
					char * combined = combineString(tokenizedString, 3, lines.size);
					strcat(t[i-1].value, terminator);
					strcat(t[i-1].value, combined);
					free(combined);
				}
				else if (i-1 != 0){
					t[i-1].value = combineString(tokenizedString, 3, lines.size);
				}
			}
			
			
		}
		else {
			
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
					t[i].recieverAddress = address;
					isAddress = true;
				}
			}
			if (tokenizedString[2] != NULL) {
				if (tokenizedString[2][0] == '@') {
					address = calloc(2, strlen(tokenizedString[2]) * sizeof(char));
					strcpy(address, tokenizedString[2]);
					t[i].senderAddress = address;
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
			i++;
			*arraySize = i;
		}	//for added if
	
	freeStringArray(tokenizedString, lines.size);
	}
	
	
	
	for (int i = 0; i < a.size; i++) {
		free(array[i]);
	}
	free(array);
	
	return t;
}

GEDCOMerror checkForError(tagList * arr, int size) {
	
	//All header requirements!
	bool headerCheck = true;
	bool submitterCheck = false;
	bool sourceCheck = false;
	bool charCheck = false;
	bool versCheck = false;
	bool gedcCheck = false;
	bool mainSubmitter = false;
	
	int endOfHeader = 0;
	
	int tempLevel = 0;
	int currentLevel = 0;
	
	
	if (arr == NULL) {
		return createError(INV_GEDCOM, -1);
	}
	
	if (strcmp(arr[0].tag, "HEAD") != 0) {
		return createError(INV_GEDCOM, -1);
	}
	
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
		
		if (arr[i].senderAddress != NULL && strlen(arr[i].senderAddress) > 22) {
			return createError(INV_RECORD, i + 1);
		}
		if (arr[i].recieverAddress != NULL && strlen(arr[i].recieverAddress) > 22) {
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
		
		if ((strcmp(arr[i].tag, "INDI") == 0	|| strcmp(arr[i].tag, "FAM") == 0 || strcmp(arr[i].tag, "TRLR") == 0) && strtol(arr[i].level, NULL, 10) != 0) {
			return createError(INV_RECORD, i + 1);
		}
		
		if ((strcmp(arr[i].tag, "HEAD") == 0) && strtol(arr[i].level, NULL, 10) != 0) {
			return createError(INV_HEADER, i + 1);
		}
		
		if (strcmp(arr[i].tag, "HEAD") != 0 && strtol(arr[i].level, NULL, 10) == 0 && headerCheck == true) {
			headerCheck = false;
			endOfHeader = i;
		}
		
		if (headerCheck == true) {
			if (strcmp(arr[i].tag, "GEDC") == 0) {
				gedcCheck = true;
			}
			
			if (gedcCheck == true) {
				if (strcmp(arr[i].tag, "VERS") == 0 && arr[i].value != NULL) {
					versCheck = true;
				}
			}
			
			if (strcmp(arr[i].tag, "CHAR") == 0 && arr[i].value != NULL) {
				charCheck = true;
			}
			if (strcmp(arr[i].tag, "SOUR") == 0 && arr[i].value != NULL) {
				sourceCheck = true;
			}
			if (strcmp(arr[i].tag, "SUBM") == 0 && (arr[i].value != NULL || arr[i].senderAddress != NULL)) {
				submitterCheck = true;
			}
		}
		
		if (strcmp(arr[i].tag, "SUBM") == 0 && strtol(arr[i].level, NULL, 10) == 0) {
			mainSubmitter = true;
		}
		
		currentLevel = strtol(arr[i].level, NULL, 10);
		
		if ((currentLevel - tempLevel) > 1) {
			return createError(INV_RECORD, i + 1);
		}
		
		tempLevel = currentLevel;
	}
	
	if (versCheck == false || charCheck == false || sourceCheck == false || submitterCheck == false) {	
		return createError(INV_HEADER, endOfHeader + 1);
	}
	
	if (mainSubmitter == false) {
		return createError(INV_GEDCOM, -1);
	}
	
	if (strcmp(arr[size-1].tag, "TRLR") != 0) {
		return createError(INV_GEDCOM, -1);
	}
	
	return createError(OK, -1);
}

void freeTaglist(tagList * t, int size) {
	
	if (t == NULL) {
		return;
	}
	
	char * value;
	char * level;
	char * Saddress;
	char * Raddress;
	char * tag;
	
	for (int i = 0; i < size; i ++) {
		value = t[i].value;
		tag = t[i].tag;
		Saddress = t[i].senderAddress;
		Raddress = t[i].recieverAddress;
		level = t[i].level;
		
		if (value != NULL) {
			free(value);
		}
		if (level != NULL) {
			free(level);
		}
		if (Saddress != NULL) {
			free(Saddress);
		}
		if (Raddress != NULL) {
			free(Raddress);
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
	
	for (char * counter = strtok(temp, " /"); counter != NULL; counter = strtok(NULL, " /")) {
		countDelim++;
	}
	
	tokenizedString = calloc(countDelim + 1, sizeof(char*));
	//tokenizedString = calloc(1,(countDelim + 1));
	strcpy(temp, input);
	
	int size = countDelim;
	
	countDelim = 0;
	
	for (char * counter = strtok(temp, " /"); counter != NULL; counter = strtok(NULL, " /")) {
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


//I downloaded fgets sourcecode and modified to accept \r's and 'n's
char * myfgets(char *dst, int max, FILE *fp)
{
	int c;
	int temp;
	char *p;
	long int position = 0;

	/* get max bytes or upto a newline */

	for (p = dst, max--; max > 0; max--) {
		
		if ((c = fgetc (fp)) == EOF)
			break;
		*p++ = c;
		
		position = ftell(fp);
		temp = fgetc(fp);
		if (temp == EOF) {
			break;
		}
		if (c == '\n' && temp != '\r') {
			fseek(fp, position, SEEK_SET);
			break;
		}
		else if (c == '\r' && temp != '\n') {
			fseek(fp, position, SEEK_SET);
			break;
		}
		
		fseek(fp, position, SEEK_SET);
	}
	
	*p = 0;
	if (p == dst || c == EOF)
		return NULL;
	return (p);
}

void recursiveDescendant(List * descendants, Individual * i) {
	if (i == NULL) {
		return;
	}
	
	if (i->families.length == 0) {
		return;
	}
	
	Node * n = i->families.head;
	
	while (n != NULL) {
		if (checkIfSpouse(n->data, i)) {
			Node * e = ((Family*)(n->data))->children.head;
			while (e!= NULL) {
				if (!compareFindPerson(i, e->data)) {
					recursiveDescendant(descendants, e->data);
					if (!checkIfExists(descendants, e->data)) {
						insertBack(descendants, createCopy(e->data));
					}
				}
				e=e->next;
			}
		}
		n = n->next;
	}
}

bool checkIfSpouse (Family * f, Individual * i) {
	
	if (f==NULL || i == NULL) {
		return false;
	}
	
	if (f->husband == NULL) {
		return false;
	}
	else if (f->wife == NULL) {
		return false;
	}
	
	else if (i==NULL) {
		return false;
	}
	
	else if (compareFindPerson(f->husband, i)) {
		return true;
	}
	
	else if (compareFindPerson(f->wife, i)) {
		return true;
	}
	else {
		return false;
	}
}

bool checkIfExists(List * l, Individual * i) {
	
	if (l == NULL || i == NULL) {
		return false;
	}
	
	Node * n = l->head;
	
	while (n!=NULL) {
		if (compareFindPerson(n->data, i)) {
			return true;
		}
		n = n->next;
	}
	return false;	
}

bool compareFindPerson(const void* first,const void* second) {
	
	if (first == NULL || second == NULL) {
		return false;
	}
	
	char temp[1024];
	
	strcpy(temp, ((Individual*)first)->givenName);
	strcat(temp, ", ");
	strcat(temp, ((Individual*)first)->surname);
	
	char secondTemp[1024];
	
	strcpy(secondTemp, ((Individual*)second)->givenName);
	strcat(secondTemp, ", ");
	strcat(secondTemp, ((Individual*)second)->surname);
	
	if (strcmp(temp, secondTemp) == 0) {
		return true;
	}
	else {
		return false;
	}

}

Individual * createCopy(Individual * input) {
	if (input == NULL) {
		return NULL;
	}
	
	Individual * i = malloc(sizeof(Individual));
	if (input->givenName != NULL) {
		i->givenName = malloc(strlen(input->givenName) * 2);
		strcpy(i->givenName, input->givenName);
	}
	
	if (input->surname != NULL) {
		i->surname = malloc(strlen(input->surname) * 2);
		strcpy(i->surname, input->surname);
	}
	
	i->otherFields = initializeList(printField, deleteField, compareFields);
	i->events = initializeList(printEvent, deleteEvent, compareEvents);
	i->families = initializeList(printFamily, deleteFamily, compareFamilies);
	
	return i;
	
}

