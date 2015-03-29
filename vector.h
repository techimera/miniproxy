
#ifndef __VECTOR_H
#define __VECTOR_H

typedef struct vector_s *vector_t;
extern vector_t vector_create(void);
extern vector_destroy(vector_t vector);
extern int    vector_append(vector_t vector,void* data,size_t len);
extern int    vector_prepend(vector_t vector,void* data,size_t len);
extern void*  vector_getat(vector_t vector,size_t pos,size_t* len);
extern size_t vector_length(vector_t vector);
extern size_t vector_sizeof(vector_t vector);
#endif // __VECTOR_H
