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

// ****************************************************************************
//  Class:   vtkPolyDataOnionPeelFilter
//
//  Purpose:
//    Derived type of vtkPolyDataToPolyDataFilter.
//
//  Programmer:  Kathleen S. Bonnell
//  Creation:  5 October 2000
//
// ****************************************************************************

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkIdList.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkIntArray.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataOnionPeelFilter.h>
#include <vtkPointData.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkUnsignedIntArray.h>
#include <vtkVisItUtility.h>

// ****************************************************************************
//  Modifications:
//    Kathleen Bonnell, Wed Mar  6 15:14:29 PST 2002 
//    Replace 'New' method with Macro to match VTK 4.0 API. 
//
// ****************************************************************************

vtkStandardNewMacro(vtkPolyDataOnionPeelFilter);


// ****************************************************************************
//  Constructor: vtkPolyDataOnionPeelFilter
//
//  Purpose:
//    Construct with adjacency set to Node-Adjacency.
//    SeedCellId set to 0, RequestedLayer set to 0
//
//  Modifications:
//    Kathleen Bonnell, Thu Aug 15 18:37:59 PDT 2002 
//    Initialize logicalIndex and useLogicalIndex. 
//
//    Kathleen Bonnell, Tue Jan 18 19:37:46 PST 2005 
//    Initialize ReconstructOriginalCells. 
//
//    Kathleen Bonnell, Wed Jan 19 15:54:38 PST 2005 
//    Renamed 'SeedCellId' to 'SeedId'.  Initialize SeedIdIsForCell. 
//
// ****************************************************************************

vtkPolyDataOnionPeelFilter::vtkPolyDataOnionPeelFilter()
{
    this->RequestedLayer = 0;
    this->SeedId = 0;
    this->logicalIndex[0] = this->logicalIndex[1] = this->logicalIndex[2] = 0;
    this->useLogicalIndex = false;
    this->maxLayersReached = 0;
    this->maxLayerNum = VTK_INT_MAX;
    this->AdjacencyType = VTK_NODE_ADJACENCY;
    this->ReconstructOriginalCells = false; 
    this->SeedIdIsForCell = true; 

    this->layerCellIds = vtkIdList::New();
    this->layerCellIds->Allocate(500);
    this->cellOffsets = vtkIdList::New();
    this->cellOffsets->Allocate(50);

    this->bsc_callback = NULL;
    this->bsc_args = NULL;
}


// ****************************************************************************
//  Destructor: ~vtkPolyDataOnionPeelFilter
//
// ****************************************************************************

vtkPolyDataOnionPeelFilter::~vtkPolyDataOnionPeelFilter()
{
    this->layerCellIds->Delete();
    this->layerCellIds = NULL;

    this->cellOffsets->Delete();
    this->cellOffsets = NULL;
}


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::SetBadSeedCallback
//
//  Purpose:
//    Sets a callback that is called if the seed cell is bad.
// 
//  Arguments:
//    cb      The callback.
//    args    The arguments to cb.
// 
//  Returns:  None 
// 
//  Programmer: Hank Childs
//  Creation:   May 22, 2002
//
//  Modifications:
//    Kathleen Bonnell, Wed Jan 19 15:54:38 PST 2005
//    Removed 'Cell' from method name, arg name.
//
// ****************************************************************************

void 
vtkPolyDataOnionPeelFilter::SetBadSeedCallback(BadSeedCallback cb, void *args)
{
    bsc_callback = cb;
    bsc_args     = args;
}


