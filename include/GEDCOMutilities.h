#ifndef GEDCOMUTILITIES_H
#define GEDCOMUTILITIES_H

#include "LinkedListAPI.h"
#include "GEDCOMparser.h"

typedef struct arrayList {
	char ** allLines;
	int size;
} arrayList;

typedef struct tagList {
	char * level;
	char * tag;
	char * senderAddress;
	char * recieverAddress;
	char * value;
	int length;
	int format;
} tagList;

typedef struct container {
	tagList * t;
	int size;
} container;

typedef struct {
	char ** tokenizedString;
	int size;
} stringInformation;

GEDCOMobject ** initObject (GEDCOMobject ** obj);

bool fileExist (const char * fileName);

GEDCOMerror createError (ErrorCode type, int line);

arrayList initializeArray(FILE * inFile);

tagList * initializeTags(arrayList a, int * arraySize);

stringInformation stringTokenizer(char * input);

char * combineString (char ** string, int first, int second);

void freeTaglist(tagList * t, int size);

void freeStringArray(char ** stringArray, int size);

char * myfgets(char *dst, int max, FILE *fp);

GEDCOMerror checkForError(tagList * arr, int size);

void recursiveDescendant(List * descendants, Individual * i);

bool checkIfSpouse (Family * f, Individual * i);

bool checkIfExists(List * l, Individual * i);

bool compareFindPerson(const void* first,const void* second);

Individual * createCopy(Individual * input);

#endif
