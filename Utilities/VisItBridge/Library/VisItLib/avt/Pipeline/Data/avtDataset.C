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
//                                avtDataset.C                               //
// ************************************************************************* //


#include <avtDataset.h>

#include <float.h>

#include <vtkDataSet.h>

#include <avtCommonDataFunctions.h>
#include <avtCompactTreeFilter.h>
#include <avtDataRepresentation.h>
#include <avtDataSetWriter.h>
#include <avtIntervalTree.h>
#include <avtParallel.h>
#include <avtSourceFromAVTDataset.h>
#include <avtWebpage.h>

#include <DebugStream.h>

#include <string>
#include <vector>

using std::string;
using std::vector;


// ****************************************************************************
//  Method: avtDataset constructor
//
//  Arguments:
//      src     The upstream object for this dataset.
//
//  Programmer: Hank Childs
//  Creation:   July 24, 2000
//
//  Modifications:
//
//    Hank Childs, Tue May 22 20:56:19 PDT 2001
//    Rewrote to account for new base type.  Blew away antiquated comments.
//
// ****************************************************************************

avtDataset::avtDataset(avtDataObjectSource *src)
    : avtDataObject(src)
{
    dataTree = new avtDataTree();
}


// ****************************************************************************
//  Method: avtDataset constructor
//
//  Arguments:
//      ds      A vtk dataset.
//
//  Programmer: Hank Childs
//  Creation:   June 17, 2001
//
// ****************************************************************************

avtDataset::avtDataset(vtkDataSet *ds)
    : avtDataObject(NULL)
{
    dataTree = new avtDataTree(ds, 0);
}


// ****************************************************************************
//  Method: avtDataset constructor
//
//  Arguments:
//      ds      A vtk dataset.
//
//  Programmer: Hank Childs
//  Creation:   September 21, 2005
//
// ****************************************************************************

avtDataset::avtDataset(vtkDataSet *ds, avtDataset *obj)
    : avtDataObject(obj->source)
{
    info.Copy(obj->GetInfo());
    dataTree = new avtDataTree(ds, 0);
}


// ****************************************************************************
//  Method: avtDataset copy constructor
//
//  Programmer: Mark C. Miller 
//  Creation:   April 23, 2003
//
// ****************************************************************************

avtDataset::avtDataset(avtDataset_p in, bool dontCopyData)
   : avtDataObject(in->source)
{
   info.Copy(in->GetInfo());
   dataTree = new avtDataTree(in->dataTree, dontCopyData);
}


// ****************************************************************************
//  Method: avtDataset copy constructor
//
//  Programmer: Hank Childs
//  Creation:   May 18, 2007
//
// ****************************************************************************

avtDataset::avtDataset(avtDataset_p in, avtDataTree_p newTree)
   : avtDataObject(in->source)
{
   info.Copy(in->GetInfo());
   dataTree = newTree;
}


// ****************************************************************************
//  Method: avtDataset destructor
//
//  Purpose:
//      Defines the destructor.  Note: this should not be inlined in the header
//      because it causes problems for certain compilers.
//
//  Programmer: Hank Childs
//  Creation:   February 5, 2004
//
// ****************************************************************************

avtDataset::~avtDataset()
{
    ;
}


// ****************************************************************************
//  Method: avtDataset::GetNumberOfCells
//
//  Purpose:
//      Returns number of cells in the dataset. Will count only polygons if
//      polysOnly is true
//
//  Programmer:  Mark C. Miller 
//  Creation:    November 5, 2003 
//
//  Modifications:
//    
//    Hank Childs, Tue Nov 18 06:09:52 PST 2008
//    Handle NULL data trees.
// 
// ****************************************************************************

long long
avtDataset::GetNumberOfCells(bool polysOnly) const
{
   int topoDim = -1;

   // we only care about topoDim if we're counting polys-only
   if (polysOnly)
      topoDim = GetInfo().GetAttributes().GetTopologicalDimension();

   if (*dataTree == NULL)
       return 0;

   return dataTree->GetNumberOfCells(topoDim, polysOnly);
}