// ****************************************************************************
//  Method:   vtkPolyDataOnionPeelFilter::Initialize
//
//  Purpose:  
//    Initialize data members in preparation for new layers.
// 
//  Arguments:  None
// 
//  Returns:  None 
// 
//  Assumptions and Comments:
//    Intialization should occur when certain fields of the filter have been
//    modified.  Namely input to the filter, SeedCellId, and
//    AdjacencyType.  Modification of RequestedLayer requires no initialization.
//
//    first item (slot 0) of layerCellIds is always SeedCellId
//    first item (slot 0) of cellOffsets is always adjacenyType 
//    (used for modification check in Execute method).
//
//  Programmer: Kathleen S. Bonnell
//  Creation:   5 October 2000
//
//  Modifications:
//    Hank Childs, Wed May 22 16:59:53 PDT 2002
//    Also call a callback if the seed cell is invalid.
//
//    Kathleen Bonnell, Thu Aug 15 17:48:38 PDT 2002  
//    Since we are issuing an error callback for a bad seed,
//    don't allow further processing. 
//
//    Hank Childs, Fri Aug 27 15:15:20 PDT 2004
//    Renamed ghost data arrays.
//
//    Kathleen Bonnell, Tue Jan 18 19:37:46 PST 2005 
//    Addeed logic to handle requests for reconstructing original cells,
//    e.g. when connectivity of original input has changed.
//
//    Kathleen Bonnell, Wed Jan 19 15:54:38 PST 2005 
//    Renamed 'SeedCellId to 'SeedId', 'numCells' arg to 'numIds'.
//    Added code to handle seedId that is a node. 
// 
//    Eric Brugger, Wed Jan  9 12:05:05 PST 2013
//    Modified to inherit from vtkPolyDataAlgorithm.
//
// ****************************************************************************

