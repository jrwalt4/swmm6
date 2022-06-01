#ifndef SWMM_H
#define SWMM_H
#define SWMM6_EXPORT __attribute__((visibility("default")))

#if __cplusplus
#define EXTERN_C extern "C"
#define EXTERN_C_OPEN extern "C" {
#define EXTERN_C_CLOSE }
#else
#define EXTERN_C
#define EXTERN_C_OPEN
#define EXTERN_C_CLOSE
#endif

EXTERN_C_OPEN

typedef int swmm6_uid;

typedef struct swmm6 swmm6;
typedef struct swmm6_node swmm6_node;
typedef struct swmm6_link swmm6_link;

SWMM6_EXPORT
int swmm6_open(const char* input, swmm6** pPrj);

typedef struct swmm6_simulation swmm6_simulation;

SWMM6_EXPORT
int swmm6_open_simulation(const char* scenario, swmm6* prj, swmm6_simulation** pSim);

SWMM6_EXPORT
int swmm6_step(swmm6_simulation* sim);

SWMM6_EXPORT
int swmm6_finish(swmm6_simulation* sim);

SWMM6_EXPORT
int swmm6_close(swmm6* prj);

EXTERN_C_CLOSE

#define SWMM_NOID    -1
#define SWMM_OK       0
#define SWMM_ERROR    1
#define SWMM_ABORT    2
#define SWMM_NOTFOUND 3
#define SWMM_ROW      4
#define SWMM_DONE     5
#define SWMM_NOIMPL   6
#define SWMM_NOMEM    7
#define SWMM_NEXT     8
#define SWMM_NOPRV    9

#endif // SWMM_H