// ****************************************************************************
//  Method: avtDataset::SetDataTree
//
//  Purpose:
//      Sets the domain to be the argument.
//
//  Arguments:
//      dr       A representaiton of the data.
//
//  Programmer:  Hank Childs
//  Creation:    July 24, 2000
//
//    Jeremy Meredith, Tue Sep 26 09:22:26 PDT 2000
//    Made this take an avtDomain.
//
//    Kathleen Bonnell, Fri Feb  9 17:11:18 PST 2001 
//    Made this set domain as an avtDomainTree.
//
//    Kathleen Bonnell, Thu Apr  5 14:50:11 PDT 2001 
//    Renamed this method as SetDataTree to reflect that data
//    is no longer stored as an array of avtDomainTrees but as
//    a single avtDataTree.  Removed argument specifying domain number.
//
// ****************************************************************************

void
avtDataset::SetDataTree(avtDataRepresentation &dr)
{
    dataTree = new avtDataTree(dr);
}


// ****************************************************************************
//  Method: avtDataset::SetDataTree
//
//  Purpose:
//      Sets the data tree to be the argument.
//
//  Arguments:
//      dt       A data tree.
//
//  Programmer:  Hank Childs
//  Creation:    July 24, 2000
//
//    Jeremy Meredith, Tue Sep 26 09:22:26 PDT 2000
//    Made this take an avtDomain.
//
//    Kathleen Bonnell, Thu Feb  1 13:49:46 PST 2001 
//    Made this take an avtDomainTree.
//
//    Kathleen Bonnell, Thu Apr  5 14:00:43 PDT 2001 
//    Renamed this method as SetDataTree to reflect that data
//    is no longer stored as an array of avtDomainTrees but as
//    a single avtDataTree.  Removed argument specifying domain number. 
//
// ****************************************************************************

void
avtDataset::SetDataTree(avtDataTree_p dt)
{
    dataTree = dt;
}


// ****************************************************************************
//  Method: avtDataset::Instance
//
//  Purpose:
//      Creates an instance of an avtDataset.
//
//  Programmer: Hank Childs
//  Creation:   July 24, 2001
//
// ****************************************************************************

avtDataObject *
avtDataset::Instance(void)
{
    avtDataObjectSource *src = NULL;
    avtDataset *ds = new avtDataset(src);
    return ds;
}


// ****************************************************************************
//  Method: avtDataset::DerivedCopy
//
//  Purpose:
//      Copies the argument to this object.  
//
//  Arguments:
//      dob     The dataset to copy.
//
//  Notes:      This should only be called by the base class' copy 
//              routine (avtDataObject::Copy).
//
//  Programmer: Hank Childs
//  Creation:   July 24, 2001
//
//  Modifications:
//
//    Hank Childs, Tue Jul 31 10:07:07 PDT 2001
//    Copy the data tree rather than sharing a reference.
//
//    Hank Childs, Wed Sep 11 09:19:41 PDT 2002
//    Do not assume that the data tree is non-NULL.
//
// ****************************************************************************

void
avtDataset::DerivedCopy(avtDataObject *dob)
{
    avtDataset *ds = (avtDataset *) dob;
    if (*(ds->dataTree) != NULL)
    {
        dataTree = new avtDataTree(ds->dataTree);
    }
    else
    {
        dataTree = NULL;
    }
}


// ****************************************************************************
//  Method: avtDataset::DerivedMerge
//
//  Purpose:
//      Merges the argument to this object.  
//
//  Arguments:
//      dob     The dataset to merge.
//
//  Notes:      This should only be called by the base class' merge 
//              routine (avtDataObject::Merge).
//
//  Programmer: Hank Childs
//  Creation:   July 24, 2001
//
//  Modifications:
//
//    Hank Childs, Wed Sep 11 09:19:41 PDT 2002
//    Do not assume that the data tree is non-NULL.
//
// ****************************************************************************

void
avtDataset::DerivedMerge(avtDataObject *dob)
{
    avtDataset *ds = (avtDataset *) dob;
    if (*(ds->dataTree) != NULL && *dataTree != NULL)
    {
        dataTree->Merge(ds->dataTree);
    }
    else if (*(ds->dataTree) != NULL)
    {
        dataTree = ds->dataTree;
    }
}


// ****************************************************************************
//  Method: avtDataset::WriteTreeStructure
//
//  Purpose:
//    Writes the data tree's structure to the given output stream. 
//
//  Arguments:
//    os       The output stream.
//    indent   The starting indent (optional).
//
//  Programmer: Kathleen Bonnell 
//  Creation:   September 18, 2001 
//
//  Modifications:
//
//    Hank Childs, Wed Sep 11 09:19:41 PDT 2002
//    Do not assume that the data tree is non-NULL.
//
// ****************************************************************************

