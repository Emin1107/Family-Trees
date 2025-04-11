#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "utility.h"

int createRootsArray(RootsArray **roots)
{
  *roots = (RootsArray *)malloc(sizeof(RootsArray));
  if (*roots == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }
  (*roots)->roots_array_ = (Person **)malloc(sizeof(Person *));
  if ((*roots)->roots_array_ == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    free(*roots);
    return ERROR_MEMORY;
  }
  (*roots)->roots_array_[0] = NULL;
  (*roots)->size_ = 0;
  return SUCCESS;
}

void freeRootsArray(RootsArray **roots)
{
  for (int i = 0; i < (*roots)->size_; i++)
  {
    Person *person_to_free = (*roots)->roots_array_[i];
    freePerson(person_to_free);
  }
  free((*roots)->roots_array_);
  free(*roots);
}

// ToDo: check if free function is allowed to use malloc?
//(If memory error occurs this function will likly also run into mem error)
int freeTree(RootsArray **roots)
{
  if (*roots == NULL)
  {
    return SUCCESS;
  }

  Queue *queue = NULL;
  Queue *explored = NULL;
  int size = 0;
  if (fillQueuesWithAllPersons(*roots, &queue, &explored, &size) == ERROR_MEMORY)
  {
    if ((*roots)->roots_array_ != NULL)
    {
      free((*roots)->roots_array_);
    }
    if (*roots != NULL)
    {
      free(*roots);
    }
    return ERROR_MEMORY;
  }
  if (size != 0)
  {
    if (explored->head_ != NULL)
    {
      freeQueueWithPerson(explored->head_);
      if (explored != NULL)
      {
        free(explored);
      }
    }
    if (queue != NULL)
    {
      free(queue);
    }
  }
  if ((*roots)->roots_array_ != NULL)
  {
    free((*roots)->roots_array_);
  }
  if (*roots != NULL)
  {
    free(*roots);
  }
  return SUCCESS;
}

int addRoot(RootsArray *roots, Person *root_to_add)
{
  if (roots->size_ == 0)
  {
    roots->roots_array_[0] = root_to_add;
    roots->size_++;
  }
  else
  {
    Person **temp = (Person **)realloc(roots->roots_array_,
                                       (roots->size_ + INCREASE_ROOTS_SIZE_BY) *
                                           sizeof(Person *));
    if (temp == NULL)
    {
      printf(ERROR_MEMORY_MESSAGE);
      return ERROR_MEMORY;
    }
    else
    {
      roots->roots_array_ = temp;
      roots->roots_array_[roots->size_] = root_to_add;
      roots->size_++;
    }
  }
  return SUCCESS;
}

int removeRoot(RootsArray *roots, Person *person_to_remove)
{
  if (roots->size_ == 0)
  {
    // ToDo: use constants
    printf("Root empty!");
    return SUCCESS;
  }
  Person **temp = (Person **)malloc((roots->size_ - 1) * sizeof(Person *));
  if (temp == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }
  int idx_new = 0;
  for (int i = 0; i < roots->size_; i++)
  {
    if (roots->roots_array_[i] != person_to_remove)
    {
      temp[idx_new] = roots->roots_array_[i];
      idx_new++;
    }
  }
  free(roots->roots_array_);
  roots->roots_array_ = temp;
  roots->size_--;
  return SUCCESS;
}

int isInRootsArray(RootsArray *roots, Person *person_to_search)
{
  for (int i = 0; i < roots->size_; i++)
  {
    if (roots->roots_array_[i] == person_to_search)
    {
      return true;
    }
  }
  return false;
}

