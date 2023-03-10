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
//                              avtDataObject.C                              //
// ************************************************************************* //

#include <avtDataObject.h>

#include <avtDataObjectSource.h>
#include <avtQueryableSource.h>
#include <avtOriginatingSource.h>
#include <avtWebpage.h>

#include <DebugStream.h>
#include <ImproperUseException.h>
#include <NoInputException.h>

#include <cstring>

// ****************************************************************************
//  Method: avtDataObject constructor
//
//  Arguments:   
//      src      The source for this data object.
//    
//  Programmer: Hank Childs
//  Creation:   May 23, 2001
//
//  Modifications:
//
//    Hank Childs, Thu May 25 16:45:41 PDT 2006
//    Initialize transient.
//
// ****************************************************************************

avtDataObject::avtDataObject(avtDataObjectSource *src)
{
    source = src;
    transient = true;
}


// ****************************************************************************
//  Method: avtDataObject destructor
//
//  Purpose:
//      Defines the destructor.  Note: this should not be inlined in the header
//      because it causes problems for certain compilers.
//
//  Programmer: Hank Childs
//  Creation:   February 5, 2004
//
// ****************************************************************************

avtDataObject::~avtDataObject()
{
    ;
}


// ****************************************************************************
//  Method: avtDataObject::Update
//
//  Purpose:
//      Propagates an Update up the pipeline.
//
//  Arguments:
//      contract   The contract of what data the pipeline should execute.
//
//  Programmer: Hank Childs
//  Creation:   May 23, 2001
//
//  Modifications:
//
//    Hank Childs, Mon Feb  2 09:26:36 PST 2009
//    Store off the contract if this is not a transient data object.
//    (Chose to do this only for non-transients, because those are probably
//     the only ones we'd want it for ... simply trying to save on space.)
//
// ****************************************************************************

bool
avtDataObject::Update(avtContract_p contract)
{
    bool rv = false;
    if (source == NULL)
    {
        //
        // This object has nothing upstream, so no action item.
        //
        rv = false;
    }
    else
    {
        rv = source->Update(contract);
    }

    contractFromPrevExecution = contract;

    return rv;
}


// ****************************************************************************
//  Method: avtDataObject::SetSource
//
//  Purpose:
//      Allows a source object to be re-set for a data object.
//
//  Arguments:
//      src      The new source.
//
//  Programmer:  Hank Childs
//  Creation:    May 23, 2001
//
// ****************************************************************************

void
avtDataObject::SetSource(avtDataObjectSource *src)
{
    source = src;
}


// ****************************************************************************
//  Method: avtDataObject::GetOriginatingSource
//
//  Purpose:
//      Walks up a pipeline and finds the terminating source.  This passes a
//      call from the data object to the next source.
//
//  Returns:    The terminating source object.
//
//  Programmer: Hank Childs
//  Creation:   May 29, 2001
//
// ****************************************************************************

avtOriginatingSource *
avtDataObject::GetOriginatingSource(void)
{
    if (source == NULL)
    {
        EXCEPTION0(NoInputException);
    }

    return source->GetOriginatingSource();
}


// ****************************************************************************
//  Method: avtDataObject::GetQueryableSource
//
//  Purpose:
//      Walks up a pipeline and finds the queryable source.  This passes a
//      call from the data object to the next source.
//
//  Returns:    The queryable source object.
//
//  Programmer: Hank Childs
//  Creation:   July 28, 2003
//
// ****************************************************************************

avtQueryableSource *
avtDataObject::GetQueryableSource(void)
{
    if (source == NULL)
    {
        EXCEPTION0(NoInputException);
    }

    return source->GetQueryableSource();
}


// ****************************************************************************
//  Method: avtDataObject::Clone
//
//  Purpose:
//      Creates a new instance and copies this data object into it.
//
//  Programmer:  Hank Childs
//  Creation:    July 24, 2001
//
// ****************************************************************************

avtDataObject *
avtDataObject::Clone(void)
{
    avtDataObject *rv = Instance();
    rv->Copy(this);
    return rv;
}


// ****************************************************************************
//  Method: avtDataObject::Copy
//
//  Purpose:
//      Copies the argument to this object.
//
//  Arguments:
//      dob      The data object to copy.
//
//  Programmer:  Hank Childs
//  Creation:    July 24, 2001
//
// ****************************************************************************

void
avtDataObject::Copy(avtDataObject *dob)
{
    //
    // Calling CompatibleTypes will handle it if the types don't match.
    //
    CompatibleTypes(dob);

    info.Copy(dob->GetInfo());
    DerivedCopy(dob);
}


// ****************************************************************************
//  Method: avtDataObject::Merge
//
//  Purpose:
//      Merges the argument into this object.
//
//  Arguments:
//      dob      The data object to copy.
//      blind    Whether or not we should do a "blind" merge -- where we don't
//               check to see if the attributes match up.
//
//  Programmer:  Hank Childs
//  Creation:    July 24, 2001
//
//  Modifications:
//
//    Hank Childs, Mon May 27 12:23:27 PDT 2002
//    Add argument for whether we should merge the information.
//
// ****************************************************************************

