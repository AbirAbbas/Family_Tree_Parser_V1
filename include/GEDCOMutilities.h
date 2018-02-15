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

typedef struct {
	char * address;
	List * listPointer;
	Individual ** spousePointer;
	Submitter ** submitterPointer;
	void * initializedPointer;
	int type;
	int count;
} addressInformation;

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

Header * initializeHeader ();

Submitter * initializeSubmitter ();

Individual * initializeIndividual ();

Family * initializeFamily ();

GEDCOMerror parserDistributor (GEDCOMobject * obj, container allInformation, List * sendList, List * recieveList);

GEDCOMerror headParser (GEDCOMobject * obj, tagList * tagInformation, int * currentLocation, List * sendList, List * recieveList);

GEDCOMerror submitterParser (GEDCOMobject * obj, tagList * tagInformation, int * currentLocation, List * sendList, List * recieveList);

GEDCOMerror individualParser (GEDCOMobject * obj, tagList * tagInformation, int * currentLocation, List * sendList, List * recieveList);

GEDCOMerror familyParser (GEDCOMobject * obj, tagList * tagInformation, int * currentLocation, List * sendList, List * recieveList);

Field * initializeField ();

void dummyDelete(void* toBeDeleted);

int dummyCompare(const void* first,const void* second);

char* dummyPrint(void* toBePrinted);

int compareAddress(const void* first,const void* second);

addressInformation * createAddress (List * listPointer, Submitter ** submitterPointer, Individual ** spousePointer, char * address, void * initializedPointer, int count);

bool checkIndividualEvent (char * tag);

bool checkFamilyEvent (char * tag);

Event * initializeEvent ();

#endif
