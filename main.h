#ifndef MAIN_H
#define MAIN_H

#include "tree.h"

#define WELCOME_MESSAGE "Welcome to the Family-Trees!\n\n"

#define AMOUNT_PARAMETERS 2

#define USERINPUT_PROMPT "> "
#define STRING_DEFAULT_SIZE 10

#define QUIT_MESSAGE "Bye.\n"

#define RELATIONSHIP_RELATED "%s [%d] and %s [%d] are related.\n"
#define RELATIONSHIP_NOT_RELATED "There is no relationship between %s [%d] and %s [%d].\n\n"
#define RELATIONSHIP_OUTPUT "%s [%d] is the %s of %s [%d].\n"
#define RELATIONSHIP_SIBLING "sibling"
#define RELATIONSHIP_CHILD "child"
#define RELATIONSHIP_GRANDCHILD "grandchild"
#define RELATIONSHIP_PARENT "parent"
#define RELATIONSHIP_GRANDPARENT "grandparent"

#define HELP_MESSAGE \
"Available commands:\n" \
"- help: Display this help message.\n" \
"- add <person name>\n" \
"  Add a new person to the program\n" \
"- list\n" \
"  Print all individuals that have been added (or initially imported)\n" \
"- connect <id> <relation> <id>\n" \
"  Connect a person to another person and specify their relationship\n" \
"- rename <id> <new name>\n" \
"  Rename an existing person\n" \
"- delete <id>\n" \
"  Remove an existing person from the family tree\n" \
"- draw-all <filename>\n" \
"  Write all relationships to the terminal or a DOT file. <filename> is optional\n" \
"- draw <id> <filename>\n" \
"  Write all ancestors of a person to the terminal or a DOT file. <filename> is optional\n" \
"- relationship <id1> <id2>\n" \
"  Find out whether a family relationship exists between two individuals\n" \
"- prune\n" \
"  Remove unknown leaf nodes from tree(s)\n" \
"- quit\n" \
"  End the program\n"
#define ID_DOESNT_EXIST_USE_LIST_MESSAGE "Error: ID [%d] does not exist. Use 'list' to find all available persons.\n\n"
#define RENAMING_SUCCES_MESSAGE "Renaming %s [%d] to %s [%d] was successful!\n\n"
#define DELETE_SUCCESS_MESSAGE "Deleting %s [%d] was successful!\n\n"

#define LIST_HINT "Persons with an asterisk (*) are not connected to any other person.\n\n"
#define LIST_MESSAGE "- %s [%d]"

// Negative paramters if the paramters cant be split by space
#define COMMAND_HELP "help"
#define COMMAND_HELP_MIN_PARAMETERS 0
#define COMMAND_ADD "add"
#define COMMAND_ADD_MIN_PARAMETERS -1
#define COMMAND_LIST "list"
#define COMMAND_LIST_MIN_PARAMETERS 0
#define COMMAND_CONNECT "connect"
#define COMMAND_CONNECT_MIN_PARAMETERS 3
#define COMMAND_RENAME "rename"
#define COMMAND_RENAME_MIN_PARAMETERS -2
#define COMMAND_DELETE "delete"
#define COMMAND_DELETE_MIN_PARAMETERS 1
#define COMMAND_DRAW_ALL "draw-all"
#define COMMAND_DRAW_ALL_MIN_PARAMETERS -1
#define COMMAND_DRAW "draw"
#define COMMAND_DRAW_MIN_PARAMETERS -1
#define COMMAND_RELATIONSHIP "relationship"
#define COMMAND_RELATIONSHIP_MIN_PARAMETERS 2
#define COMMAND_PRUNE "prune"
#define COMMAND_PRUNE_MIN_PARAMETERS 0
#define COMMAND_QUIT "quit"
#define COMMAND_QUIT_MIN_PARAMETERS 0

typedef enum _COMMAND_CODES_
{
  HELP,
  ADD,
  LIST,
  CONNECT,
  RENAME,
  DELETE,
  DRAW_ALL,
  DRAW,
  RELATIONSHIP,
  RPUNE,
  QUIT,
  INVALID
} COMMAND_CODES;

int checkStartingConditions(int argc, char **argv, FILE **file);
int programLoop(RootsArray *roots);
int getUserInput(char **string);
int validateCommand(RootsArray *roots, char *string);
int getParameterAmount(char *string);
int isCommandValid(char *string, char *correct_command);
int areParamtersValid(int correct_parameter_amount, int parameter_amount);
int getParameters(char ***parameter_array, int parameter_amount, char *input_string);
void freeParameters(char **parameter_array, int parameter_amount);
int validateConnectParameter(RootsArray *roots, char **parameter_array);
int checkForInt(char *int_string);
void printList(Person **persons, int size);

int addWrapper(RootsArray *roots, int parameter_amount, char *input_string);
int connectWrapper(RootsArray *roots, int parameter_amount, char *input_string);
int relationshipWrapper(RootsArray *roots, int parameter_amount, char *input_string);
int renameWrapper(RootsArray *roots, int parameter_amount, char *input);
int drawAllWrapper(RootsArray* roots, int parameter_amount, char* input);
int drawWrapper(RootsArray* roots, int parameter_amount, char* input_string);
int deleteWrapper(RootsArray* roots, int parameter_amount, char* input_string);
int pruneWrapper(RootsArray* roots);

#endif
