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
	
	int size = gedFile.size;
	
	tagList * allTags = initializeTags(gedFile);
	
	for (int i = 0; i < size; i++) {
		printf("Length : %d, Level : %s, Tag : %s, Address : %s, Value : %s\n", allTags[i].length, allTags[i].level, allTags[i].tag, allTags[i].address, allTags[i].value);
	}
	
	
	
	
	
	
	
	
	//after it is done being used
	freeTaglist(allTags, size);
	
	obj = initObject(obj);
	
	fclose(inFile);

	return createError((ErrorCode)OK, -1);
	
}

char* printGEDCOM(const GEDCOMobject* obj) {
	return NULL;
}

void deleteGEDCOM(GEDCOMobject * obj) {
	
}

char* printError(GEDCOMerror err) {
	return NULL;
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










