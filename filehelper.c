#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "main.h"
#include "tree.h"
#include "filehelper.h"
#include "utility.h"

int parseDOTFile(FILE *file, RootsArray *roots)
{
  char *line = NULL;
  size_t line_capacity = 256;
  size_t line_length = 0;
  bool valid_header = false;

  line = (char *)malloc(line_capacity);
  if (line == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }

  int input = 0;
  while ((input = fgetc(file)) != EOF)
  {
    if (line_length >= line_capacity)
    {
      line_capacity += STRING_DEFAULT_SIZE;
      char *temp = (char *)realloc(line, line_capacity);
      if (temp == NULL)
      {
        free(line);
        line = NULL;
        printf(ERROR_MEMORY_MESSAGE);
        return ERROR_MEMORY;
      }
      line = temp;
      temp = NULL;
    }

    line[line_length++] = input;

    if (input == '\n')
    {
      line[line_length] = '\0';

      // Valid header?
      if (strncmp(line, MAGIC_STRING, MAGIC_STRING_LENGTH) == 0)
      {
        valid_header = true;
        line_length = 0;
        continue;
      }
      if (!valid_header)
      {
        free(line);
        line = NULL;
        printf(ERROR_INVALID_FILE_MESSAGE);
        return ERROR_INVALID_FILE;
      }

      // Extraction of infos from the file into the trees
      char *child_node = (char*) malloc(line_capacity);
      char *parent_node = (char*) malloc(line_capacity);
      if (child_node == NULL || parent_node == NULL)
      {
        free(line);
        free(child_node);
        free(parent_node);
        printf(ERROR_MEMORY_MESSAGE);
        return ERROR_MEMORY;
      }
      if (sscanf(line, " \"%[^\"]\" -> \"%[^\"]\";", child_node, parent_node) == 2)
      {
        Person *child = NULL;
        if(createPersonFromString(child_node, &child) == ERROR_MEMORY)
        {
          free(child_node);
          free(parent_node);
          free(line);
          line = NULL;
          return ERROR_MEMORY;
        }

        Person *parent = NULL;
        if(createPersonFromString(parent_node, &parent) == ERROR_MEMORY)
        {
          free(child_node);
          free(parent_node);
          free(child);
          free(line);
          line = NULL;
          return ERROR_MEMORY;
        }
        Person *temp = NULL;
        int child_exists = getPersonFromIdSilent(roots, &temp, child->id_);
        if(child_exists == ERROR_MEMORY)
        {
          free(child_node);
          free(parent_node);
          free(child);
          free(parent);
          free(line);
          line = NULL;
          return ERROR_MEMORY;
        }
        if(temp != NULL)
        {
          freePerson(child);
          child = temp;
          temp = NULL;
        }
        int parent_exists = getPersonFromIdSilent(roots, &temp, parent->id_);
        if(parent_exists == ERROR_MEMORY)
        {
          free(child_node);
          free(parent_node);
          free(child);
          free(parent);
          free(line);
          line = NULL;
          return ERROR_MEMORY;
        }
        if(temp != NULL)
        {
          freePerson(parent);
          parent = temp;
          temp = NULL;
        }

        if(child_exists == PERSON_NOT_FOUND && parent_exists == PERSON_NOT_FOUND)
        {
          addRoot(roots, child);
          if(setParent(child,&(child->parent1_), parent) == ERROR_MEMORY)
          {
            free(child_node);
            free(parent_node);
            free(child);
            free(parent);
            free(line);
            line = NULL;
            return ERROR_MEMORY;
          }
        }
        else if(child_exists == SUCCESS && parent_exists == PERSON_NOT_FOUND)
        {
          switch (addParent(child, parent))
          {
          case TOO_MANY_PARENTS:
            free(child_node);
            free(parent_node);
            freePerson(parent);
            free(line);
            line = NULL;
            printf(ERROR_INVALID_FILE_MESSAGE);
            return ERROR_INVALID_FILE;
          case ERROR_MEMORY:
            free(child_node);
            free(parent_node);
            free(child);
            free(parent);
            free(line);
            line = NULL;
            return ERROR_MEMORY;
          }
        }
        else if(child_exists == PERSON_NOT_FOUND && parent_exists == SUCCESS)
        {
          if (addRoot(parent->children_, child) == ERROR_MEMORY)
          {
            free(child_node);
            free(parent_node);
            free(child);
            free(parent);
            free(line);
            line = NULL;
            return ERROR_MEMORY;
          }
          child->parent1_ = parent;
        }
        else if (child_exists == SUCCESS && parent_exists == SUCCESS)
        {
          switch (addParent(child, parent))
          {
          case TOO_MANY_PARENTS:
            free(child_node);
            free(parent_node);
            freePerson(parent);
            freePerson(child);
            free(line);
            line = NULL;
            printf(ERROR_INVALID_FILE_MESSAGE);
            return ERROR_INVALID_FILE;
          case ERROR_MEMORY:
            free(child_node);
            free(parent_node);  
            free(child);
            free(parent);
            free(line);
            line = NULL;
            return ERROR_MEMORY;
          }

          if(isInRootsArray(roots, parent) == true)
          {
            if(removeRoot(roots, parent) == ERROR_MEMORY)
            {
              free(child_node);
              free(parent_node);
              free(line);
              line = NULL;
              return ERROR_MEMORY;
            }
          }
          if(isInRootsArray(roots, child) == true)
          {
            if(removeRoot(roots, child) == ERROR_MEMORY)
            {
              free(child_node);
              free(parent_node);
              free(line);
              line = NULL;
              return ERROR_MEMORY;
            }
          }
        }
      }
      free(child_node);
      free(parent_node);
      line_length = 0;
    }
  }
  free(line);
  return SUCCESS;
}

