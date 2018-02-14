#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include "assert.h"
#include <unistd.h>

#include "LinkedListAPI.h"
#include "GEDCOMutilities.h"
#include "GEDCOMparser.h"

GEDCOMerror createGEDCOM(char* fileName, GEDCOMobject** obj) {
	
	bool filePass = false;
	
	//fileValidator : checks if file exists, if fileName is NULL, and if fileName ends with .ged
	if (fileName != NULL) {
			
		if ((strlen(fileName) - 3) > 0) {
			if (fileName[strlen(fileName) - 4] == '.' && fileName[strlen(fileName) - 3] == 'g' && fileName[strlen(fileName) - 2] == 'e' && fileName[strlen(fileName) - 1] == 'd') {
				if (fileExist(fileName)) {
					filePass = true;
				}
			}
		}
	}
	//Returns error code if failed!
	if (filePass == false) {
		obj = NULL;
		return createError((ErrorCode)INV_FILE, -1);
	}
	
	FILE * inFile = fopen(fileName, "r");
	
	arrayList gedFile = initializeArray(inFile);
	
	int * size = malloc(sizeof(int));
	
	tagList * allTags = initializeTags(gedFile, size);
	
	container c;
	c.t = allTags;
	c.size = *size;
	
	for (int i = 0; i < *size; i++) {
		printf("Length : %d, Level : %s, Tag : %s, Address : %s, Value : %s\n", allTags[i].length, allTags[i].level, allTags[i].tag, allTags[i].senderAddress, allTags[i].value);
	}
	
	GEDCOMerror e = checkForError(c.t, c.size);
	
	if (e.type == OK) {
		
	}
	else {
		return e;
	}
		
	//after it is done being used
	freeTaglist(allTags, *size);
	free(size);
	obj = initObject(obj);
	(*obj)->header = malloc(sizeof(Header));
	(*obj)->header->encoding = ANSEL;
	fclose(inFile);

	return createError((ErrorCode)OK, -1);
	
}

char* printGEDCOM(const GEDCOMobject* obj) {
	return NULL;
}

void deleteGEDCOM(GEDCOMobject * obj) {
	
}

char* printError(GEDCOMerror err) {
	char * error = calloc(1024, sizeof(char));
	if (err.type == OK) {
		strcpy(error, "No errors reported\n");
	}
	else if (err.type == INV_FILE) {
		strcpy(error, "Error while opening file of format .ged!\n");
	}
	else if (err.type == INV_GEDCOM) {
		strcpy(error, "Invalid Gedcom object, Missing header or TRLR character!\n");
		char * num = malloc(sizeof(char) * 32);
		sprintf(num, "%d", err.line);
		if (num != NULL) {
			strcat(error, num);
			free(num);
		}
		strcat(error, "\n");
	}
	else if (err.type == INV_HEADER) {
		strcpy(error, "Invalid Header Object, at line ");
		char * num = malloc(sizeof(char) * 32);
		sprintf(num, "%d", err.line);
		if (num != NULL) {
			strcat(error, num);
			free(num);
		}
		strcat(error, "\n");
	}
	else if (err.type == INV_RECORD) {
		strcpy(error, "Invalid Record Object, at line ");
		char * num = malloc(sizeof(char) * 32);
		sprintf(num, "%d", err.line);
		if (num != NULL) {
			strcat(error, num);
			free(num);
		}
		strcat(error, "\n");
	}
	else if (err.type == OTHER_ERROR) {
		strcpy(error, "System out of memory!\n");
	}
	
	return error;
}

Individual* findPerson(const GEDCOMobject* familyRecord, bool (*compare)(const void* first, const void* second), const void* person) {
	
	return NULL;
}


List getDescendants(const GEDCOMobject* familyRecord, const Individual* person) {
	
	List descendants = initializeList(printIndividual, deleteIndividual, compareIndividuals);

	return descendants;
}


// ******************* HELPER FUNCTIONS ***********************
//EVENTS
void deleteEvent(void* toBeDeleted) {
	
}

int compareEvents(const void* first,const void* second) {
	return 0;
}

char* printEvent(void* toBePrinted) {
	return NULL;
}


//INDIVIDUAL
void deleteIndividual(void* toBeDeleted) {
	
}

int compareIndividuals(const void* first,const void* second) {
	return 0;
}


char* printIndividual(void* toBePrinted) {
	return NULL;
}



//FAMILY
void deleteFamily(void* toBeDeleted) {
	
}

int compareFamilies(const void* first,const void* second) {
	return 0;
}


char* printFamily(void* toBePrinted) {
	return NULL;
}


//FIELD
void deleteField(void* toBeDeleted) {
	
}

int compareFields(const void* first,const void* second) {
	return 0;
}

char* printField(void* toBePrinted) {
	return NULL;
}










