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
//                             avtDatasetSource.C                            //
// ************************************************************************* //

#include <avtDatasetSource.h>


// ****************************************************************************
//  Method: avtDatasetSource constructor
//
//  Programmer: Hank Childs
//  Creation:   May 29, 2001
//      
// ****************************************************************************

avtDatasetSource::avtDatasetSource()
{
    dataset = new avtDataset(this);
}


// ****************************************************************************
//  Method: avtDatasetSource destructor
//
//  Purpose:
//      Defines the destructor.  Note: this should not be inlined in the header
//      because it causes problems for certain compilers.
//
//  Programmer: Hank Childs
//  Creation:   February 5, 2004
//
// ****************************************************************************

avtDatasetSource::~avtDatasetSource()
{
    ;
}


// ****************************************************************************
//  Method: avtDatasetSource::GetOutput
//
//  Purpose:
//      Returns the dataset as a base type data object, with ref_ptr type
//      issues resolved.
//
//  Returns:    This source's dataset, typed as a data object.
//
//  Programmer: Hank Childs
//  Creation:   May 29, 2001
//
//  Modifications:
//    Brad Whitlock, Thu Apr 4 14:59:26 PST 2002
//    Changed CopyTo so it is an inline template function.
//
// ****************************************************************************

avtDataObject_p
avtDatasetSource::GetOutput(void)
{
    avtDataObject_p rv;
    CopyTo(rv, dataset);

    return rv;
}


// ****************************************************************************
//  Method: avtDatasetSource::SetOutputDataTree
//
//  Purpose:
//      Uses avtDatasetSource's friend status with avtDataset to set the top
//      level data tree.  This is defined so any of the derived types can take
//      advantage of the base type's friend status.
//
//  Arguments:
//      dt      The avtDataTree object that makes up the output.
//
//  Programmer: Hank Childs
//  Creation:   July 24, 2000
//
// ****************************************************************************

void
avtDatasetSource::SetOutputDataTree(const avtDataTree_p dt)
{
    dataset->SetDataTree(dt);
}


