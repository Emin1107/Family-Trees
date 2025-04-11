#ifndef FILEHELPER_H
#define FILEHELPER_H

#define ERROR_INVALID_FILE_MESSAGE "Error: Invalid input file content.\n"

#define MAGIC_STRING "digraph FamilyTree"
#define MAGIC_STRING_LENGTH 18

int parseDOTFile(FILE *file, RootsArray *roots);
int createPersonFromString(char *person_string, Person **person);
int extractID(char *person_string);
char *extractName(char *person_string);
int drawAll(RootsArray* roots, char** parameters);
int drawTree(Person* roots, char* filename);
int printTreeToFileAndConsole(Person **persons, int amountOfPersons, char *filename, int printToFile);

#endif