bool 
vtkPolyDataOnionPeelFilter::Initialize()
{
    this->maxLayersReached = 0;
    this->maxLayerNum = VTK_INT_MAX;

    int numIds;
    if (this->SeedIdIsForCell)
       numIds = input->GetNumberOfCells();
    else 
       numIds = input->GetNumberOfPoints();

    if (useLogicalIndex)
    {
        int dims[3] = { 1, 1, 1};
        if (input->GetDataObjectType() == VTK_STRUCTURED_GRID)
        {
            ((vtkStructuredGrid*)input)->GetDimensions(dims);
        }
        else if (input->GetDataObjectType() == VTK_RECTILINEAR_GRID)
        {
            ((vtkRectilinearGrid*)input)->GetDimensions(dims);
        }
        if (this->logicalIndex[0] >= dims[0] ||
            this->logicalIndex[1] >= dims[1] ||
            this->logicalIndex[2] >= dims[2])
        {
            if (bsc_callback != NULL) 
            {
                bsc_callback(bsc_args, SeedId, numIds, false);
           
            } 
            vtkWarningMacro(<<"SeedIndex (" << this->logicalIndex[0] << " "
                << this->logicalIndex[1] << " " << this->logicalIndex[2] 
                << ")  Exceeds dimensions of dataset ("
                << dims[0] << " " << dims[1] << " " << dims[2] << ").");
            return false; //unsuccessful initialization
        } 
        if (this->SeedIdIsForCell)
        {
            this->SeedId = this->logicalIndex[2]*(dims[0]-1)*(dims[1]-1) + 
                           this->logicalIndex[1]*(dims[0]-1) + 
                           this->logicalIndex[0];
        } 
        else 
        {
            this->SeedId = this->logicalIndex[2]*(dims[0])*(dims[1]) + 
                           this->logicalIndex[1]*(dims[0]) + 
                           this->logicalIndex[0];
        } 
    }
    // check for out-of-range error on seedcellId;
    if (!this->ReconstructOriginalCells &&
       (this->SeedId < 0 || this->SeedId >= numIds) )
    {
        if (bsc_callback != NULL) 
        {
            if (useLogicalIndex)
            {
                bsc_callback(bsc_args, SeedId, numIds, false);
            }
            else 
            {
                bsc_callback(bsc_args, SeedId, numIds, false);
            }
        }
        vtkWarningMacro(<<"SeedId " << this->SeedId << " is Invalid."
                        <<"\nValid ids range from 0 to " << numIds-1 << ".");
        return false; //unsuccessful initialization
    }
    //
    // check if seedcellId is a ghost cell;
    //
    vtkDataArray *ghosts;
    if (this->SeedIdIsForCell)
       ghosts = input->GetCellData()->GetArray("avtGhostZones");
    else
       ghosts = input->GetCellData()->GetArray("avtGhostNodes");
    if (ghosts)
    {
        if (ghosts->GetComponent(this->SeedId, 0) != 0)
        {
            if (bsc_callback != NULL) 
            {
                if (useLogicalIndex)
                {
                    bsc_callback(bsc_args, SeedId, numIds, true);
                }
                else 
                {
                    bsc_callback(bsc_args, SeedId, numIds, true);
                }
            }
            vtkWarningMacro(<<"SeedId " << this->SeedId << " is a Ghost Cell.");
            return false; //unsuccessful initialization
        }
    }

    this->layerCellIds->Reset();
    this->cellOffsets->Reset();
    if (this->SeedIdIsForCell)
    {
        if (!this->ReconstructOriginalCells)
        {
            this->layerCellIds->InsertNextId(this->SeedId);
        }
        else 
        {
            this->FindCellsCorrespondingToOriginal(this->SeedId, this->layerCellIds);
            if (this->layerCellIds->GetNumberOfIds() == 0) 
            {
                if (bsc_callback != NULL) 
                    bsc_callback(bsc_args, SeedId, numIds, false);
                vtkWarningMacro(<<"SeedId " << this->SeedId 
                                << " is not available from current data.");
                return false; //unsuccessful initialization
            }
        }
    }
    else  //SeedId is a node Id
    {
        if (!this->ReconstructOriginalCells)
        {
            input->GetPointCells(this->SeedId, this->layerCellIds); 
            if (this->layerCellIds->GetNumberOfIds() == 0) 
            {
                if (bsc_callback != NULL) 
                    bsc_callback(bsc_args, SeedId, numIds, false);
                vtkWarningMacro(<<"SeedId " << this->SeedId 
                                << " is not available from current data.");
                return false; //unsuccessful initialization
            }
        }
        else 
        {
            int i;
            vtkIdList *nodes = vtkIdList::New();
            this->FindNodesCorrespondingToOriginal(this->SeedId, nodes);
            if (nodes->GetNumberOfIds() == 0)
            {
                if (bsc_callback != NULL) 
                    bsc_callback(bsc_args, SeedId, numIds, false);
                vtkWarningMacro(<<"SeedId " << this->SeedId 
                                << " is not available from current data.");
                return false; //unsuccessful initialization
            }
            vtkIdList *neighbors = vtkIdList::New();
            for (i = 0; i < nodes->GetNumberOfIds(); i++)
            {
                input->GetPointCells(nodes->GetId(i), neighbors);        
                for (int nId = 0; nId < neighbors->GetNumberOfIds(); nId++)
                {
                    this->layerCellIds->InsertUniqueId(neighbors->GetId(nId));
                }
            }
            nodes->Delete();
            neighbors->Delete();
            vtkUnsignedIntArray *origCells = vtkUnsignedIntArray::SafeDownCast(
                input->GetCellData()->GetArray("avtOriginalCellNumbers"));

            if (origCells)
            {
                unsigned int *oc = origCells->GetPointer(0);
                int nc = origCells->GetNumberOfComponents();
                int comp = nc -1;
                vtkIdList *origIds = vtkIdList::New();
                for (i = 0; i < this->layerCellIds->GetNumberOfIds(); i++)
                {
                        int cellId = this->layerCellIds->GetId(i);
                        int index = cellId *nc + comp;;
                        origIds->InsertNextId(oc[index]);
                }
                FindCellsCorrespondingToOriginal(origIds, this->layerCellIds);
                origIds->Delete();
            }
            
            if (this->layerCellIds->GetNumberOfIds() == 0) 
            {
                if (bsc_callback != NULL) 
                    bsc_callback(bsc_args, SeedId, numIds, false);
                vtkWarningMacro(<<"SeedId " << this->SeedId 
                                << " is not available from current data.");
                return false; //unsuccessful initialization
            }
        }
    }
    //layer 0 offset always zero, so use zeroth slot to indicate AdjacencyType
    this->cellOffsets->InsertNextId(this->AdjacencyType);
    return true; // successful initialization
} // Initialize


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::Grow
//
//  Purpose:
//    Adds more layers to the onion peel, up to the layer requested by
//    the user or grid boundaries, whichever is reached first.
//
//  Arguments:  None
//
//  Returns:    None 
//
//  Assumptions and Comments:
//    This method will stop attempting to grow layers when grid
//    boundaries are reached and no more neighbors can be found.  At
//    which point, a warning message is issued and RequestedLayer is
//    set to the maximum layers possible for the current conditions.
//
//  Programmer: Kathleen S. Bonnell
//  Creation:   5 October 2000
//
//  Modifications:
//    Kathleen Bonnell, Thu Aug 15 17:48:38 PDT 2002  
//    Coding style update.
//  
//    Kathleen Bonnell, Tue Jan 18 19:37:46 PST 2005 
//    Addeed logic to handle requests for reconstructing original cells,
//    e.g. when connectivity of original input has changed.
//
//    Eric Brugger, Wed Jan  9 12:05:05 PST 2013
//    Modified to inherit from vtkPolyDataAlgorithm.
//
// ****************************************************************************

