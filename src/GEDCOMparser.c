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
	
	//temporary!
	(*obj)->header = malloc(sizeof(Header));
	(*obj)->header->encoding = ANSEL;
	fclose(inFile);

	return createError((ErrorCode)OK, -1);
	
}

char* printGEDCOM(const GEDCOMobject* obj) {
	
	if (obj == NULL) {
		return "";
	}
	
	char * temp = calloc(1024, sizeof(char));
	char * tempHolder = NULL;
	
	strcat(temp, "Header Information : \n");
	
	if (obj->header != NULL) {
		Header * h = obj->header;
		if (h->source != NULL) {
			strcat(temp, h->source);
			strcat(temp, "\n");
		}
		char * num = malloc(sizeof(char) * 32);
		if (h->gedcVersion >= 0) {
			strcat(temp, "GEDC Version : ");
			sprintf(num, "%f", h->gedcVersion);
			if (num != NULL) {
				strcat(temp, num);
				free(num);
			}
			strcat(temp, "\n");
		}
	}
	
	if (obj->submitter != NULL) {
		Submitter * s = obj->submitter;
		
		if (s->submitterName != NULL) {
			strcat(temp, "Submitter Name : ");
			strcat(temp, s->submitterName);
			strcat(temp, "\n");
		}
		
		if (s->address != NULL) {
			strcat(temp, "Submitter Address : ");
			strcat(temp, s->address);
			strcat(temp, "\n");
		}
		
		tempHolder = toString(s->otherFields);
		
		if (tempHolder != NULL) {
			temp = realloc(temp, (strlen(temp) + strlen(tempHolder) + 50) * sizeof(char));
			strcat(temp, "Header, OtherFields : \n");
			strcat(temp, tempHolder);
			free(tempHolder);
			strcat(temp, "\n");
		}
	}
	
	temp = realloc(temp, (strlen(temp)+ 1024) * sizeof(char));
	
	strcat(temp, "Header information completed\n");
	
	char * allOtherFields = toString(obj->families);
	
	if (allOtherFields != NULL) {
		temp = realloc(temp, (strlen(temp) + strlen(allOtherFields) + 1024) * sizeof(char));
		strcat(temp, allOtherFields);
		free(allOtherFields);
	}
	
	strcat(temp, "All information has been completely printed!\n");

	return temp;
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
	
	if (familyRecord == NULL || person == NULL) {
		return NULL;
	}
	
	if (familyRecord->individuals.length == 0) {
		return NULL;
	}
	
	Node * n = familyRecord->individuals.head;
	
	while (n!= NULL) {
		if (compare(person, n->data) == true) { // Found
			Individual * data = n->data;
			
			return data;
		}
		n = n->next;
	}
	
	// Not Found
	return NULL;
}


List getDescendants(const GEDCOMobject* familyRecord, const Individual* person) {
	
	List descendants = initializeList(printIndividual, deleteIndividual, compareIndividuals);
	
	if (familyRecord == NULL || person == NULL) {
		return descendants;
	}
	
	Individual * i = findPerson(familyRecord, compareFindPerson, person);
	
	recursiveDescendant(&descendants, i); 
	
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
	if (toBePrinted == NULL) {
		return "";
	}
	
	Event * e = toBePrinted;
	
	char * printEvent = calloc(2, sizeof(char));
	char * tempHolder = calloc(2048, sizeof(char));
	strcpy(printEvent, "");
	strcpy(tempHolder, "");
	strcat(tempHolder, "--------Event-------\n");
	if (e->type != NULL) { 
		strcat(tempHolder, "Type : ");
		strcat(tempHolder, e->type);
		strcat(tempHolder, "\n");
	}
	
	if (e->date != NULL) { 
		strcat(tempHolder, "Date : ");
		strcat(tempHolder, e->date);
		strcat(tempHolder, "\n");
	}
	
	if (e->place != NULL) { 
		strcat(tempHolder, "Place : ");
		strcat(tempHolder, e->place);
		strcat(tempHolder, "\n");
	}
	
	if (tempHolder != NULL) {
		printEvent = (char*)realloc(printEvent, (strlen(printEvent) + strlen(tempHolder) + 50) * sizeof(char));
		strcpy(printEvent, tempHolder);
		free(tempHolder);
	}
	
	tempHolder = toString(e->otherFields);
	
	if (tempHolder != NULL) {
		printEvent = (char*)realloc(printEvent, (strlen(printEvent) + strlen(tempHolder) + 150) * sizeof(char));	
		strcat(printEvent, tempHolder);
		free(tempHolder);
	}
	
	strcat(printEvent, "-----END OF EVENT----\n");
	
	return printEvent;
}


//INDIVIDUAL
void deleteIndividual(void* toBeDeleted) {
	
}

int compareIndividuals(const void* first,const void* second) {
	return 0;
}


