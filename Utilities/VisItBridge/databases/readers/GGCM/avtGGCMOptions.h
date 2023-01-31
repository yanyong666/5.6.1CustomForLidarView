// ************************************************************************* //
//                             avtGGCMOptions.h                              //
// ************************************************************************* //

#ifndef AVT_GGCM_OPTIONS_H
#define AVT_GGCM_OPTIONS_H

class DBOptionsAttributes;

#include <string>


// ****************************************************************************
//  Functions: avtGGCMOptions
//
//  Purpose:
//      Creates the options for  GGCM readers and/or writers.
//
//  Programmer: tfogal -- generated by xml2avt
//  Creation:   Fri Jun 2 10:13:25 PDT 2006
//
// ****************************************************************************

DBOptionsAttributes *GetGGCMReadOptions(void);
DBOptionsAttributes *GetGGCMWriteOptions(void);


#endif