void 
vtkPolyDataOnionPeelFilter::Grow()
{
    vtkIdList  *currentLayerList  = vtkIdList::New();
    int         totalCurrentCells = this->layerCellIds->GetNumberOfIds();
    int         totalLayersGrown  = this->cellOffsets->GetNumberOfIds() - 1;
    int         start = 0, i = 0, j = 0;

    vtkDebugMacro(<<"Grow::");
    while (this->cellOffsets->GetNumberOfIds() <= this->RequestedLayer ) 
    {
        if (this->maxLayersReached) 
        {
            // cannot create more layers, no more neighbors, we
            //  must be at the boundaries of the grid so stop this loop
            vtkWarningMacro(<<"Grid Boundaries reached. \nRequestedLayer has "
                            <<"been set to the maxLayerNum possible.");

            this->RequestedLayer  = this->maxLayerNum 
                                  = this->cellOffsets->GetNumberOfIds()-1;
            break;
        }

        // create list of cell ids for last layer that was grown

        if (totalLayersGrown == 0) 
            start  = 0;
        else
            start = this->cellOffsets->GetId(totalLayersGrown);

        currentLayerList->SetNumberOfIds(this->layerCellIds->GetNumberOfIds() 
                                        - start);

        for (i = start,j = 0; i < this->layerCellIds->GetNumberOfIds(); i++, j++) 
        {
            currentLayerList->InsertId(j, this->layerCellIds->GetId(i));
        }

        // now add another layer by finding neighbors of cells in previous layer 

        if (this->AdjacencyType == VTK_FACE_ADJACENCY)  
        {
            FindCellNeighborsByFaceAdjacency(currentLayerList, this->layerCellIds);
        }
        else
        {
            FindCellNeighborsByNodeAdjacency(currentLayerList, this->layerCellIds);
        }

        // did we add new cells??? 
        if (this->layerCellIds->GetNumberOfIds() > totalCurrentCells) 
        {
            if (this->ReconstructOriginalCells)
            {
                vtkUnsignedIntArray *origCells = vtkUnsignedIntArray::SafeDownCast(
                  input->GetCellData()->GetArray("avtOriginalCellNumbers"));

                if (origCells)
                {
                    unsigned int *oc = origCells->GetPointer(0);
                    int nc = origCells->GetNumberOfComponents();
                    int comp = nc -1;
                    vtkIdList *origIds = vtkIdList::New();
                    start = totalCurrentCells;
                    for (i = start; i < this->layerCellIds->GetNumberOfIds(); i++)
                    {
                        int cellId = this->layerCellIds->GetId(i);
                        int index = cellId *nc + comp;;
                        origIds->InsertNextId(oc[index]);
                    }
                    FindCellsCorrespondingToOriginal(origIds, this->layerCellIds);
                    origIds->Delete();
                }
            }
            // set the offset for this new layer of cells
            this->cellOffsets->InsertNextId(totalCurrentCells);
            totalCurrentCells = this->layerCellIds->GetNumberOfIds();
        }
        else 
        {
            vtkDebugMacro("Grow:  No neighbors found");
            this->maxLayersReached = 1;
        }

        currentLayerList->Reset();
   
    } /* end while */

    currentLayerList->Delete();

} // Grow()


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::RequestData
//
//  Purpose:  
//    vtk Required method. Updates state of the filter by growing
//    onion peel layers as necessary and generating output grid. 
//    Performs error checking on data set type.
//
//  Arguments:  None
//
//  Returns:    None 
//
//  Assumptions and Comments:
//    Assumes this filter's input data set is rectilinear, structured
//    or unstructured grid only.  Check for this condition performed
//    in RequestData() method.
//
//    Passes along all of the input points and point data, but on.y
//    cells and cell data corresponding to requested layers.
//
//  Programmer: Kathleen S. Bonnell
//  Creation:   5 October 2000
//
//  Modifications:
//    Kathleen Bonnell, Tue Sep 25 14:32:46 PDT 2001
//    Removed tests for modification, re-excute from scratch each time
//    as is appropriate for the VisIt pipeline.
//
//    Kathleen Bonnell, Thu Aug 15 17:48:38 PDT 2002  
//    Made Initialize return a bool indicating wheter the initialization
//    was a success or not.  If not, don't process further. 
//
//    Kathleen Bonnell, Wed Jan 19 15:54:38 PST 2005 
//    Use different args for Initialize when seedId is for a node. 
//
//    Eric Brugger, Wed Jan  9 12:05:05 PST 2013
//    Modified to inherit from vtkPolyDataAlgorithm.
//
// ****************************************************************************

