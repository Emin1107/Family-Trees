#ifndef TREE_H
#define TREE_H

#include <stdio.h>

// ToDo: Combine in one header file
// Messages
#define COMMAND_ADD_SUCCESSFULL "Successfully created %s [%d]!\n\n"

#define RELATIONSHIP_ALREADY_SET 44
#define RELATIONSHIP_ALREADY_SET_MESSAGE "Error: Relationship already set.\n\n"

// Constants
#define INCREASE_ROOTS_SIZE_BY 1

typedef struct _Person_
{
  char *name_;
  int id_;
  struct _Person_ *parent1_;
  struct _Person_ *parent2_;
  struct _RootsArray_ *children_;
} Person;

typedef struct _RootsArray_
{
  struct _Person_ **roots_array_;
  int size_;
} RootsArray;

typedef struct _QueueElement_
{
  struct _QueueElement_ *next_;
  struct _Person_ *person_data_;
} QueueElement;

typedef struct _Queue_
{
  struct _QueueElement_ *head_;
} Queue;

// General Tree Func
int createRootsArray(RootsArray **roots);
void freeRootsArray(RootsArray **roots);
int freeTree(RootsArray **roots);
int addRoot(RootsArray *roots, Person *root_to_add);
int removeRoot(RootsArray *roots, Person *person_to_remove);
int isInRootsArray(RootsArray *roots, Person *person_to_search);
int getHighestID(RootsArray *roots, int *id);
int initQueue(Queue **queue, Queue **explored, Person *root);
int bfsSearchHighest(Person *root, int *bfs_high);
int bfsSearchPerson(Person *root, Person **person, int id);
int initPerson(Person **person_to_init, char *name, int id);
int getPersonFromId(RootsArray *roots, Person **person, int id);
int getPersonFromIdSilent(RootsArray *roots, Person **person, int id);
int addRelationShip(Person *child, Person *parent_or_grandparent, char *relatioship, RootsArray *roots);
int setParent(Person *child, Person **parent, Person *parent_or_grandparent);
int addParent(Person *child, Person *parent);
int setChild(Person *child, Person *parent);
int getArrayOfWholeTree(RootsArray *roots, Person ***persons, int *size);

// Queue functions
int isInQueue(Queue *queue, Person *person_to_search);
Person *getNextInQueue(Queue *queue);
int addToQueue(Queue *queue, Person *person_to_add);
int addEdgesToQueue(Queue *queue, Queue *explored, Person *to_check);
void freeQueue(QueueElement *free_from);
void freeQueueWithPerson(QueueElement *free_from);
int fillQueuesWithAllPersons(RootsArray *roots, Queue **queue, Queue **explored, int *size);
int removeFromQueue(Queue* queue, int id);

// Commands
int add(RootsArray *roots, char *name);
int connect(RootsArray *roots, int id_person_one, char *relationship, int id_person_two);
int populateRoot(Person **persons, int *person_count, RootsArray *roots);
int isInExistingTree(Person *person, RootsArray *roots);
int isInTree(Person *root, Person *person);
int pruneTreeRec(Person* person, RootsArray *roots, Queue *explored);
int pruneNode(RootsArray* roots, Person** person_to_prune);

// Person
void freePerson(Person *person_to_free);
int areSiblings(Person *person1, Person *person2);
int isParent(Person *person1, Person *person2);
int isGrandparent(Person *person1, Person *person2);
int comparePerson(const void *a, const void *b);
int hasDirectRelativ(Person person);
int processAncestors(Person* person, Queue* queue, Queue* explored, int *size);
int hasParents(Person* person);
int addChildrenToRoots(RootsArray* roots, RootsArray* children);
int removeParentFromChildren(RootsArray* children, Person* parent);

#endif