int getHighestID(RootsArray *roots, int *id)
{
  int highest_id = 0;
  if (roots->size_ == 0)
  {
    *id = highest_id + 1;
    return SUCCESS;
  }
  for (int i = 0; i < roots->size_; i++)
  {
    int bfs_high = 0;
    if (bfsSearchHighest(roots->roots_array_[i], &bfs_high) ==
        ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
    highest_id = (bfs_high > highest_id) ? bfs_high : highest_id;
  }
  *id = highest_id + 1;
  return SUCCESS;
}

int initQueue(Queue **queue, Queue **explored, Person *root)
{

  if (*queue == NULL)
  {
    *queue = (Queue *)malloc(sizeof(Queue));
    if (*queue == NULL)
    {
      printf(ERROR_MEMORY_MESSAGE);
      return ERROR_MEMORY;
    }
    (*queue)->head_ = NULL;
  }
  if (*explored == NULL)
  {
    *explored = (Queue *)malloc(sizeof(Queue));
    if (*explored == NULL)
    {
      free(queue);
      printf(ERROR_MEMORY_MESSAGE);
      return ERROR_MEMORY;
    }
    (*explored)->head_ = NULL;
  }

  if (addToQueue(*explored, root) == ERROR_MEMORY)
  {
    free(queue);
    free(explored);
    return ERROR_MEMORY;
  }
  if (addToQueue(*queue, root) == ERROR_MEMORY)
  {
    freeQueueWithPerson((*explored)->head_);
    free(explored);
    free(queue);

    return ERROR_MEMORY;
  }
  return SUCCESS;
}

int bfsSearchHighest(Person *root, int *bfs_high)
{
  Queue *queue = NULL;
  Queue *explored = NULL;
  if (initQueue(&queue, &explored, root))
  {
    return ERROR_MEMORY;
  }
  // ToDo: remove while(1)
  while (1)
  {
    Person *to_check = getNextInQueue(queue);
    if (to_check == NULL)
    {
      break;
    }
    *bfs_high = (to_check->id_ > *bfs_high) ? to_check->id_ : *bfs_high;
    if (addEdgesToQueue(queue, explored, to_check) == ERROR_MEMORY)
    {
      freeQueue(explored->head_);
      free(explored);
      free(queue);
      return ERROR_MEMORY;
    }
  }
  freeQueue(explored->head_);
  free(explored);
  free(queue);
  return SUCCESS;
}

int bfsSearchPerson(Person *root, Person **person, int id)
{
  if (root == NULL)
  {
    *person = NULL;
    return PERSON_NOT_FOUND;
  }
  int found_person = 0;
  Queue *queue = NULL;
  Queue *explored = NULL;
  if (initQueue(&queue, &explored, root))
  {
    return ERROR_MEMORY;
  }
  // ToDo: remove while(1)
  while (1)
  {
    *person = getNextInQueue(queue);
    if (*person == NULL)
    {
      break;
    }
    if ((*person)->id_ == id)
    {
      found_person = 1;
      while(getNextInQueue(queue) != NULL);
      break;
    }
    if (addEdgesToQueue(queue, explored, *person) == ERROR_MEMORY)
    {
      freeQueue(explored->head_);
      free(explored);
      free(queue);
      return ERROR_MEMORY;
    }
  }
  if (found_person == 0)
  {
    *person = NULL;
  }
  freeQueue(explored->head_);
  free(explored);
  free(queue);
  return (*person == NULL) ? PERSON_NOT_FOUND : SUCCESS;
}

int isInQueue(Queue *queue, Person *person_to_search)
{
  QueueElement *current_el = queue->head_;
  if (current_el == NULL)
  {
    return 0;
  }
  if (current_el->next_ == NULL)
  {
    if (current_el->person_data_ == person_to_search)
    {
      return 1;
    }
    return 0;
  }
  while (current_el != NULL)
  {
    if (current_el->person_data_ == person_to_search)
    {
      return 1;
    }
    current_el = current_el->next_;
  }
  return 0;
}

Person *getNextInQueue(Queue *queue)
{
  if (queue->head_ == NULL)
  {
    return NULL;
  }
  QueueElement *temp_to_return = queue->head_;
  queue->head_ = temp_to_return->next_;
  Person *person_in_queue = temp_to_return->person_data_;
  free(temp_to_return);
  return person_in_queue;
}

int addToQueue(Queue *queue, Person *person_to_add)
{
  if (isInQueue(queue, person_to_add))
  {
    return IN_QUEUE;
  }
  QueueElement *elToAdd = (QueueElement *)malloc(sizeof(QueueElement));
  if (elToAdd == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }
  elToAdd->next_ = NULL;
  elToAdd->person_data_ = person_to_add;
  QueueElement *iterEl = queue->head_;
  if (iterEl == NULL) // Zero Elements
  {
    queue->head_ = elToAdd;
    return SUCCESS;
  }
  if (iterEl->next_ == NULL) // One Element
  {
    iterEl->next_ = elToAdd;
    return SUCCESS;
  }
  while (iterEl->next_ != NULL) // > 1 Elements
  {
    iterEl = iterEl->next_;
  }
  iterEl->next_ = elToAdd;
  return SUCCESS;
}

int addEdgesToQueue(Queue *queue, Queue *explored, Person *to_check)
{
  if (to_check->parent1_ != NULL &&
      !isInQueue(explored, to_check->parent1_)) // Parent One
  {
    if (addToQueue(queue, to_check->parent1_) == ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
    if (addToQueue(explored, to_check->parent1_) == ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
  }
  if (to_check->parent2_ != NULL &&
      !isInQueue(explored, to_check->parent2_)) // Parent Two
  {
    if (addToQueue(queue, to_check->parent2_) == ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
    if (addToQueue(explored, to_check->parent2_) == ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
  }
  if (to_check->children_ != NULL)
  {
    for (int i = 0; i < to_check->children_->size_; i++) // All children
    {
      // ToDo: rename current_child
      Person *curChild = to_check->children_->roots_array_[i];
      if (!isInQueue(explored, curChild))
      {
        if (addToQueue(queue, curChild) == ERROR_MEMORY)
        {
          return ERROR_MEMORY;
        }
        if (addToQueue(explored, curChild) == ERROR_MEMORY)
        {
          return ERROR_MEMORY;
        }
      }
    }
  }
  return SUCCESS;
}

void freeQueueWithPerson(QueueElement *free_from)
{
  if (free_from->next_ != NULL)
  {
    freeQueueWithPerson(free_from->next_);
  }
  freePerson(free_from->person_data_);
  free_from->person_data_ = NULL;
  free(free_from);
  free_from = NULL;
}

void freeQueue(QueueElement *free_from)
{
  if(free_from == NULL)
  {
    return;
  }
  if (free_from->next_ != NULL)
  {
    freeQueue(free_from->next_);
  }
  free(free_from);
}

int initPerson(Person **person_to_init, char *name, int id)
{
  *person_to_init = (Person *)malloc(sizeof(Person));
  if (*person_to_init == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }
  if (createRootsArray(&(*person_to_init)->children_) == ERROR_MEMORY)
  {
    free(*person_to_init);
    return ERROR_MEMORY;
  }
  (*person_to_init)->parent1_ = NULL;
  (*person_to_init)->parent2_ = NULL;
  (*person_to_init)->id_ = id;
  (*person_to_init)->name_ = (char *)malloc(strlen(name) + 1);
  if ((*person_to_init)->name_ == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    free(*person_to_init);
    return ERROR_MEMORY;
  }
  strcpy((*person_to_init)->name_, name);
  return SUCCESS;
}

int getPersonFromId(RootsArray *roots, Person **person, int id)
{
  int i = 0;
  do
  {
    if (bfsSearchPerson(roots->roots_array_[i], person, id) ==
        ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
    i++;
  } while (*person == NULL && i < roots->size_);
  if (*person == NULL)
  {
    // ToDo: Use constants
    printf("Error: ID [%d] does not exist. Use 'list' to find all available "
           "persons.\n\n",
           id);
    return PERSON_NOT_FOUND;
  }
  return SUCCESS;
}

int getPersonFromIdSilent(RootsArray *roots, Person **person, int id)
{
  int i = 0;
  do
  {
    if (bfsSearchPerson(roots->roots_array_[i], person, id) ==
        ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
    i++;
  } while (*person == NULL && i < roots->size_);
  if (*person == NULL)
  {
    return PERSON_NOT_FOUND;
  }
  return SUCCESS;
}

int setParent(Person *child, Person **parent, Person *parent_or_grandparent)
{
  if (*parent != NULL)
  {
    printf(RELATIONSHIP_ALREADY_SET_MESSAGE);
    return RELATIONSHIP_ALREADY_SET;
  }
  *parent = parent_or_grandparent;
  if (addRoot((*parent)->children_, child) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }
  return SUCCESS;
}

int addParent(Person *child, Person *parent)
{
  if(child->parent1_ == NULL)
  {
    return setParent(child, &(child->parent1_), parent);
  }
  else if(child->parent2_ == NULL)
  {
    return setParent(child, &(child->parent2_), parent);
  }
  else
  {
    return TOO_MANY_PARENTS;
  }
}

// ToDo: use constants
int addRelationShip(Person *child, Person *parent_or_grandparent,
                    char *relatioship, RootsArray *roots)
{
  char *temp = malloc(strlen(relatioship) + 1);
  if (temp == NULL)
  {
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }
  strcpy(temp, relatioship);
  char *prefix = strtok(temp, ":");
  char *suffix = strtok(NULL, "");
  if (suffix == NULL)
  {
    if (strcmp(prefix, "parent1") == 0)
    {
      free(temp);
      return setParent(child, &(child->parent1_), parent_or_grandparent);
    }
    if (strcmp(prefix, "parent2") == 0)
    {
      free(temp);
      return setParent(child, &(child->parent2_), parent_or_grandparent);
    }
  }
  if (strcmp(prefix, "parent1") == 0)
  {
    if (child->parent1_ == NULL)
    {
      int next_free_id = 0;
      if (getHighestID(roots, &next_free_id) == ERROR_MEMORY)
      {
        free(temp);
        return ERROR_MEMORY;
      }
      Person* new_unknown_parent = NULL;
      if (initPerson(&new_unknown_parent, "?", next_free_id) ==
          ERROR_MEMORY)
      {
        free(temp);
        return ERROR_MEMORY;
      }
      if(setParent(child, &(child->parent1_), new_unknown_parent) == ERROR_MEMORY)
      {
        free(temp);
        return ERROR_MEMORY;
      }
      printf("Created unknown parent1 [%d]!\n", next_free_id);
    }
    if (strcmp(suffix, "gp1") == 0)
    {
      free(temp);
      return setParent((child->parent1_), &(child->parent1_)->parent1_,
                       parent_or_grandparent);
    }
    if (strcmp(suffix, "gp2") == 0)
    {
      free(temp);
      return setParent((child->parent1_), &(child->parent1_)->parent2_,
                       parent_or_grandparent);
    }
  }
  if (strcmp(prefix, "parent2") == 0)
  {
    if (child->parent2_ == NULL)
    {
      int next_free_id = 0;
      if (getHighestID(roots, &next_free_id) == ERROR_MEMORY)
      {
        free(temp);
        return ERROR_MEMORY;
      }
      Person* new_unknown_parent = NULL;
      if (initPerson(&new_unknown_parent, "?", next_free_id) ==
          ERROR_MEMORY)
      {
        free(temp);
        return ERROR_MEMORY;
      }
      if(setParent(child, &(child->parent2_), new_unknown_parent) == ERROR_MEMORY)
      {
        free(temp);
        return ERROR_MEMORY;
      }
      printf("Created unknown parent2 [%d]!\n", next_free_id);
    }
    if (strcmp(suffix, "gp1") == 0)
    {
      free(temp);
      return setParent((child->parent2_), &(child->parent2_)->parent1_,
                       parent_or_grandparent);
    }
    if (strcmp(suffix, "gp2") == 0)
    {
      free(temp);
      return setParent((child->parent2_), &(child->parent2_)->parent2_,
                       parent_or_grandparent);
    }
  }
  return SUCCESS;
}

int add(RootsArray *roots, char *name)
{
  int next_free_id = 0;
  if (getHighestID(roots, &next_free_id) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }

  Person *person_to_add = NULL;
  if (initPerson(&person_to_add, name, next_free_id) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }
  // ToDo: use constants
  addRoot(roots, person_to_add);
  // ToDo: use constants
  printf(COMMAND_ADD_SUCCESSFULL, person_to_add->name_, person_to_add->id_);
  return SUCCESS;
}

int connect(RootsArray *roots, int id_person_one, char *relationship,
            int id_person_two)
{
  Person *parent_or_grandparent = NULL;
  int error_code =
      getPersonFromId(roots, &parent_or_grandparent, id_person_one);
  if (error_code == ERROR_MEMORY || error_code == PERSON_NOT_FOUND)
  {
    return error_code;
  }
  Person *child = NULL;
  error_code = getPersonFromId(roots, &child, id_person_two);
  if (error_code == ERROR_MEMORY || error_code == PERSON_NOT_FOUND)
  {
    return error_code;
  }
  error_code =
      addRelationShip(child, parent_or_grandparent, relationship, roots);
  if (error_code == ERROR_MEMORY ||
      error_code == RELATIONSHIP_ALREADY_SET)
  {
    return error_code;
  }
  // remove node from roots array
  if (isInRootsArray(roots, child))
  {
    if (removeRoot(roots, child) == ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
  }

  // ToDo: use constants
  printf("Successfully connected %s [%d] as %s of %s [%d]!\n\n",
         parent_or_grandparent->name_, parent_or_grandparent->id_, relationship,
         child->name_, child->id_);
  return SUCCESS;
}

void freePerson(Person *person_to_free)
{
  if (person_to_free->name_ != NULL)
  {
    free(person_to_free->name_);
    person_to_free->name_ = NULL;
  }
  if (person_to_free->children_ != NULL)
  {
    if (person_to_free->children_->roots_array_ != NULL)
    {
      free(person_to_free->children_->roots_array_);
      person_to_free->children_->roots_array_ = NULL;
    }
    free(person_to_free->children_);
    person_to_free->children_ = NULL;
  }
  if (person_to_free != NULL)
  {
    free(person_to_free);
    person_to_free = NULL;
  }
}

int areSiblings(Person *person1, Person *person2)
{
  if ((person1->parent1_ == NULL || person1->parent2_ == NULL))
  {
    return false;
  }
  return (person1->parent1_ == person2->parent1_ || person1->parent1_ == person2->parent2_) &&
         ((person1->parent2_ == person2->parent1_ || person1->parent2_ == person2->parent2_));
}

int isParent(Person *child, Person *parent)
{
  if ((child == NULL || parent == NULL))
  {
    return false;
  }
  return (child->parent1_ == parent || child->parent2_ == parent);
}

int isGrandparent(Person *person1, Person *person2)
{
  return isParent(person1->parent1_, person2) || isParent(person1->parent2_, person2);
}

int hasDirectRelativ(Person person)
{
    return (person.children_ != NULL && person.children_->size_ == 0) 
            && person.parent1_ == NULL 
            && person.parent2_ == NULL;
}

int fillQueuesWithAllPersons(RootsArray *roots, Queue **queue, Queue **explored, int *size)
{
  *size = 0;
  for (int i = 0; i < roots->size_; i++)
  {
    if (initQueue(queue, explored, roots->roots_array_[i]) ==
        ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
    Person *person;
    while ((person = getNextInQueue(*queue)) != NULL)
    {
      *size = *size + 1;
      if (addEdgesToQueue(*queue, *explored, person) == ERROR_MEMORY)
      {
        freeQueueWithPerson((*explored)->head_);
        freeQueueWithPerson((*queue)->head_);
        free(*explored);
        free(*queue);
        return ERROR_MEMORY;
      }
    }
  }
  return SUCCESS;
}

int getArrayOfWholeTree(RootsArray *roots, Person ***persons, int *size)
{
  Queue *queue = NULL;
  Queue *explored = NULL;
  *size = 0;
  if (fillQueuesWithAllPersons(roots, &queue, &explored, size) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }

  *persons = malloc(sizeof(Person*) * (*size));

  if(*persons == NULL)
  {
    freeQueue(queue->head_);
    free(queue);
    freeQueue(explored->head_);
    free(explored);
    printf(ERROR_MEMORY_MESSAGE);
    return ERROR_MEMORY;
  }

  if (*size != 0)
  {
    int new_valid_size = 0;
    for (int i = 0; i < *size; i++)
    {
      Person* next_person = getNextInQueue(explored);
      if(next_person != NULL)
      {
        (*persons)[i] = next_person;
        new_valid_size++;
      }
    }
    qsort(*persons, new_valid_size, sizeof(Person *), comparePerson);
    freeQueue(queue->head_);
    free(queue);
    freeQueue(explored->head_);
    free(explored);
    *size = new_valid_size;
  }
  return SUCCESS;
}

int comparePerson(const void *a, const void *b)
{
    Person *person_a = *(Person **)a; 
    Person *person_b = *(Person **)b;
    return person_a->id_ - person_b->id_;
}

int populateRoot(Person **persons, int *person_count, RootsArray *roots)
{
  for (int i = 0; i < *person_count; i++)
  {
    Person* curr = persons[i];

    if (curr == NULL)
    {
      continue;
    }

    int status_check = isInExistingTree(curr, roots);
    if (status_check == DOES_NOT_EXIST_IN_TREE)
    {
      if (addRoot(roots, curr) == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
    }
    else if (status_check == ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
  }

  return SUCCESS;
}

int isInExistingTree(Person *person, RootsArray *roots)
{
  for (int i = 0; i < roots->size_; i++)
  {
    int status_check = isInTree(roots->roots_array_[i], person);
    if (status_check != DOES_NOT_EXIST_IN_TREE)
    {
      return status_check;
    }
  }

  return DOES_NOT_EXIST_IN_TREE;
}

int isInTree(Person *root, Person *person)
{
  Queue *queue = NULL;
  Queue *explored = NULL;

  if (initQueue(&queue, &explored, root) == ERROR_MEMORY)
  {
    return ERROR_MEMORY;
  }

  while (queue->head_ != NULL)
  {
    Person *curr = getNextInQueue(queue);
    if (curr == NULL)
    {
      break;
    }

    if (curr == person)
    {
      freeQueue(explored->head_);
      free(explored);
      explored = NULL;
      free(queue);
      queue = NULL;
      return EXISTS_IN_TREE;
    }

    if (addEdgesToQueue(queue, explored, curr) == ERROR_MEMORY)
    {
      freeQueue(explored->head_);
      free(explored);
      explored = NULL;
      free(queue);
      queue = NULL;
      return ERROR_MEMORY;
    }
  }

  freeQueue(explored->head_);
  free(explored);
  explored = NULL;
  free(queue);
  queue = NULL;

  return DOES_NOT_EXIST_IN_TREE;
}

int processAncestors(Person* person, Queue* queue, Queue* explored, int *size)
{
  while (1)
  {
    Person *to_check = getNextInQueue(queue);
    if (to_check == NULL)
    {
      break;
    }
    *size = *size + 1;
    if(!isInQueue(explored, person))
    {
      if (addEdgesToQueue(queue, explored, to_check) == ERROR_MEMORY)
      {
        freeQueue(explored->head_);
        free(explored);
        free(queue);
        return ERROR_MEMORY;
      }
    }
    else 
    {
      if (to_check->parent1_ != NULL &&
          !isInQueue(explored, to_check->parent1_)) // Parent One
      {
        if (addToQueue(queue, to_check->parent1_) == ERROR_MEMORY)
        {
          freeQueue(explored->head_);
        free(explored);
        free(queue);
          return ERROR_MEMORY;
        }
        if (addToQueue(explored, to_check->parent1_) == ERROR_MEMORY)
        {
          freeQueue(explored->head_);
        free(explored);
        free(queue);
          return ERROR_MEMORY;
        }
      }
      if (to_check->parent2_ != NULL &&
          !isInQueue(explored, to_check->parent2_)) // Parent Two
      {
        if (addToQueue(queue, to_check->parent2_) == ERROR_MEMORY)
        {
          freeQueue(explored->head_);
        free(explored);
        free(queue);
          return ERROR_MEMORY;
        }
        if (addToQueue(explored, to_check->parent2_) == ERROR_MEMORY)
        {
          freeQueue(explored->head_);
        free(explored);
        free(queue);
          return ERROR_MEMORY;
        }
      }
    }
  }
  return SUCCESS;
}

int pruneTreeRec(Person* person, RootsArray *roots, Queue *explored)
{
  int pruned = NOTHING_TO_PRUNE_CODE;
  if (addToQueue(explored, person) == ERROR_MEMORY)
  {
    freeQueueWithPerson(explored->head_);
    free(explored);
    return ERROR_MEMORY;
  }
  int temp;
  if(person->parent1_ != NULL)
  {
    temp = pruneTreeRec(person->parent1_, roots, explored);
    pruned = (temp == NOTHING_TO_PRUNE_CODE) ? pruned : temp;
  }
  if(person->parent2_ != NULL)
  {
    temp = pruneTreeRec(person->parent2_, roots, explored);
    pruned = (temp == NOTHING_TO_PRUNE_CODE) ? pruned : temp;
  }

  if(!hasParents(person) && strcmp(person->name_, CONST_UNKNOW_STRING) == 0)
  {
    removeFromQueue(explored, person->id_);
    if(pruneNode(roots, &person) == ERROR_MEMORY)
    {
      free(explored);
      return ERROR_MEMORY;
    }
    pruned = PRUNED_SOMETHING;
  }
  else if (person->children_ != NULL)
  {
    for (int i = 0; i < person->children_->size_; i++)
    {
      Person *current_child = person->children_->roots_array_[i];
      if (!isInQueue(explored, current_child))
      {
        int temp;
        temp = pruneTreeRec(current_child, roots, explored);
        pruned = (temp == NOTHING_TO_PRUNE_CODE) ? pruned : temp;
      }
    }
  }

  return pruned;
}

int pruneNode(RootsArray* roots, Person** person_to_prune)
{
  if((*person_to_prune)->children_->size_ != 0)
  {
    if(isInRootsArray(roots, *person_to_prune) )
    {
      if(addChildrenToRoots(roots, (*person_to_prune)->children_) == ERROR_MEMORY)
      {
        return ERROR_MEMORY;
      }
    }
    removeParentFromChildren((*person_to_prune)->children_, *person_to_prune);
  }

  freePerson(*person_to_prune);
  person_to_prune = NULL;
  return SUCCESS;
}

int hasParents(Person* person)
{
  if(person->parent1_ == NULL && person->parent2_ == NULL)
  {
    return 0;
  }
  return 1;
}

int addChildrenToRoots(RootsArray* roots, RootsArray* children)
{
  for(int i = 0; i < children->size_; i++)
  {
    Person* current_child = children->roots_array_[i];
    if(addRoot(roots, current_child) == ERROR_MEMORY)
    {
      return ERROR_MEMORY;
    }
  }
  return SUCCESS;
}

int removeParentFromChildren(RootsArray* children, Person* parent)
{
  for(int i = 0; i < children->size_; i++)
  {
    Person* current_child = children->roots_array_[i];
    if(current_child->parent1_ == parent)
    {
      current_child->parent1_ = NULL;
    }
    else if(current_child->parent2_ == parent)
    {
      current_child->parent2_ = NULL;
    }
  }
  return SUCCESS;
}

int removeFromQueue(Queue* queue, int id)
{
  QueueElement *last = queue->head_;
  QueueElement *current_el = queue->head_;
  if (current_el == NULL)
  {
    return false;
  }
  if (current_el->next_ == NULL)
  {
    if (current_el->person_data_->id_ == id)
    {
      queue->head_ = NULL;
      return true;
    }
    return false;
  }
  while (current_el != NULL)
  {
    if (current_el->person_data_->id_ == id)
    {
      last->next_ = current_el->next_;
      free(current_el);
      return true;
    }
    last = current_el;
    current_el = current_el->next_;
  }
  return false;
}

