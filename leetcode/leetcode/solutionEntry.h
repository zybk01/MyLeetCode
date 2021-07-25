#ifndef SOLUTION_ENTRY
#define SOLUTION_ENTRY

typedef void *pHandle;
typedef void **pData;

struct solutionEntry;

typedef void (*EntryFunc)(solutionEntry &entry);

struct solutionEntry
{
    /* data */
    void (*create)(pHandle& out);
    void (*solve)(pHandle handle,pData dataptr,void *out);
    void (*destroy)(pHandle handle);
};



#endif