void
avtDataset::WriteTreeStructure(ostream & os, int indent)
{
    if (*dataTree != NULL)
    {
        dataTree->WriteTreeStructure(os, indent);
    }
    else
    {
        os << "NULL data tree" << endl;
    }
}

// ****************************************************************************
//  Method: avtDataset::Compact
//
//  Purpose:
//      Compacts the dataset.
//
//  Programmer: Hank Childs
//  Creation:   September 28, 2001
//
//  Modifications:
//    Kathleen Bonnell, Fri Oct 12 11:38:41 PDT 2001
//    Set flag for compact tree filter specifying that execution does not
//    depend on DLB.
//
// ****************************************************************************

void
avtDataset::Compact(void)
{
    //
    // Create an identical dataset.  We need an identical one because we can't
    // send 'this' into avtSourceFromAVTDataset (ref_ptr issues).
    //
    avtDataset_p ds = (avtDataset *) Clone();
    ds->SetSource(source);

    //
    // Make a fake-o pipeline.
    //
    avtSourceFromAVTDataset *src = new avtSourceFromAVTDataset(ds);

    //
    // The compact tree filter is what will do the magic.
    //
    avtCompactTreeFilter *cf = new avtCompactTreeFilter;
    cf->SetInput(src->GetOutput());
    cf->DLBDependentExecutionOFF();

    //
    // Force an execution.  This is ugly.
    //
    cf->Update(cf->GetGeneralContract());

    //
    // Now copy the compact filter's output's tree into 'this'.
    //
    avtDataset_p ds1 = cf->GetTypedOutput();
    SetDataTree(ds1->GetDataTree());

    //
    // Clean up memory.
    //
    delete src;
    delete cf;
}


// ****************************************************************************
//  Method: avtDataset::InstantiateWriter
//
//  Purpose:
//      Instantiates a writer that is appropriate for an avtDataset.
//
//  Programmer: Hank Childs
//  Creation:   October 1, 2001
//
// ****************************************************************************

avtDataObjectWriter *
avtDataset::InstantiateWriter(void)
{
    return new avtDataSetWriter;
}


// ****************************************************************************
//  Method: avtDataset::ReleaseData
//
//  Purpose:
//      Releases this dataset's data tree.
//
//  Programmer: Hank Childs
//  Creation:   November 5, 2001
//
// ****************************************************************************

void
avtDataset::ReleaseData(void)
{
    dataTree = NULL;
}


// ****************************************************************************
//  Method: avtDataset::SetActiveVariable
//
//  Purpose:
//      Sets the active variable of the dataset.
//
//  Arguments:
//      name    The name of the new active variable.
//
//  Programmer: Hank Childs
//  Creation:   March 15, 2002
//
// ****************************************************************************

void
avtDataset::SetActiveVariable(const char *name)
{
    if (*dataTree != NULL)
    {
        SetActiveVariableArgs args;
        args.varname = name;
        bool success;
        dataTree->Traverse(CSetActiveVariable, (void *) &args, success);
    }
}


// ****************************************************************************
//  Method: avtDataset::DebugDump
//
//  Purpose:
//      Does a DebugDump
//
//  Programmer: Hank Childs
//  Creation:   December 21, 2006
//
//  Modifications:
//
//    Hank Childs, Wed Jan 17 11:02:43 PST 2007
//    Add support for bad data trees.
//
// ****************************************************************************

void
avtDataset::DebugDump(avtWebpage *webpage, const char *prefix)
{
    avtDataObject::DebugDump(webpage, prefix);
    if (*dataTree != NULL)
    {
        webpage->AddSubheading("Data tree");
        webpage->StartTable();
        webpage->AddTableHeader4("Node ID", "Parent ID", "Type", "Description");
        dataTree->DebugDump(webpage, prefix, 0, -1);
        webpage->EndTable();
    }
    else
        webpage->AddSubheading("NULL data tree ... very bad");
}


// ****************************************************************************
//  Method: avtDataset::CalculateSpatialIntervalTree
//
//  Purpose:
//      Calculates a spatial interval tree.
//
//  Programmer: Hank Childs
//  Creation:   June 16, 2008
//
//  Modifications:
//
//    Hank Childs, Wed Aug 13 09:05:12 PDT 2008
//    Add support for processors that have no data.
//
//    Hank Childs, Fri Sep 10 19:20:28 PDT 2010
//    Add an option for calculating a tree only on this processor.
//
// ****************************************************************************