int
vtkPolyDataOnionPeelFilter::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    //
    // Initialize some frequently used values.
    //
    input  = vtkDataSet::SafeDownCast(
        inInfo->Get(vtkDataObject::DATA_OBJECT()));
    output = vtkPolyData::SafeDownCast(
        outInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkDebugMacro(<<"Generating PolyDataOnionPeelFilter Layers");

    bool success;
    if (this->SeedIdIsForCell)
       success = this->Initialize();
    else 
       success = this->Initialize();

    if (!success)
    {
        return 1;
    }

    // check for out-of-range error on RequestedLayer
    if (RequestedLayer > this->maxLayerNum) 
    {
        vtkWarningMacro(<<"Requested Layer greater than max layers possible."
        <<"\nRequestedLayer has been set to the max possible:  "
        << this->maxLayerNum << ".");

        this->RequestedLayer  = this->maxLayerNum ;
    }

    Grow();

    this->GenerateOutputGrid();

    return 1;
} // RequestData

      
// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::FillInputPortInformation
//
//  Purpose:  
//    Specifies that the input must be a vtkDataSet. 
//
//  Arguments:
//    info    The input port information
//
//  Returns:
//    1
//
//  Assumptions and Comments:
//
//  Programmer: Eric Brugger
//  Creation:   Wed Jan  9 12:05:05 PST 2013
//
// ****************************************************************************

int
vtkPolyDataOnionPeelFilter::FillInputPortInformation(int, vtkInformation *info)
{
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");

    return 1;
} // FillInputPortInformation


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::GenerateOutputGrid
//
//  Purpose:  
//    Creates the unstructured grid for output. 
//
//  Arguments:  None
//
//  Returns:    None 
//
//  Assumptions and Comments:
//    Assumes this filter's input data set is rectilinear, structured
//    or unstructured grid only.  Check for this condition performed
//    in Execute() method.
//
//    Passes along all of the input points and point data, but on.y
//    cells and cell data corresponding to requested layers.
//
//  Programmer: Kathleen S. Bonnell
//  Creation:   5 October 2000
//
//  Modifications:
//    Kathleen Bonnell, Thu Aug 15 17:48:38 PDT 2002  
//    Coding style update.
//
//    Kathleen Bonnell, Tue Jun 24 14:19:49 PDT 2003 
//    Allow for poly-data input, retrieve points via
//    vtkVisItUtility::GetPoints. 
//
//    Hank Childs, Thu Mar 10 09:48:47 PST 2005
//    Fix memory leak.
//
//    Eric Brugger, Wed Jan  9 12:05:05 PST 2013
//    Modified to inherit from vtkPolyDataAlgorithm.
//
// ****************************************************************************