int createPersonFromString(char *person_string, Person **person)
{
  int id = extractID(person_string);
  char *name = extractName(person_string);
  if(initPerson(person, name, id) == ERROR_MEMORY)
  {
    free(name);
    return ERROR_MEMORY;
  }
  free(name);
  return SUCCESS;
}

int extractID(char *person_string)
{
  char *id = strchr(person_string, '[');
  return atoi(id + 1);
}

char *extractName(char *person_string)
{
  char *name = (char *)malloc(strlen(person_string) + 1);
  if (name == NULL)
  {
    return NULL; // Mem alloc fail
  }
  strcpy(name, person_string);

  char *null_character = strchr(name, '[');
  if (null_character)
  {
    *null_character = '\0';
  }

  int length = strlen(name);
  while (length > 0 && name[length - 1] == ' ')
  {
    name[length - 1] = '\0';
    length--;
  }

  return name;
}

int drawAll(RootsArray* roots, char** parameters)
{
  Person** persons = NULL;
  int number_of_persons = 0;
  if (getArrayOfWholeTree(roots, &persons, &number_of_persons) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }

  printTreeToFileAndConsole(persons, number_of_persons ,
    (parameters != NULL) ? parameters[0] : NULL, parameters != NULL);

  free(persons);
  return SUCCESS;
}

int drawTree(Person* root_person, char* filename)
{
  if (root_person->parent1_ == NULL && root_person->parent2_ == NULL)
  {
    if(filename == NULL)
    {
      printf("\n");
    }
    else
    {
      FILE *file = fopen(filename, "w");
      if (file == NULL)
      {
        printf(ERROR_FILE_CANT_OPEN_MESSAGE);
        return ERROR_FILE_CANT_OPEN;
      }
      fprintf(file, "digraph FamilyTree\n{\n}\n");
      fclose(file);
      printf("Creating DOT-file was successful!\n");
    }
    
    return SUCCESS;
  }

  Queue *queue = NULL;
  Queue *explored = NULL;
  if (initQueue(&queue, &explored, root_person) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }

  int size = 0;
  if(processAncestors(root_person, queue, explored, &size) == ERROR_MEMORY)
  {
    freeQueue(queue->head_);
    free(queue);
    freeQueue(explored->head_);
    free(explored);
    return ERROR_MEMORY;
  }

  Person **persons = malloc(sizeof(Person*) * (size));
  if(persons == NULL)
  {
    freeQueue(queue->head_);
    free(queue);
    freeQueue(explored->head_);
    free(explored);
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }

  int new_valid_size = 0;
  for (int i = 0; i < size; i++)
  {
    Person* next_person = getNextInQueue(explored);
    if(next_person != NULL)
    {
      persons[i] = next_person;
      new_valid_size++;
    }
  }
  qsort(persons, new_valid_size, sizeof(Person *), comparePerson);
  printTreeToFileAndConsole(persons, size, filename, filename != NULL);

  freeQueue(queue->head_);
  free(queue);
  freeQueue(explored->head_);
  free(explored);
  free(persons);
  
  return SUCCESS;
}

int printTreeToFileAndConsole(Person **persons, int amountOfPersons, char *filename, int printToFile)
{
  FILE* file = NULL;
  char filename_with_extention[256];
  if (printToFile)
  {
    //make a DOT file, add .dot extension
    snprintf(filename_with_extention, sizeof(filename_with_extention), "%s.dot", filename);
    file = fopen(filename_with_extention, "w");
    if (file == NULL)
    {
      free(persons);
      printf(ERROR_FILE_CANT_OPEN_MESSAGE);
      return ERROR_FILE_CANT_OPEN;
    }

    fprintf(file, "digraph FamilyTree\n{\n");
  }
  for (int i = 0; i < amountOfPersons; i++)
  {
    Person* child = persons[i];
    if (child->parent1_ != NULL)
    {
      if(!printToFile)
      {
      printf("  %s [%i] -> %s [%i]\n",
      child->name_, child->id_, child->parent1_->name_, child->parent1_->id_);
      }
      else if (printToFile)
      {
        fprintf(file, "  \"%s [%i]\" -> \"%s [%i]\";\n", 
        child->name_, child->id_, child->parent1_->name_, child->parent1_->id_);
      }
    }

    if (child->parent2_ != NULL)
    {
            if(!printToFile)
      {
      printf("  %s [%i] -> %s [%i]\n",
      child->name_, child->id_, child->parent2_->name_, child->parent2_->id_);
      }
      else if (printToFile)
      {
        fprintf(file, "  \"%s [%i]\" -> \"%s [%i]\";\n", 
        child->name_, child->id_, child->parent2_->name_, child->parent2_->id_);
      }
    }
  }
  if (printToFile)
  {
    fprintf(file, "}\n");
    fclose(file);
    printf("Creating DOT-file was successful!\n");
  }
  printf("\n");
  return SUCCESS;
}
