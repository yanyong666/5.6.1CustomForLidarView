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
//                               avtImageSink.C                              //
// ************************************************************************* //

#include <avtImageSink.h>

#include <DebugStream.h>
#include <ImproperUseException.h>

#include <cstring>

// ****************************************************************************
//  Method: avtImageSink constructor
//
//  Programmer: Hank Childs
//  Creation:   May 29, 2001
//
// ****************************************************************************

avtImageSink::avtImageSink()
{
    input = NULL;
}


// ****************************************************************************
//  Method: avtImageSink destructor
//
//  Purpose:
//      Defines the destructor.  Note: this should not be inlined in the header
//      because it causes problems for certain compilers.
//
//  Programmer: Hank Childs
//  Creation:   February 5, 2004
//
// ****************************************************************************

avtImageSink::~avtImageSink()
{
    ;
}


// ****************************************************************************
//  Method: avtImageSink::SetTypedInput
//
//  Purpose:
//      Sets the input of the sink and performs some type checking.
//
//  Arguments:
//      in      The image as a data object.
//
//  Programmer: Hank Childs
//  Creation:   May 29, 2001
//
//  Modifications:
//
//    Brad Whitlock, Thu Apr 4 14:57:22 PST 2002
//    Changed CopyTo to an inline template function.
//
//    Hank Childs, Tue Sep 10 09:02:17 PDT 2002
//    Do not assume that an input is non-NULL.
//
// ****************************************************************************

void
avtImageSink::SetTypedInput(avtDataObject_p in)
{
    if (*in != NULL && strcmp(in->GetType(), "avtImage") != 0)
    {
        //
        // Should create a new exception here, but I'm under time constraints.
        //
        debug1 << "Looking for avtImage, but found type \""
               << in->GetType() << "\"." << endl;
        EXCEPTION0(ImproperUseException);
    }

    CopyTo(input, in);
}


// ****************************************************************************
//  Method: avtImageSink::GetInput
//
//  Purpose:
//      Gets the input of the sink (properly typed as an avtDataObject).
//
//  Returns:    The input of the sink.
//
//  Programmer: Hank Childs
//  Creation:   May 29, 2001
//
//  Modifications:
//    Brad Whitlock, Thu Apr 4 14:57:22 PST 2002
//    Changed CopyTo to an inline template function.
//
// ****************************************************************************

avtDataObject_p
avtImageSink::GetInput(void)
{
    avtDataObject_p rv;
    CopyTo(rv, input);
    return rv;
}

// ****************************************************************************
//  Method: avtImageSink::GetInput
//
//  Purpose:
//      Gets the input of the sink (properly typed as an avtDataObject).
//
//  Returns:    The input of the sink.
//
//  Programmer: Tom Fogal
//  Creation:   June 23, 2009
//
// ****************************************************************************

const avtDataObject_p
avtImageSink::GetInput(void) const
{
    avtDataObject_p rv;
    CopyTo(rv, input);
    return rv;
}

// ****************************************************************************
//  Method: avtImageSink::GetImageRep
//
//  Purpose:
//      Gets the image representation.
//
//  Returns:     The representation of an image.
//
//  Programmer:  Hank Childs
//  Creation:    June 20, 2001
//
// ****************************************************************************

avtImageRepresentation &
avtImageSink::GetImageRep(void)
{
    return input->GetImage();
}