char* printIndividual(void* toBePrinted) {
	
	if (toBePrinted == NULL) {
		return "";
	}
	
	Individual * e = toBePrinted;
	
	char * printField = calloc(1,sizeof(char));
	char * tempHolder = calloc(1,sizeof(char) * 2048);
	
	strcpy(tempHolder, "	@@@@ - Individual - @@@@\n");
	
	if (e->givenName != NULL) { 
		strcat(tempHolder, "	Given Name : ");
		strcat(tempHolder, e->givenName);
		strcat(tempHolder, "\n");
	}
	
	if (e->surname != NULL) { 
		strcat(tempHolder, "	Surname : ");
		strcat(tempHolder, e->surname);
		strcat(tempHolder, "\n");
	}

	
	if (tempHolder != NULL) {
		printField = (char*)realloc(printField, (strlen(printField) + strlen(tempHolder) + 50) * sizeof(char));	
		strcpy(printField, tempHolder);
		free(tempHolder);
	}
	
	tempHolder = toString(e->events);
	
	if (tempHolder != NULL) {
		printField = (char*)realloc(printField, (strlen(printField) + strlen(tempHolder) + 150) * sizeof(char));	
		strcat(printField, tempHolder);
		free(tempHolder);
	}
	
	tempHolder = toString(e->otherFields);
	
	if (tempHolder != NULL) {
		printField = (char*)realloc(printField, (strlen(printField) + strlen(tempHolder) + 150) * sizeof(char));	
		strcat(printField, tempHolder);
		free(tempHolder);
	}
	
	strcat(printField, "	@@@@@@@@ - END - @@@@@@@@\n");

	return printField;
}



//FAMILY
void deleteFamily(void* toBeDeleted) {
	
}

int compareFamilies(const void* first,const void* second) {
	return 0;
}


char* printFamily(void* toBePrinted) {
	if (toBePrinted == NULL) {
		return "";
	}
	
	Family * e = toBePrinted;
	
	char * printField = calloc(1,sizeof(char));
	char * tempHolder = calloc(1,sizeof(char) * 2048);
	
	
	strcpy(tempHolder, "");
	strcat(tempHolder, "***********************************************FAMILY************************************************\n");
	
	if (e->husband != NULL) { 
		strcat(tempHolder, "Husband : \n");
		char * husband = printIndividual(e->husband);
		if (husband != NULL) {
			strcat(tempHolder, husband);
			free(husband);
		}
		strcat(tempHolder, "\n");
	}
	
	if (e->wife != NULL) { 
		strcat(tempHolder, "Wife : \n");
		char * wife = printIndividual(e->wife);
		if (wife != NULL) {
			strcat(tempHolder, wife);
			free(wife);
		}
		strcat(tempHolder, "\n");
	}
	
	if (tempHolder != NULL) {
		printField = (char*)realloc(printField, (strlen(tempHolder) + 50) * sizeof(char));	
		strcpy(printField, tempHolder);
		free(tempHolder);
	}
	
	tempHolder = toString(e->events);
	
	if (tempHolder != NULL) {
		printField = (char*)realloc(printField, (strlen(printField) + strlen(tempHolder) + 150	) * sizeof(char));
		strcat(printField, "Family Events : \n");
		strcat(printField, tempHolder);
		free(tempHolder);
		strcat(printField, "\n");
	}
	
	tempHolder = toString(e->otherFields);
	
	if (tempHolder != NULL) {
		printField = (char*)realloc(printField, (strlen(printField) + strlen(tempHolder) + 150	) * sizeof(char));
		strcat(printField, "Other fields : \n");
		strcat(printField, tempHolder);
		strcat(printField, "\n");
		free(tempHolder);
	}
	
	strcat(printField, "****************************************END OF FAMILY********************************************\n");
	
		
	return printField;
}


//FIELD
void deleteField(void* toBeDeleted) {
	
}

int compareFields(const void* first,const void* second) {
	return 0;
}

char* printField(void* toBePrinted) {
	if (toBePrinted == NULL) {
		return "";
	}
	
	Field * e = toBePrinted;
	
	char * printField = calloc(1, sizeof(char) * 2);
	char * tempHolder = calloc(2048, sizeof(char));
	
	strcpy(printField, "");
	strcpy(tempHolder, "");
	strcat(tempHolder, "Field { \n");
	
	if (e->tag != NULL) { 
		strcat(tempHolder, "   Tag : ");
		strcat(tempHolder, e->tag);
		strcat(tempHolder, "\n");
	}
	
	if (e->value != NULL) { 
		strcat(tempHolder, "   Value : ");
		strcat(tempHolder, e->value);
		strcat(tempHolder, "\n");
	}
	
	strcat(tempHolder, "} End of Field\n");
	
	if (tempHolder != NULL) {
		printField = (char*)realloc(printField, (strlen(printField) + strlen(tempHolder) + 50) * sizeof(char));
	
		strcpy(printField, tempHolder);
	
		free(tempHolder);
	}
	
	return printField;
}










