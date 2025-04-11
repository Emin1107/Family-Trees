#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#include "main.h"
#include "utility.h"
#include "tree.h"
#include "filehelper.h"

int main(int argc, char **argv)
{
  FILE *file;
  int command_status = checkStartingConditions(argc, argv, &file);
  if (command_status != SUCCESS)
  {
    return command_status;
  }

  RootsArray *roots = NULL;
  if (createRootsArray(&roots) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }

  if (argc == AMOUNT_PARAMETERS)
  {
    int file_status = parseDOTFile(file, roots);
    if (file_status != SUCCESS)
    {
      freeTree(&roots);
      fclose(file);
      return file_status;
    }

    fclose(file);
  }
  printf(WELCOME_MESSAGE);

  int result = programLoop(roots);

  return result;
}

int checkStartingConditions(int argc, char **argv, FILE **file)
{
  if (argc > AMOUNT_PARAMETERS)
  {
    printf(ERROR_USAGE_MESSAGE);
    return ERROR_USAGE;
  }
  else if (argc == AMOUNT_PARAMETERS)
  {
    *file = fopen(argv[AMOUNT_PARAMETERS - 1], "r");
    if (*file == NULL)
    {
      printf(ERROR_FILE_CANT_OPEN_MESSAGE);
      return ERROR_FILE_CANT_OPEN;
    }
  }
  return SUCCESS;
}

int programLoop(RootsArray *roots)
{
  int command_status;

  do
  {
    char *input;

    command_status = getUserInput(&input);
    if (command_status != QUIT && command_status != EOF)
    {
      if (validateCommand(roots, input) == ERROR_MEMORY)
      {
        // ToDo: is mem error check needed
        freeTree(&roots);
        return ERROR_MEMORY;
      }
    }
    free(input);
  } while (command_status != QUIT && command_status != EOF);
  if (freeTree(&roots) == ERROR_MEMORY)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }

  return SUCCESS;
}

int getUserInput(char **string)
{
  int capacity = STRING_DEFAULT_SIZE;
  int count;
  *string = malloc(sizeof(char) * STRING_DEFAULT_SIZE);
  if (*string == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }

  do
  {
    printf(USERINPUT_PROMPT);
    int input = 0;
    count = 0;

    while ((input = getchar()) != '\n')
    {
      if (input == EOF)
      {
        free(*string);
        *string = NULL;
        return EOF;
      }
      if (count >= capacity - 1)
      {
        capacity += STRING_DEFAULT_SIZE;
        char *temp = realloc(*string, capacity * sizeof(char));
        if (temp == NULL)
        {
          free(*string);
          *string = NULL;
          return ERROR_MEMORY;
        }
        *string = temp;
      }

      (*string)[count] = input;
      count++;
    }
    (*string)[count] = '\0';
  } while (count == 0);
  if (strcmp(COMMAND_QUIT, *string) == 0)
  {
    printf(QUIT_MESSAGE);
    return QUIT;
  }
  return SUCCESS;
}

