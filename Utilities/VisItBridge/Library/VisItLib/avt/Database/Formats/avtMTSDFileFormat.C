/*****************************************************************************
*
* Copyright (c) 2000 - 2018, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-442911
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

// ************************************************************************* //
//                            avtMTSDFileFormat.C                            //
// ************************************************************************* //
#include <limits.h>

#include <snprintf.h>

#include <avtDatabaseMetaData.h>
#include <avtMTSDFileFormat.h>

#include <DBYieldedNoDataException.h>
#include <ImproperUseException.h>
#include <InvalidFilesException.h>

#include <cstring>

using     std::vector;

// Initialize static values.
const int avtMTSDFileFormat::MAX_FILES = 1000;


// ****************************************************************************
//  Method: avtMTSDFileFormat constructor
//
//  Arguments:
//      names    The file names.
//      nNames   The number of names.
//
//  Programmer:  Hank Childs
//  Creation:    October 8, 2001
//
//  Modifications:
//
//    Hank Childs, Mon Aug 16 16:22:56 PDT 2004
//    Initialize myDomain.
//
//    Hank Childs, Sun May  9 18:47:06 CDT 2010
//    Initialize time slice offset.
//
//    Hank Childs, Tue Apr 10 15:12:58 PDT 2012
//    Initialize read all cycles and times.
//
// ****************************************************************************

avtMTSDFileFormat::avtMTSDFileFormat(const char * const *names, int nNames)
{
    readAllCyclesAndTimes = false;
    nFiles = nNames;
    filenames = new char*[MAX_FILES];
    int  i;
    for (i = 0 ; i < nFiles ; i++)
    {
        filenames[i] = new char[strlen(names[i])+1];
        strcpy(filenames[i], names[i]);
    }
    for (i = nFiles ; i < MAX_FILES ; i++)
    {
        filenames[i] = NULL;
    }
    myDomain = -1;
    timeSliceOffset = 0;
}


// ****************************************************************************
//  Method: avtMTSDFileFormat destructor
//
//  Programmer: Hank Childs
//  Creation:   October 8, 2001
//
// ****************************************************************************

avtMTSDFileFormat::~avtMTSDFileFormat()
{
    if (filenames != NULL)
    {
        for (int i = 0 ; i < nFiles ; i++)
        {
            if (filenames[i] != NULL)
            {
                delete [] filenames[i];
                filenames[i] = NULL;
            }
        }
        delete [] filenames;
        filenames = NULL;
    }
}


// ****************************************************************************
//  Method: avtMTSDFileFormat::AddFile
//
//  Purpose:
//      Adds a file to the list of files this manages.
//
//  Arguments:
//      fname     The name of the new file.
//
//  Programmer:   Hank Childs
//  Creation:     October 8, 2001
//
// ****************************************************************************

int
avtMTSDFileFormat::AddFile(const char *fname)
{
    if (nFiles + 1 >= MAX_FILES)
    {
        //
        // We have run out of room, flag the first file as being bad since it
        // is most likely the table of contents.
        //
        EXCEPTION1(InvalidFilesException, fname);
    }

    filenames[nFiles] = new char[strlen(fname)+1];
    strcpy(filenames[nFiles], fname);
    nFiles++;

    //
    // nFiles-1 is the index of the file we just added, so return that.
    //
    return (nFiles-1);
}

// ****************************************************************************
//  Method: avtMTSDFileFormat::GetNTimesteps
//
//  Purpose:
//      Gets the number of timesteps.
//
//  Returns:     The number of timesteps.
//
//  Programmer:  Hank Childs
//  Creation:    October 8, 2001
//
// ****************************************************************************

int
avtMTSDFileFormat::GetNTimesteps(void)
{
    return 1;
}


// ****************************************************************************
//  Method: avtMTSDFileFormat::GetAuxiliaryData
//
//  Purpose:
//      Gets the auxiliary data specified.
//
//  Arguments:
//      <unnamed>  The variable of interest.
//      <unnamed>  The timestep of interest.
//      <unnamed>  The type of auxiliary data.
//      <unnamed>  The arguments for that type.
//
//  Returns:    The auxiliary data.  Throws an exception if this is not a
//              supported data type.
//
//  Programmer: Hank Childs
//  Creation:   October 8, 2001
//
// ****************************************************************************

void *
avtMTSDFileFormat::GetAuxiliaryData(const char *, int, const char *, void *,
                                    DestructorFunction &)
{
    //
    // This is defined only so the simple file formats that have no auxiliary
    // data don't have to define this.
    //
    return NULL;
}


// ****************************************************************************
//  Method: avtMTSDFileFormat::GetVectorVar
//
//  Purpose:
//      Gets a vector variable.  This is defined in the base class to throw
//      an exception, so formats that don't have vectors don't have to worry
//      about this.
//
//  Arguments:
//      <unnamed>    The timestep of the vector variable.
//      <unnamed>    The name of the vector variable.
//
//  Returns:     Normally a vector var, this actually throws an exception.
//
//  Programmer:  Hank Childs
//  Creation:    March 19, 2001
//
//  Modifications:
//    Kathleen Bonnell, Fri Feb  8 11:03:49 PST 2002
//    vtkVectors has been deprecated in VTK 4.0, use vtkDataArray instead.
//
// ****************************************************************************

vtkDataArray *
avtMTSDFileFormat::GetVectorVar(int, const char *)
{
    EXCEPTION0(ImproperUseException);
}

// ****************************************************************************
//  Method: avtMTSDFileFormat::PopulateDatabaseMetaData
//
//  Purpose: Provide a default implementation for the non-time-qualified
//  request to populate metadata. We do this so the time-qualified version can
//  distinguish between our (bad) default implementation and a plugin's real
//  implementation.
//
//  Ordinarilly, we'd simply use pure virtual functions to enforce which
//  PopulateDatabaseMetaData method should be implemented. However, since
//  many plugins pre-date these changes, we decided NOT to use pure-virtual.
//
//  Programmer:  Mark C. Miller 
//  Creation:    April 7, 2005
//
// ****************************************************************************

void
avtMTSDFileFormat::PopulateDatabaseMetaData(avtDatabaseMetaData *md)
{
    //
    // Do something bogus to the metadata that we would never expect a 
    // real plugin to do so that we can distinguish this method it later
    //
    md->SetNumStates(-INT_MAX);
}

// ****************************************************************************
//  Method: avtMTSDFileFormat::PopulateDatabaseMetaData
//
//  Purpose: Provide a default implementation for the time-qualified
//  request to populate metadata. This just turns around and calls the
//  non-time-qualified method. And, typically, that method has been overridden
//  by an MTXX plugin. However, it may not have been and we can catch that
//  error here, at least at run time, and do something about it.
//
//  Programmer:  Mark C. Miller 
//  Creation:    April 7, 2005
//
// ****************************************************************************
void
avtMTSDFileFormat::PopulateDatabaseMetaData(avtDatabaseMetaData *md, int ts)
{
    //
    // Most plugins pre-dating the existence of a time qualified request to 
    // populate database metadata implemented only this method.
    //
    PopulateDatabaseMetaData(md);

    //
    // If we're here, then an MTXX plugin has not overridden this method.
    // However, a plugin may have overridden PopulateDatabaseMetaData(md). On
    // the other hand we don't know for sure if that is indeed the case. So,
    // we look for the bogus change our default PopulateDatabaseMetaData(md)
    // would have resulted in and throw an exception if we see it.
    //
    if (md->GetNumStates() == -INT_MAX)
    {
        md->SetNumStates(0);
        char msg[512];
        SNPRINTF(msg, sizeof(msg), "It looks like the \"%s\" database plugin "
            "did not implement either of the PopulateDatabaseMetaData methods. "
            "Please contact the plugin developer. This error cannot be corrected "
            "without changes to code", GetType());
        EXCEPTION1(ImproperUseException, msg);
    }
}

// ****************************************************************************
//  Method: avtSTMDFileFormat::SetDatabaseMetaData
//
//  Programmer:  Mark C. Miller
//  Creation:    28Oct10
//
//  Modifications:
//    Mark C. Miller, Mon Nov  1 12:19:02 PDT 2010
//    Remove strict mode test.
//
//    Mark C. Miller, Mon Nov  8 06:53:26 PST 2010
//    Predicate on whether this is a simulation or not.
// ****************************************************************************

void
avtMTSDFileFormat::SetDatabaseMetaData(avtDatabaseMetaData *md, int ts)
{
    metadata = md;
    PopulateDatabaseMetaData(metadata, ts);
    if ((!metadata->GetIsSimulation()) && metadata->Empty())
    {
        EXCEPTION1(DBYieldedNoDataException, filenames[0]);
    }
}