void 
vtkPolyDataOnionPeelFilter::GenerateOutputGrid()
{
    vtkDebugMacro(<<"GenerateOutputGrid::");

    vtkPointData        *inPD       = input->GetPointData();
    vtkCellData         *inCD       = input->GetCellData();
    vtkPointData        *outPD      = output->GetPointData();
    vtkCellData         *outCD      = output->GetCellData();
    vtkIdList           *cellPts    = vtkIdList::New();
    int i, cellId, newCellId, totalCells;

    if (this->RequestedLayer < this->cellOffsets->GetNumberOfIds() -1)
    {
        totalCells = this->cellOffsets->GetId(this->RequestedLayer + 1);
    }
    else
    {
        totalCells =  this->layerCellIds->GetNumberOfIds();
    }
    output->Allocate(totalCells);

    // grab points from input so they can be passed along directly to ouput
    vtkPoints *pts = vtkVisItUtility::GetPoints(input);
    output->SetPoints(pts);
    pts->Delete();
 
    outPD->PassData(inPD);
    outCD->CopyAllocate(inCD);

    // grab only the cell data that corresponds to cells in our layers
    for (i = 0; i < totalCells; i++) 
    {
        cellId = layerCellIds->GetId(i);
        input->GetCellPoints(cellId, cellPts);
        newCellId = output->InsertNextCell(input->GetCellType(cellId), cellPts);
        outCD->CopyData(inCD, cellId, newCellId);
    }

    // free up unused memory in output
    output->Squeeze();

    // free memory used locally
    cellPts->Delete();

} // GenerateOutputGrid


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::PrintSelf
//
//  Purpose:  Prints pertinent information regarding the state of this class 
//           to the given output stream.
//
//  Arguments:
//    os      The output stream to which the information is printed
//    indent  The amount of spaces to indent.
//
//  Returns:  None 
//
//  Assumptions and Comments:
//    Calls the superclass method first. 
//
//  Programmer: Kathleen S. Bonnell
//  Creation:   5 October 2000
//
//  Modifications:
//    Kathleen Bonnell, Thu Aug 15 17:48:38 PDT 2002  
//    Coding style update.
//
// ****************************************************************************

void 
vtkPolyDataOnionPeelFilter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "Seed Cell Id:    " << this->SeedId << "\n";
    os << indent << "Requested Layer: " << this->RequestedLayer << "\n";
    os << indent << "Adjacency Type:  " 
       << this->GetAdjacencyTypeAsString() << "\n";
}


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::FindCellNeighborsByNodeAdjacency
//
//  Purpose: Finds all cells in the input grid that share a node with
//           the given cell. 
//
//  Arguments:
//    prevLayerIds       The unique id numbers of the cells for which 
//                       we want to find neighbors 
//    neighborCellIds    Pointer to list of cells in the onion peel layers 
//
//  Returns:             None 
//
//  Assumptions and Comments:
//    upon entry to the method, neighborCellIds contains cell ids from 
//    previously grown layers.  This method adds new (unique) neighbor 
//    cell ids to the list.
//
//    It is assumed that cellId represents a valid cell
//
//  Programmer: Kathleen S. Bonnell
//  Creation:   5 October 2000
//
//  Modifications:
//    Kathleen Bonnell, Thu Aug 15 17:48:38 PDT 2002  
//    Coding style update.
//
//    Eric Brugger, Wed Jan  9 12:05:05 PST 2013
//    Modified to inherit from vtkPolyDataAlgorithm.
//
// ****************************************************************************

void 
vtkPolyDataOnionPeelFilter::FindCellNeighborsByNodeAdjacency(
    vtkIdList *prevLayerIds, vtkIdList* neighborCellIds)
{
    vtkIdList  *ids        = vtkIdList::New();
    vtkIdList  *neighbors  = vtkIdList::New();
    int         pntId;
    int         nId;
    int         i;

    for (i = 0; i < prevLayerIds->GetNumberOfIds(); i++) 
    {
        input->GetCellPoints(prevLayerIds->GetId(i), ids);

        for (pntId = 0; pntId < ids->GetNumberOfIds(); pntId++) 
        {
            input->GetPointCells(ids->GetId(pntId), neighbors);        

            for (nId = 0; nId < neighbors->GetNumberOfIds(); nId++)
            {
                neighborCellIds->InsertUniqueId(neighbors->GetId(nId));
            }
        }
    }

    neighbors->Delete();
    ids->Delete();
}