void
avtDataObject::Merge(avtDataObject *dob, bool blind)
{
    //
    // Calling CompatibleTypes will handle it if the types don't match.
    //
    CompatibleTypes(dob);

    if (!blind)
    {
        info.Merge(dob->GetInfo());
    }
    DerivedMerge(dob);
}


// ****************************************************************************
//  Method: avtDataObject::Instance
//
//  Purpose:
//      Creates a instance of a data object, properly typed (ie the concrete
//      type).
//
//  Returns:     A new instance of the data object.
//
//  Programmer:  Hank Childs
//  Creation:    July 24, 2001
//
// ****************************************************************************

avtDataObject *
avtDataObject::Instance(void)
{
    EXCEPTION0(ImproperUseException);
}


// ****************************************************************************
//  Method: avtDataObject::DerivedCopy
//
//  Purpose:
//      Does the work of copying the data that applies to the derived class.
//      This does not apply to this data object, but is defined so all derived
//      types don't have to define it.
//
//  Arguments:
//      <unnamed>   The data object to copy.
//
//  Programmer: Hank Childs
//  Creation:   July 24, 2001
//
// ****************************************************************************

void
avtDataObject::DerivedCopy(avtDataObject *)
{
    EXCEPTION0(ImproperUseException);
}


// ****************************************************************************
//  Method: avtDataObject::DerivedMerge
//
//  Purpose:
//      Does the work of merging the data that applies to the derived class.
//      This does not apply to this data object, but is defined so all derived
//      types don't have to define it.
//
//  Arguments:
//      <unnamed>   The data object to merge.
//
//  Programmer: Hank Childs
//  Creation:   July 24, 2001
//
// ****************************************************************************

void
avtDataObject::DerivedMerge(avtDataObject *)
{
    EXCEPTION0(ImproperUseException);
}


// ****************************************************************************
//  Method: avtDataObject::CompatibleTypes
//
//  Purpose:
//      Determines if this data object is compatible with the argument.  Will
//      throw an exception if there is a problem, so there is no need for
//      a return value.
//
//  Arguments:
//      dob       The data object to compare to.
//
//  Programmer:   Hank Childs
//  Creation:     July 24, 2001
//
// ****************************************************************************

void
avtDataObject::CompatibleTypes(avtDataObject *dob)
{
    if (strcmp(GetType(), dob->GetType()) != 0)
    {
        EXCEPTION0(ImproperUseException);
    }
}


// ****************************************************************************
//  Method: avtDataObject::InstantiateWriter
//
//  Purpose:
//      Meant to instantiate a writer.  This just throws an exception, so the
//      derived types that don't have writers don't need to worry about this.
//
//  Programmer: Hank Childs
//  Creation:   October 1, 2001
//
// ****************************************************************************

avtDataObjectWriter *
avtDataObject::InstantiateWriter(void)
{
    EXCEPTION0(ImproperUseException);
}


// ****************************************************************************
//  Method: avtDataObject::ReleaseData
//
//  Purpose:
//      Meant to release any data associated with the data object.  This is
//      defined for derived types where it is not appropriate.
//
//  Programmer: Hank Childs
//  Creation:   November 5, 2001
//
// ****************************************************************************

void
avtDataObject::ReleaseData(void)
{
    debug1 << "avtDataObject::ReleaseData was not re-defined by type "
           << GetType() << endl;
}


// ****************************************************************************
//  Method: avtDataObject::DebugDump
//
//  Purpose:
//      Writes out its information to a webpage.
//
//  Programmer: Hank Childs
//  Creation:   December 21, 2006
//
//  Modifications:
//
//    Hank Childs, Wed Jan 17 09:50:11 PST 2007
//    Add transient to dump.
//
// ****************************************************************************

void
avtDataObject::DebugDump(avtWebpage *webpage, const char *prefix)
{
    if (transient)
    {
        webpage->AddHeading("This data object is transient");
        webpage->AddSubheading("(It will be deleted after the "
                               "filter is executed.)");
    }
    else
    {
        webpage->AddHeading("This data object is not transient");
        webpage->AddSubheading("(It will not be deleted after the "
                               "filter is executed ... probably so that "
                               "queries can be performed against it.)");
    }
    info.DebugDump(webpage);
}


// ****************************************************************************
//  Method: avtDataObject::ResetAllExtents
//
//  Purpose:
//      Propagates a ResetAllExtents up the pipeline.
//
//  Programmer: Kathleen Biagas
//  Creation:   June 5, 2017
//
//  Modifications:
//
// ****************************************************************************

void
avtDataObject::ResetAllExtents()
{
    info.GetAttributes().ResetAllExtents();
    if (source != NULL)
    {
        source->ResetAllExtents();
    }
}