avtIntervalTree *
avtDataset::CalculateSpatialIntervalTree(bool acrossAllProcs)
{
    vector<avtDataTree_p> trees;
    avtDataTree_p root = GetDataTree();
    vector<int> ids;
    root->GetAllDomainIds(ids);
    int max = -1;
    for (size_t i = 0 ; i < ids.size() ; i++)
        max = (ids[i] > max ? ids[i] : max);
    if (acrossAllProcs)
        max = UnifyMaximumValue(max);

    if (max < 0)
    {
        EXCEPTION0(ImproperUseException);
    }
    max += 1;

    int dim = GetInfo().GetAttributes().GetSpatialDimension();
    avtIntervalTree *itree = new avtIntervalTree(max, dim, acrossAllProcs);
    trees.push_back(root);
    int idx = 1;
    while (idx > 0)
    {
        idx -= 1;

        avtDataTree_p curTree = trees[idx];
        if (*curTree == NULL)
            continue;

        if (curTree->HasData())
        {
            avtDataRepresentation dr = curTree->GetDataRepresentation();
            int id = dr.GetDomain();
            if (id < 0 || id >= max)
            {
                EXCEPTION0(ImproperUseException);
            }
            double bbox[6];
            vtkDataSet *ds = dr.GetDataVTK();
            ds->GetBounds(bbox);
            itree->AddElement(id, bbox);
        }
        else
        { 
            int nc = curTree->GetNChildren();
            if (trees.size() < (size_t)idx+nc)
                trees.resize(idx+nc);
            for (int j = 0 ; j < nc ; j++)
                trees[idx++] = curTree->GetChild(j);
        }
    }
    
    itree->Calculate();
    return itree;
}


// ****************************************************************************
//  Method: avtDataset::RenumberDomainIDs
//
//  Purpose:
//      Renumbers the domain IDs.  If a domain has ID 10 and then the reflect
//      operator comes along, then each of the reflected data sets will also
//      have ID 10.  This is a nightmare for integral curve calculations.
//      This routine will through away the original IDs and create a new
//      numbering space.
//
//  Programmer: Hank Childs
//  Creation:   September 2, 2010
//
//  Modifications:
//
//    Hank Childs, Fri Sep 10 19:20:28 PDT 2010
//    Add an option for limiting the calculation to each processor.
//
//    Hank Childs, Tue Sep 21 11:34:17 CDT 2010
//    Fix array size problem.
//
// ****************************************************************************

void
avtDataset::RenumberDomainIDs(bool acrossAllProcs)
{
    int  i;

    int numLeaves = 0;
    vector<string> labels;
    vtkDataSet **leaves = NULL;
    if (*dataTree == NULL)
    {
        if (! acrossAllProcs)
            return;
    }
    else
    {
        leaves = dataTree->GetAllLeaves(numLeaves);
        dataTree->GetAllLabels(labels);
    }

    // This should never execute, but better safe than sorry
    while (labels.size() < (size_t)numLeaves)
    {
        //debug1 << "Unexpected: less labels than leaves" << endl;
        labels.push_back("");
    }

    int myOffset = 0;
    if (acrossAllProcs)
    {
        int  numProcs = PAR_Size();
        int *numPer = new int[numProcs];
        for (i = 0 ; i < numProcs ; i++)
            numPer[i] = 0;
        numPer[PAR_Rank()] = numLeaves;
        int *numPer2 = new int[numProcs];
        SumIntArrayAcrossAllProcessors(numPer, numPer2, numProcs);
        delete [] numPer;
        numPer = numPer2;
        for (i = 0 ; i < PAR_Rank() ; i++)
            myOffset += numPer[i];
        delete [] numPer;
    }
    
    if (numLeaves == 0) // had to wait this long to do parallel communication.
    {
        // Free the memory from the GetAllLeaves function call.
        delete [] leaves;
        return;
    }

    avtDataTree_p *newTrees = new avtDataTree_p[numLeaves];
    for (i = 0 ; i < numLeaves ; i++)
         newTrees[i] = new avtDataTree(leaves[i], myOffset++, labels[i]);

    dataTree = new avtDataTree(numLeaves, newTrees);

    delete [] newTrees; // new array allocated by avtDataTree constructor

    // Free the memory from the GetAllLeaves function call.
    delete [] leaves;
}

std::string
avtDataset::GetDatasetAsString()
{
    return dataTree->GetDatasetAsString();
}
