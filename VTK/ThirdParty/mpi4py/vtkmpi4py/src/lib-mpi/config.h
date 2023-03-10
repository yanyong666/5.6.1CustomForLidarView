#if defined(MS_WINDOWS)
#  if !defined(MSMPI_VER) && (defined(MPICH2) && defined(MPIAPI))
#    define MSMPI_VER 0x100
#  endif
#endif

#if defined(MPT_VERSION)
#  define SGI_MPT
#endif

#if defined(MPICH_NAME) && (MPICH_NAME==3)
#  define MPICH3 1
#endif
#if defined(MPICH_NAME) && (MPICH_NAME==1)
  #define MPICH1 1
#endif

#if !defined(MPIAPI)
#  define MPIAPI
#endif

#if defined(HAVE_CONFIG_H)
#include "config/config.h"
#elif defined(MSMPI_VER)
#include "config/msmpi.h"
#elif defined(SGI_MPT)
#include "config/sgi-mpt.h"
#elif defined(MPICH3)
#include "config/mpich3.h"
#elif defined(MPICH2)
#include "config/mpich2.h"
#elif defined(OPEN_MPI)
#include "config/openmpi.h"
#else /* Unknown MPI*/
#include "config/unknown.h"
#endif

#ifdef PyMPI_MISSING_MPI_Type_create_f90_integer
#undef PyMPI_HAVE_MPI_Type_create_f90_integer
#endif

#ifdef PyMPI_MISSING_MPI_Type_create_f90_real
#undef PyMPI_HAVE_MPI_Type_create_f90_real
#endif

#ifdef PyMPI_MISSING_MPI_Type_create_f90_complex
#undef PyMPI_HAVE_MPI_Type_create_f90_complex
#endif

#ifdef PyMPI_MISSING_MPI_Status_c2f
#undef PyMPI_HAVE_MPI_Status_c2f
#endif

#ifdef PyMPI_MISSING_MPI_Status_f2c
#undef PyMPI_HAVE_MPI_Status_f2c
#endif
