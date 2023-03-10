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
//                       avtDataObjectToDatasetFilter.C                      //
// ************************************************************************* //

#include <snprintf.h>

#include <avtDataObjectToDatasetFilter.h>

#include <avtCommonDataFunctions.h>

#include <DebugStream.h>
#include <TimingsManager.h>

bool avtDataObjectToDatasetFilter::vtkDebugMode = false;

// ****************************************************************************
//  Method: avtDataObjectToDatasetFilter constructor
//
//  Purpose:
//      Defines the constructor.  Note: this should not be inlined in the
//      header because it causes problems for certain compilers.
//
//  Programmer: Hank Childs
//  Creation:   February 5, 2004
//
// ****************************************************************************

avtDataObjectToDatasetFilter::avtDataObjectToDatasetFilter()
{
    ;
}


// ****************************************************************************
//  Method: avtDataObjectToDatasetFilter destructor
//
//  Purpose:
//      Defines the destructor.  Note: this should not be inlined in the header
//      because it causes problems for certain compilers.
//
//  Programmer: Hank Childs
//  Creation:   February 5, 2004
//
// ****************************************************************************

avtDataObjectToDatasetFilter::~avtDataObjectToDatasetFilter()
{
    ;
}


// ****************************************************************************
//  Method: avtDataObjectToDatasetFilter::OutputSetActiveVariable
//
//  Purpose:
//      Sets the active variable in the output.
//
//  Arguments:
//      varname    The name of the new active variable.
//
//  Notes:     The method name contains "output" to prevent name collisions in
//             multiply inheriting derived types.  The compilers should be
//             able to handle this when given a scope resolution operator, but
//             I don't trust them.
//
//  Programmer: Hank Childs
//  Creation:   October 24, 2001
//
//  Modifications:
//
//    Hank Childs, Sat Dec 13 16:07:44 PST 2003
//    Set the dimension of the output variable.
//
//    Hank Childs, Sun Dec 14 14:15:02 PST 2003
//    Initialize the dimension for the cases of non-variables (like meshes and
//    subsets).
//
//    Mark C. Miller, Thu Jan 22 22:13:19 PST 2004
//    Initialized activeVarDim to -1 and success to false before call to
//    Traverse and protected call to SetVariableDimension with check for
//    success==true and activeVarDim!=-1
//
//    Hank Childs, Fri Feb 20 15:14:32 PST 2004
//    Use the new data attributes method to set the active variable.
//    Remove logic dealing with setting variable dimension -- it is now
//    managed correctly without pseudo-logic.
//
// ****************************************************************************

void
avtDataObjectToDatasetFilter::OutputSetActiveVariable(const char *varname)
{
    SetActiveVariableArgs args;
    args.varname = varname;
    args.activeVarDim = -1;
    avtDataTree_p tree = GetDataTree();
    bool success = false;
    tree->Traverse(CSetActiveVariable, (void *) &args, success);

    GetOutput()->GetInfo().GetAttributes().SetActiveVariable(varname);
}


// ****************************************************************************
//  Method: avtDataObjectToDatasetFilter::PostExecute
//
//  Purpose:
//      Walks through the data and determines if we have polygonal data stored
//      as an unstructured grid.
//
//  Programmer: Hank Childs
//  Creation:   July 27, 2004
//
//  Modifications:
//
//    Hank Childs, Tue Jan 16 11:24:53 PST 2007
//    Break all VTK pipeline connections.
//
//    Cyrus Harrison, Sat Aug 11 19:48:30 PDT 2007
//    Add support for vtk-debug mode.
//
//    Kathleen Biagas, Thu Apr 26 13:57:42 PDT 2012
//    Add debug statement to aid in debugging new filters that inadvertently
//    trigger the transformation.
//
// ****************************************************************************

void
avtDataObjectToDatasetFilter::PostExecute(void)
{
    avtDataTree_p tree = GetDataTree();
    bool dummy;

    avtDataAttributes &atts = GetOutput()->GetInfo().GetAttributes();
    if ((atts.GetSpatialDimension()==3 && atts.GetTopologicalDimension()<3) ||
        (atts.GetSpatialDimension()==2 && atts.GetTopologicalDimension()<2))
    {
        char dimDataStr[32];
        debug3 << "avtDataObjectToDatastFilter converting ugrids to polydata "
               << "in postex." << endl;
        int t0 = visitTimer->StartTimer();
        SNPRINTF(dimDataStr, sizeof(dimDataStr),
            "avtTopoDim=%d", atts.GetTopologicalDimension());
        tree->Traverse(CConvertUnstructuredGridToPolyData, dimDataStr, dummy);
        visitTimer->StopTimer(t0, "converting ugrids to polydata in postex");
    }

    int t0 = visitTimer->StartTimer();
    tree->Traverse(CBreakVTKPipelineConnections, (void*)&vtkDebugMode, dummy);
    visitTimer->StopTimer(t0, "Breaking pipeline connections in postex");
}