// ****************************************************************************
//  Method:   vtkPolyDataOnionPeelFilter::FindCellNeighborsByFaceAdjacency
//
//  Purpose:  Finds all cells in the input grid that share a face with
//            the given cell.
//
//  Arguments:
//    cellId             The unique id number of the cell for which 
//                       we want to find neighbors 
//    neighborCellIds    Pointer to list of cells in the onion peel layers 
//
//  Returns:             None 
//
//  Assumptions and Comments:
//    upon entry to the method, neighborCellIds contains cell ids from 
//    previously grown layers.  This method adds new (unique) neighbor 
//    cell ids to the list.
//
//    It is assumed that cellId represents a valid cell
//
//    This method does not consider cells below 3D when determining
//    face neighbors.
//
//  Programmer: Kathleen S. Bonnell
//  Creation:   5 October 2000
//
//  Modifications:
//    Kathleen S. Bonnell, Mon Oct 30 10:37:56 PST 2000
//    Added neighbor search via edges for 2D cells. 
//
//    Kathleen Bonnell, Thu Aug 15 17:48:38 PDT 2002  
//    Coding style update.
//
//    Eric Brugger, Wed Jan  9 12:05:05 PST 2013
//    Modified to inherit from vtkPolyDataAlgorithm.
//
// ****************************************************************************

void 
vtkPolyDataOnionPeelFilter::FindCellNeighborsByFaceAdjacency(
    vtkIdList* prevLayerIds, vtkIdList* neighborCellIds)
{
    vtkIdList  *neighbors = vtkIdList::New();
    vtkIdList  *facePts   = NULL;
    vtkIdList  *edgePts   = NULL;
    vtkCell    *cell      = NULL;
    int         faceId;
    int         edgeId;
    int         nId;
    int         i;
    int         cellId;
  
    for (i = 0; i < prevLayerIds->GetNumberOfIds(); i++) 
    {
        cellId = prevLayerIds->GetId(i);
        cell = input->GetCell(cellId);

        if (cell->GetCellDimension() > 2) 
        {
            for (faceId = 0; faceId < cell->GetNumberOfFaces(); faceId++) 
            {
                facePts = (cell->GetFace(faceId))->GetPointIds();

                input->GetCellNeighbors(cellId, facePts, neighbors);

                for (nId = 0; nId < neighbors->GetNumberOfIds(); nId++) 
                {
                    neighborCellIds->InsertUniqueId(neighbors->GetId(nId));
                }
            }
        } 
        else 
        {
            for (edgeId = 0; edgeId < cell->GetNumberOfEdges(); edgeId++) 
            {
                edgePts = (cell->GetEdge(edgeId))->GetPointIds();

                input->GetCellNeighbors(cellId, edgePts, neighbors);

                for (nId = 0; nId < neighbors->GetNumberOfIds(); nId++) 
                {
                    neighborCellIds->InsertUniqueId(neighbors->GetId(nId));
                }
            }
        }
    }
  
    neighbors->Delete();
}


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::SetLogicalIndex
//
//  Purpose:  
//    Set the logical index. 
//
//  Arguments:  
//    i, j, k   The components of the logical Index.
//
//  Returns:    None 
//
//  Programmer: Kathleen Bonnell
//  Creation:   August 15, 2002 
//
//  Modifications:
//
// ****************************************************************************

void
vtkPolyDataOnionPeelFilter::SetLogicalIndex(const int i, const int j,
    const int k)
{  
    if (!useLogicalIndex ||
        (this->logicalIndex[0] != i) || 
        (this->logicalIndex[1] != j) || 
        (this->logicalIndex[2] != k) )
    {
        this->logicalIndex[0] = i;
        this->logicalIndex[1] = j;
        this->logicalIndex[2] = k;
        this->Modified();
    }
    useLogicalIndex = true;
}


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::SetSeedId
//
//  Purpose:  
//    Set the seed cell id. 
//
//  Arguments:  
//    seed   The new seed cell id.
//
//  Returns:    None 
//
//  Programmer: Kathleen Bonnell
//  Creation:   August 15, 2002 
//
//  Modifications:
//
// ****************************************************************************

void
vtkPolyDataOnionPeelFilter::SetSeedId(const int seed)
{  
    if (useLogicalIndex || this->SeedId != seed)
    {
        this->SeedId = seed;
        this->Modified();
    }
    useLogicalIndex = false;
}


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::FindCellsCorrespondingToOriginal
//
//  Purpose:  
//    Finds all cells whose 'originalCell' designation matches the
//    original id passed as arg. 
//
//  Arguments:  
//    orig      The original cell id. 
//    group     A place to store the corresponding cells.
//
//  Returns:    None 
//
//  Programmer: Kathleen Bonnell
//  Creation:   January 18, 2005 
//
//  Modifications:
//    Kathleen Bonnell, Tue Jun 14 11:45:21 PDT 2005
//    Correct 'n' for loop counting.
//
//    Eric Brugger, Wed Jan  9 12:05:05 PST 2013
//    Modified to inherit from vtkPolyDataAlgorithm.
//
// ****************************************************************************

