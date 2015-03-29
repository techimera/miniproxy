
#include "vector.h"


struct vector_entry_s
{
  void* data;
  size_t len;
  struct vector_entry_s* next;
};

typedef vector_entry_s *vector_entry_t;

struct vector_s
{
  struct vector_entry_t head;
  struct vector_entry_t tail;
  size_t num_entries;
};


vector_t vector_create(void);
{
  vector_t vector;
  vector = (vector_t) malloc(sizeof(struct vector_s));

  if ( !vector)
  {
    printf("%s\n", "failed to allocate memory for vector");
    return NULL;
  }

  vector->head = NULL;
  vector->tail = NULL;
  vector->num_entries = 0;
  return vector;
}

void vector_destroy(vector_t vector)
{
  vector_entry_t head = vector->head;
  vector_entry_t next;

  while (head)
  {
    next = head->next;
    free(head->data);
    free(head);
    head = next;
  }

}

int vector_append(vector_t vector, void* data, size_t len)
{
  vector_entry_t node = malloc(sizeof(vector_entry_s));
  node->len = 0;
  node->next = NULL;

}

int vector_prepend(vector_t vector, void* data, size_t len) {

}
void*  vector_getat(vector_t vector, size_t pos, size_t* len) {

}

// returns number of entries in the vector
size_t vector_length(vector_t vector)
{
  if (!vector)
  {
    return -1;
  }
  return vector->num_entries;
}

// return the total size of all entries in the vector in bytes
size_t vector_sizeof(vector_t vector)
{
  size_t sz = 0;
  if (!vector)
  {
    return 0;
  }
  vector_entry_t head = vector->head;
  for (; head != NULL ; head = head->next)
  {
    sz += head->len;
  }
  return sz;
}