int validateCommand(RootsArray *roots, char *string)
{
  int parameter_amount = getParameterAmount(string);
  if (parameter_amount == -1)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }

  if (isCommandValid(string, COMMAND_HELP))
  {
    if (areParamtersValid(COMMAND_HELP_MIN_PARAMETERS, parameter_amount))
    {
      printf("%s\n", HELP_MESSAGE);
    }
  }
  else if (isCommandValid(string, COMMAND_ADD))
  {
    if (parameter_amount >= (COMMAND_ADD_MIN_PARAMETERS * -1))
    {
      if (addWrapper(roots, COMMAND_ADD_MIN_PARAMETERS, string) == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
    }
    else
    {
      printf(ERROR_PARAMETER_MESSAGE);
    }
  }
  else if (isCommandValid(string, COMMAND_LIST))
  {
    if (areParamtersValid(COMMAND_LIST_MIN_PARAMETERS, parameter_amount))
    {
      Person **persons = NULL;
      int size = 0;
      if(getArrayOfWholeTree(roots, &persons, &size))
      {
        return ERROR_MEMORY;
      }
      printList(persons, size);
      free(persons);
    }
  }
  else if (isCommandValid(string, COMMAND_CONNECT))
  {
    if (areParamtersValid(COMMAND_CONNECT_MIN_PARAMETERS, parameter_amount))
    {
      if (connectWrapper(roots, parameter_amount, string) == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
    }
  }
  else if (isCommandValid(string, COMMAND_RENAME))
  {
    if (parameter_amount >= (COMMAND_RENAME_MIN_PARAMETERS * -1))
    {
      if (renameWrapper(roots, parameter_amount, string) == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
      fflush(stdin);
    }
    else
    {
      printf(ERROR_PARAMETER_MESSAGE);
    }
  }
  else if (isCommandValid(string, COMMAND_DELETE))
  {
    if (areParamtersValid(COMMAND_DELETE_MIN_PARAMETERS, parameter_amount))
    {
      if(deleteWrapper(roots, parameter_amount, string) == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
    }
  }
  else if (isCommandValid(string, COMMAND_DRAW_ALL))
  {
    if (parameter_amount == 0 || parameter_amount == 1)
    {
      if (drawAllWrapper(roots, parameter_amount, string) == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
    }
    else
    {
      printf(ERROR_PARAMETER_MESSAGE);
    }
  }
  else if (isCommandValid(string, COMMAND_DRAW))
  {
    if (parameter_amount == 1 || parameter_amount == 2)
    {
      if (drawWrapper(roots, parameter_amount, string) == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
    }
    else
    {
      printf(ERROR_PARAMETER_MESSAGE);
    }
  }
  else if (isCommandValid(string, COMMAND_RELATIONSHIP))
  {
    if (areParamtersValid(COMMAND_RELATIONSHIP_MIN_PARAMETERS, parameter_amount))
    {
      if (relationshipWrapper(roots, parameter_amount, string) == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
    }
  }
  else if (isCommandValid(string, COMMAND_PRUNE))
  {
    if (areParamtersValid(COMMAND_PRUNE_MIN_PARAMETERS, parameter_amount))
    {
      if(pruneWrapper(roots) == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
    }
  }

  return SUCCESS;
}

int getParameterAmount(char *string)
{
  char *temp = malloc((strlen(string) + 1) * sizeof(char));
  if (temp == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return -1;
  }
  strncpy(temp, string, (strlen(string) + 1));
  int counter = -1;
  char *delimitter = " ";
  char *split = strtok(temp, delimitter);
  while (split != NULL)
  {
    split = strtok(NULL, delimitter);
    counter++;
  }
  free(temp);
  temp = NULL;
  return counter;
}

int isCommandValid(char *string, char *correct_command)
{
  int command_length = strlen(correct_command);
  char command[command_length + 1];
  while (*string == ' ')
  {
    string++;
  }
  strncpy(command, string, command_length);
  command[command_length] = '\0';

  return (strcmp(command, correct_command) == 0) && (string[command_length] == ' ' || string[command_length] == '\0');
}

int areParamtersValid(int correct_parameter_amount, int parameter_amount)
{
  if (parameter_amount != correct_parameter_amount)
  {
    printf(ERROR_PARAMETER_MESSAGE);
    return false;
  }
  return true;
}

int getParameters(char ***parameter_array, int parameter_amount, char *input_string)
{
  if (parameter_amount == COMMAND_ADD_MIN_PARAMETERS)
  {
    *parameter_array = (char **)malloc(sizeof(char *));
  }
  else
  {
    *parameter_array = (char **)malloc(parameter_amount * sizeof(char *));
  }
  if (*parameter_array == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }
  strtok(input_string, " ");                          // Remove Command
  if (parameter_amount == COMMAND_ADD_MIN_PARAMETERS) // Parameter can have spaces
  {
    char *temp = strtok(NULL, "");
    if (temp == NULL)
    {
      printf(ERROR_PARAMETER_MESSAGE);
      return ERROR_INVALID_PARAMETER;
    }
    (*parameter_array)[0] = (char *)malloc(strlen(temp) + 1);
    if ((*parameter_array)[0] == NULL)
    {
      free(*parameter_array);
      printf(ERROR_MEMORY_MESSAGE);
      return ERROR_MEMORY;
    }
    strcpy((*parameter_array)[0], temp);
    return SUCCESS;
  }
  for (int i = 0; i < parameter_amount; i++)
  {
    // ToDo: use constants
    char *temp = strtok(NULL, " ");
    (*parameter_array)[i] = (char *)malloc(strlen(temp) + 1);
    if ((*parameter_array)[i] == NULL)
    {
      for (int j = 0; j < i; j++) // Free previously allocated strings
      {
        free((*parameter_array)[j]);
      }
      free(*parameter_array);
      printf(ERROR_MEMORY_MESSAGE);
      return ERROR_MEMORY;
    }
    strcpy((*parameter_array)[i], temp);
  }
  return SUCCESS;
}

void freeParameters(char **parameter_array, int parameter_amount)
{
  if (parameter_amount == -1)
  {
    free(parameter_array[0]);
  }
  for (int i = 0; i < parameter_amount; i++)
  {
    free(parameter_array[i]);
  }
  free(parameter_array);
}

int checkForInt(char *int_string)
{
  unsigned long long int str_len = strlen(int_string);
  if(str_len > 5)
  {
    return 0;
  }
  for (unsigned long long int i = 0; i < str_len; i++)
  {
    if (int_string[i] < '0' || int_string[i] > '9')
    {
      printf(ERROR_INVALID_ID_WHEN_STRING, int_string);
      return 0;
    }
  }
  if(str_len == STRING_SIZE_OF_SIGNED_INT) //OVERFLOW CHECKED
  {
    for(int i = 0; i < STRING_SIZE_OF_SIGNED_INT; i++)
    {
      if(int_string[i] > STRING_MAX_INT[i])
      {
        printf(ERROR_INVALID_ID_WHEN_STRING, int_string);
        return 0;
      }
    }
  }
  return 1;
}

int validateConnectParameter(RootsArray *roots, char **parameter_array)
{
  if (!checkForInt(parameter_array[0]))
  {
    return 0;
  }
  int id_one = atoi(parameter_array[0]);
  Person *temp = NULL;
  if (getPersonFromId(roots, &temp, id_one) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }
  if (temp == NULL)
  {
    return 0;
  }
  char *temp_str = (char *)malloc(strlen(parameter_array[1]) + 1);
  strcpy(temp_str, parameter_array[1]);
  char *relation_string = strtok(temp_str, ":");
  if (strcmp(relation_string, parameter_array[1]) == 0)
  {
    if (strcmp(relation_string, CONST_PARENT1_STRING) != 0 && strcmp(relation_string, CONST_PARENT2_STRING) != 0)
    {
      free(temp_str);
      printf(ERROR_INVALID_RELATIONSHIP);
      return 0;
    }
  }
  else
  {
    if (strcmp(relation_string, CONST_PARENT1_STRING) != 0 && strcmp(relation_string, CONST_PARENT2_STRING) != 0)
    {
      free(temp_str);
      printf(ERROR_INVALID_RELATIONSHIP);
      return 0;
    }
    relation_string = strtok(NULL, "");
    if (strcmp(relation_string, CONST_GP1_STRING) != 0 && strcmp(relation_string, CONST_GP2_STRING) != 0)
    {
      free(temp_str);
      printf(ERROR_INVALID_RELATIONSHIP);
      return 0;
    }
  }
  free(temp_str);
  if (!checkForInt(parameter_array[2]))
  {
    return 0;
  }
  int id_two = atoi(parameter_array[2]);
  if (getPersonFromId(roots, &temp, id_two) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }
  if (temp == NULL)
  {
    return 0;
  }
  return 1;
}

int addWrapper(RootsArray *roots, int parameter_amount, char *input_string)
{
  char **parameters;
  int error_code = getParameters(&parameters, parameter_amount, input_string);
  if (error_code == ERROR_MEMORY || error_code == ERROR_INVALID_PARAMETER)
  {
    free(parameters);
    return error_code;
  }
  if (add(roots, parameters[0]) == ERROR_MEMORY)
  {
    freeParameters(parameters, parameter_amount);
    return ERROR_MEMORY;
  }
  freeParameters(parameters, parameter_amount);
  return SUCCESS;
}

int connectWrapper(RootsArray *roots, int parameter_amount, char *input_string)
{
  char **parameters;
  if (getParameters(&parameters, parameter_amount, input_string) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }
  int result = validateConnectParameter(roots, parameters);
  if (result == 0 || result == ERROR_MEMORY)
  {
    freeParameters(parameters, parameter_amount);
    return SUCCESS;
  }
  if (connect(roots, atoi(parameters[0]), parameters[1], atoi(parameters[2])) == ERROR_MEMORY)
  {
    freeParameters(parameters, parameter_amount);
    return ERROR_MEMORY;
  }
  freeParameters(parameters, parameter_amount);
  return SUCCESS;
}

int relationshipWrapper(RootsArray *roots, int parameter_amount,
                        char *input_string)
{
  char **parameters;
  if (getParameters(&parameters, parameter_amount, input_string) ==
      ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }
  if (!checkForInt(parameters[0]) || !checkForInt(parameters[1]))
  {
    freeParameters(parameters, parameter_amount);
    return ERROR_INVALID_PARAMETER;
  }

  int person_status;
  Person *person1 = NULL;
  if ((person_status = getPersonFromId(roots, &person1, atoi(parameters[0]))) != SUCCESS)
  {
    freeParameters(parameters, parameter_amount);
    return person_status;
  }

  Person *person2 = NULL;
  if ((person_status = getPersonFromId(roots, &person2, atoi(parameters[1]))) != SUCCESS)
  {
    freeParameters(parameters, parameter_amount);
    return person_status;
  }

  Person *temp;
  switch (bfsSearchPerson(person1, &temp, person2->id_))
  {
  case ERROR_MEMORY:
    freeParameters(parameters, parameter_amount);
    return ERROR_MEMORY;
  case SUCCESS:
    printf(RELATIONSHIP_RELATED, person1->name_, person1->id_, person2->name_, person2->id_);
    break;
  default:
    printf(RELATIONSHIP_NOT_RELATED, person1->name_, person1->id_, person2->name_, person2->id_);
    freeParameters(parameters, parameter_amount);
    return PERSON_NOT_FOUND;
  }

  if (areSiblings(person1, person2))
  {
    printf(RELATIONSHIP_OUTPUT, person1->name_, person1->id_, RELATIONSHIP_SIBLING, person2->name_, person2->id_);
  }
  //check if person1 is child of person 2
  if (isParent(person1, person2))
  {
    printf(RELATIONSHIP_OUTPUT, person1->name_, person1->id_, RELATIONSHIP_CHILD, person2->name_, person2->id_);
  }


  if (isGrandparent(person1, person2))
  {
    printf(RELATIONSHIP_OUTPUT, person1->name_, person1->id_, RELATIONSHIP_GRANDCHILD, person2->name_, person2->id_);
  }

  //check if person1 is parent of person2
  if (isParent(person2, person1))
  {
    printf(RELATIONSHIP_OUTPUT, person1->name_, person1->id_, RELATIONSHIP_PARENT, person2->name_, person2->id_);
  }

  if (isGrandparent(person2, person1))
  {
    printf(RELATIONSHIP_OUTPUT, person1->name_, person1->id_, RELATIONSHIP_GRANDPARENT, person2->name_, person2->id_);
  }
  printf("\n");
  freeParameters(parameters, parameter_amount);
  return SUCCESS;
}

void printList(Person **persons, int size)
{
  for(int i = 0; i < size; i++)
  {
    printf(LIST_MESSAGE, persons[i]->name_, persons[i]->id_);
    if(hasDirectRelativ(*persons[i]))
    {
      printf("*");
    }
    printf("\n");
  }
  if (size > 0)
  {
    printf(LIST_HINT);
  }
  else
  {
    printf("\n");
  }
}

int renameWrapper(RootsArray* roots, int parameter_amount, char* input_string)
{
  char** parameters;
  if (getParameters(&parameters, parameter_amount, input_string) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }

  if (!checkForInt(parameters[0]))
  {
    freeParameters(parameters, parameter_amount);
    return ERROR_INVALID_PARAMETER;
  }

  int id = atoi(parameters[0]);
  Person* person_to_rename = NULL;

  if (getPersonFromId(roots, &person_to_rename, id) == PERSON_NOT_FOUND)
  {
    freeParameters(parameters, parameter_amount);
    return PERSON_NOT_FOUND;
  }

  char* old_name = (char*)malloc(strlen(person_to_rename->name_) + 1);
  if (old_name == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    freeParameters(parameters, parameter_amount);
    return ERROR_MEMORY;
  }
  strcpy(old_name, person_to_rename->name_);

  size_t new_name_length = 0;
  for (int i = 1; i < parameter_amount; i++)
  {
    new_name_length += strlen(parameters[i]) + 1;
  }

  char* new_name = (char*)malloc(new_name_length);
  if (new_name == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    free(old_name);
    freeParameters(parameters, parameter_amount);
    return ERROR_MEMORY;
  }

  new_name[0] = '\0';

  for (int i = 1; i < parameter_amount; i++)
  {
    strcat(new_name, parameters[i]);
    if (i < parameter_amount - 1)
    {
      strcat(new_name, " ");
    }
  }

  // Free old name and assign the new one
  free(person_to_rename->name_);
  person_to_rename->name_ = new_name;

  printf(RENAMING_SUCCES_MESSAGE, old_name, id, person_to_rename->name_, id);

  // Clean up
  free(old_name);
  freeParameters(parameters, parameter_amount);
  return SUCCESS;
}

int drawAllWrapper(RootsArray* roots, int parameter_amount, char* input)
{
  char** parameters = NULL;
  if (parameter_amount == 1)
  {
    if (getParameters(&parameters, parameter_amount, input) == ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
  }

  int status_checker = drawAll(roots, parameters);

  if (parameters)
  {
    freeParameters(parameters, parameter_amount);
  }

  return status_checker;
}

int drawWrapper(RootsArray* roots, int parameter_amount, char* input_string)
{
  if (parameter_amount < 1 || parameter_amount > 2)
  {
    printf(ERROR_PARAMETER_MESSAGE);
    return ERROR_INVALID_PARAMETER;
  }

  char **parameters;
  if (getParameters(&parameters, parameter_amount, input_string) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }

  if (!checkForInt(parameters[0]))
  {
    freeParameters(parameters, parameter_amount);
    return ERROR_INVALID_PARAMETER;
  }

  int id = atoi(parameters[0]);

  Person *root_person = NULL;
  if (getPersonFromId(roots, &root_person, id) == PERSON_NOT_FOUND)
  {
    freeParameters(parameters, parameter_amount);
    return PERSON_NOT_FOUND;
  }

  int result;
  if (parameter_amount == 2)
  {
    result = drawTree(root_person, parameters[1]);
  }
  else
  {
    result = drawTree(root_person, NULL);
  }

  freeParameters(parameters, parameter_amount);
  return result;
}

int deleteWrapper(RootsArray* roots, int parameter_amount, char* input_string)
{
  char **parameters;
  if (getParameters(&parameters, parameter_amount, input_string) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }
  if(!checkForInt(parameters[0]))
  {
    printf(ERROR_INVALID_ID_WHEN_STRING, parameters[0]);
    freeParameters(parameters, parameter_amount);
    return ERROR_INVALID_PARAMETER;
  }
  int id = atoi(parameters[0]);
  Person* person_to_delete = NULL;
  for(int i = 0; i < roots->size_; i++)
  {
    if(bfsSearchPerson(roots->roots_array_[i], &person_to_delete, id) == ERROR_MEMORY)
    {
      freeParameters(parameters, parameter_amount);
      return ERROR_MEMORY;
    }
    if(person_to_delete != NULL)
    {
      break;
    }
  }
  if(person_to_delete == NULL)
  {
    printf(ID_DOESNT_EXIST_USE_LIST_MESSAGE, id);
    freeParameters(parameters, parameter_amount);
    return SUCCESS;
  }
  char* temp = person_to_delete->name_;
  person_to_delete->name_ = NULL;
  person_to_delete->name_ = (char*)malloc(2);
  if(person_to_delete->name_ == NULL)
  {
    freeParameters(parameters, parameter_amount);
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }
  strcpy(person_to_delete->name_, CONST_UNKNOW_STRING);
  printf(DELETE_SUCCESS_MESSAGE, temp, id);
  free(temp);
  freeParameters(parameters, parameter_amount);
  return SUCCESS;
}

int pruneWrapper(RootsArray* roots)
{
  int code = NOTHING_TO_PRUNE_CODE;
    Queue* explored = (Queue *)malloc(sizeof(Queue));
    if (explored == NULL)
    {
      printf(ERROR_MEMORY_MESSAGE);
      return ERROR_MEMORY;
    }
    explored->head_ = NULL;
    for(int i = 0; i < roots->size_; i++)
    {
      int temp = pruneTreeRec(roots->roots_array_[i], roots, explored);
      code = (temp == NOTHING_TO_PRUNE_CODE) ? code : temp;
      if(code == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
    }
  freeQueue(explored->head_);
  free(explored);
  if(code == PRUNED_SOMETHING)
  {
    printf(PRUNED_SOMETHING_MESSAGE);
  }
  else if(code == NOTHING_TO_PRUNE_CODE)
  {
    printf(NOTHING_TO_PRUNE_MESSAGE);
  }
  return SUCCESS;
}