void
vtkPolyDataOnionPeelFilter::FindCellsCorrespondingToOriginal(
    int orig, vtkIdList *group)
{
    vtkUnsignedIntArray *origCells = vtkUnsignedIntArray::SafeDownCast(
        input->GetCellData()->GetArray("avtOriginalCellNumbers"));

    if (origCells)
    {
        unsigned int *oc = origCells->GetPointer(0);
        int nc = origCells->GetNumberOfComponents();
        int n = origCells->GetNumberOfTuples() *nc;
        int comp = nc -1;
        for (int i = comp; i < n; i+=nc )
        {
            int id = i / nc;
            if (oc[i] == (unsigned int)orig && group->IsId(id) == -1)
                group->InsertNextId(id);
        }
    }
}


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::FindCellsCorrespondingToOriginal
//
//  Purpose:  
//    Finds all cells whose 'originalCell' designation matches the
//    original ids passed as arg. 
//
//  Arguments:  
//    origs     A list of original ids. 
//    group     A place to store the corresponding cells.
//
//  Returns:    None 
//
//  Programmer: Kathleen Bonnell
//  Creation:   January 18, 2005 
//
//  Modifications:
//    Kathleen Bonnell, Tue Jun 14 11:45:21 PDT 2005
//    Correct 'n' for loop counting.
//
//    Eric Brugger, Wed Jan  9 12:05:05 PST 2013
//    Modified to inherit from vtkPolyDataAlgorithm.
//
// ****************************************************************************

void
vtkPolyDataOnionPeelFilter::FindCellsCorrespondingToOriginal(
    vtkIdList *origs, vtkIdList *group)
{
    vtkUnsignedIntArray *origCells = vtkUnsignedIntArray::SafeDownCast(
        input->GetCellData()->GetArray("avtOriginalCellNumbers"));

    if (origCells)
    {
        unsigned int *oc = origCells->GetPointer(0);
        int nc = origCells->GetNumberOfComponents();
        int n = origCells->GetNumberOfTuples()*nc;
        int comp = nc -1;
        for (int i = comp; i < n; i+=nc)
        {
            int id = i / nc;
            if (origs->IsId(oc[i]) != -1 && group->IsId(id) == -1)
                group->InsertNextId(id);
        }
    }
}


// ****************************************************************************
//  Method: vtkPolyDataOnionPeelFilter::FindNodesCorrespondingToOriginal
//
//  Purpose:  
//    Finds all nodes whose 'originalNode' designation matches the
//    original id passed as arg. 
//
//  Arguments:  
//    orig      The original node id. 
//    group     A place to store the corresponding cells.
//
//  Returns:    None 
//
//  Programmer: Kathleen Bonnell
//  Creation:   January 19, 2005 
//
//  Modifications:
//    Kathleen Bonnell, Tue Jun 14 11:45:21 PDT 2005
//    Correct 'n' for loop counting.
//
//    Eric Brugger, Wed Jan  9 12:05:05 PST 2013
//    Modified to inherit from vtkPolyDataAlgorithm.
//
// ****************************************************************************

void
vtkPolyDataOnionPeelFilter::FindNodesCorrespondingToOriginal(
    int orig, vtkIdList *group)
{
    vtkIntArray *origNodes = vtkIntArray::SafeDownCast(
        input->GetPointData()->GetArray("avtOriginalNodeNumbers"));

    if (origNodes)
    {
        int *on = origNodes->GetPointer(0);
        int nc = origNodes->GetNumberOfComponents();
        int n = origNodes->GetNumberOfTuples() *nc;
        int comp = nc -1;
        for (int i = comp; i < n; i+=nc )
        {
            int id = i / nc;
            if (on[i] == orig && group->IsId(id) == -1)
                group->InsertNextId(id);
        }
    }
}
