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
//                             avtGTCFileFormat.C                            //
// ************************************************************************* //

#include <avtGTCFileFormat.h>

#include <avtDatabaseMetaData.h>

#include <InvalidDBTypeException.h>
#include <InvalidVariableException.h>
#include <InvalidFilesException.h>

#include <DebugStream.h>

#include <vtkCellType.h>
#include <vtkFloatArray.h>
#include <vtkUnstructuredGrid.h>

#ifdef PARALLEL
#include <mpi.h>
#include <avtParallel.h>
#endif

#include <vector>

// ****************************************************************************
//  Method: avtGTCFileFormat constructor
//
//  Programmer: pugmire -- generated by xml2avt
//  Creation:   Tue Nov 20 14:08:56 PST 2007
//
// ****************************************************************************

avtGTCFileFormat::avtGTCFileFormat(const char *filename)
    : avtSTMDFileFormat(&filename, 1)
{
    nVars = 0;
    nTotalPoints = 0;
    nPoints = 0;
    initialized = false;

    // Make sure that the file is in fact GTC.
    Initialize();
}

// ****************************************************************************
//  Method: avtGTCFileFormat::FreeUpResources
//
//  Purpose:
//      When VisIt is done focusing on a particular timestep, it asks that
//      timestep to free up any resources (memory, file descriptors) that
//      it has associated with it.  This method is the mechanism for doing
//      that.
//
//  Programmer: pugmire -- generated by xml2avt
//  Creation:   Tue Nov 20 14:08:56 PST 2007
//
// ****************************************************************************

void
avtGTCFileFormat::FreeUpResources(void)
{
    initialized = false;
}

// ****************************************************************************
// Method: avtGTCFileFormat::Initialize
//
// Purpose: 
//   Opens the file and makes some other checks.
//
// Programmer: Brad Whitlock
// Creation:   Thu Apr 27 13:48:11 PST 2006
//
// Modifications:
//   
//    Hank Childs, Wed Jul  9 06:34:05 PDT 2008
//    Uncomment code disabling error printing.
//
//    Jeremy Meredith, Thu Jan  7 15:36:19 EST 2010
//    Close all open ids when returning an exception.  
//
// ****************************************************************************

void
avtGTCFileFormat::Initialize()
{
    const char *mName = "avtGTCFileFormat::Initialize: ";

    if(initialized)
        return;

    // Init HDF5 and turn off error message printing.
    H5open();
    H5Eset_auto( H5E_DEFAULT, NULL, NULL );

    //bool err = false;

    // Check for a valid GTC file
    if( H5Fis_hdf5( GetFilename() ) < 0 )
      EXCEPTION1( InvalidFilesException, GetFilename() );

    hid_t fileHandle = H5Fopen(GetFilename(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (fileHandle < 0)
      EXCEPTION1( InvalidFilesException, GetFilename() );
    
    hid_t particleHandle = H5Dopen(fileHandle, "particle_data", H5P_DEFAULT);
    if(particleHandle < 0)
    {
      particleHandle = H5Dopen(fileHandle, "Particles", H5P_DEFAULT);
      if(particleHandle < 0)
      {
        H5Fclose(fileHandle);
        EXCEPTION1( InvalidFilesException, GetFilename() );
      }
    }

    // At this point consider the file to truly be a GTC file. If
    // some other file exception will be thrown.

    //Check variable's size.
    hid_t dataspace = H5Dget_space(particleHandle);
    hsize_t dims[3];
    hid_t sid = H5Dget_space(particleHandle);
    int ndims = H5Sget_simple_extent_dims(dataspace, dims, NULL);
    if(ndims < 0 || ndims > 2)
    {
        debug4 << mName << "Could not determine number of dimensions" << endl;
        H5Sclose(sid);
        H5Dclose(particleHandle);
        H5Fclose(fileHandle);
        EXCEPTION1( InvalidVariableException, "GTC Dataset Extents - Dataset 'particle_data' has an invalid extents");
    }
    
    debug4 << mName << "Determining variable size" << endl;
    int val = H5Sget_simple_extent_dims(sid, dims, NULL);

    if(val < 0 || dims[1] < 3)
    {
        debug4 << mName << "Could not determine variable size" << endl;
        H5Sclose(sid);
        H5Dclose(particleHandle);
        H5Fclose(fileHandle);
        EXCEPTION1( InvalidVariableException, "GTC Dataset Extents - Dataset 'particle_data' has an insufficient number of variables");
    }
    H5Sclose(dataspace);

    debug4 << mName << "variable size (" << dims[0] << ", " << dims[1] << ")" << endl;

    H5Dclose(particleHandle);
    H5Fclose(fileHandle);

    nTotalPoints = dims[0];
    nVars = dims[1];

#ifdef PARALLEL
    nProcs = PAR_Size();
    rank = PAR_Rank();
    nPoints = nTotalPoints / nProcs;
    int remainder = nTotalPoints % nProcs;

    startOffset = rank * nPoints;
    if ( rank < remainder )
        startOffset += rank;
    else
        startOffset += remainder;

    if ( rank < remainder )
        nPoints++;
#else
    nPoints = nTotalPoints;
    startOffset = 0;
#endif
    
    initialized = true;
}

// ****************************************************************************
//  Method: avtGTCFileFormat::PopulateDatabaseMetaData
//
//  Purpose:
//      This database meta-data object is like a table of contents for the
//      file.  By populating it, you are telling the rest of VisIt what
//      information it can request from you.
//
//  Programmer: pugmire -- generated by xml2avt
//  Creation:   Tue Nov 20 14:08:56 PST 2007
//
// ****************************************************************************

void
avtGTCFileFormat::PopulateDatabaseMetaData(avtDatabaseMetaData *md)
{
    // Add a point mesh
    std::string meshname = "particles";
    avtMeshMetaData *mmd = new avtMeshMetaData;
    mmd->name = meshname;
    mmd->spatialDimension = 3;
    mmd->topologicalDimension = 0;
    mmd->meshType = AVT_POINT_MESH;
#ifdef PARALLEL
    mmd->numBlocks = PAR_Size();
#endif
    md->Add(mmd);

    // Add scalar variables.
    for ( int i = 3; i < nVars; i++ )
    {
        std::string var = IndexToVarName( i );
        if ( var != "" )
            AddScalarVarToMetaData(md, var, meshname, AVT_NODECENT);        
    }
}


// ****************************************************************************
//  Method: avtGTCFileFormat::GetMesh
//
//  Purpose:
//      Gets the mesh associated with this file.  The mesh is returned as a
//      derived type of vtkDataSet (ie vtkRectilinearGrid, vtkStructuredGrid,
//      vtkUnstructuredGrid, etc).
//
//  Arguments:
//      domain      The index of the domain.  If there are NDomains, this
//                  value is guaranteed to be between 0 and NDomains-1,
//                  regardless of block origin.
//      meshname    The name of the mesh of interest.  This can be ignored if
//                  there is only one mesh.
//
//  Programmer: pugmire -- generated by xml2avt
//  Creation:   Tue Nov 20 14:08:56 PST 2007
//
// ****************************************************************************

vtkDataSet *
avtGTCFileFormat::GetMesh(int domain, const char *meshname)
{
    // Allocate mesh.
    vtkPoints *points  = vtkPoints::New();
    points->SetNumberOfPoints(nPoints);
    
    vtkUnstructuredGrid  *grid = vtkUnstructuredGrid::New();
    grid->SetPoints(points);
    grid->Allocate(nPoints);

    //Read in particles.
    float *ptrXYZ = (float *) points->GetVoidPointer(0);
    ReadVariable( domain, 0, 3, &ptrXYZ );    

    // Set the IDs in the grid.
    vtkIdType vertID;
    for ( int i = 0; i < nPoints; i++ )
    {
        vertID = i;
        grid->InsertNextCell( VTK_VERTEX, 1, &vertID );
    }

    points->Delete();
    return grid;
}

// ****************************************************************************
// Method: avtGTCFileFormat::ReadVariable
//
// Purpose: 
//   Reads in the variables from the file. If parallel, then do data exchange.
//
//  Arguments:
//      domain      The index of the domain.
//      varIdx      Variable of index to be read.
//      varDim      Dimension of varible to be read.
//      ptrVar      pointer to buffer for variable.
//
// Programmer: Dave Pugmire
// Creation:   Mon Dec  3 16:17:45 EST 2007
//
// Modifications:
//   
// ****************************************************************************

void
avtGTCFileFormat::ReadVariable( int domain, int varIdx, int varDim, float **ptrVar )
{
    debug5 << "Reading Variable: " << startOffset << " " << nPoints << endl;

    hid_t fileHandle = H5Fopen(GetFilename(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (fileHandle < 0)
      EXCEPTION1( InvalidFilesException, GetFilename() );
    
    hid_t particleHandle = H5Dopen(fileHandle, "particle_data", H5P_DEFAULT);
    if(particleHandle < 0)
    {
      particleHandle = H5Dopen(fileHandle, "Particles", H5P_DEFAULT);
      if(particleHandle < 0)
      {
        H5Fclose(fileHandle);
        EXCEPTION1( InvalidFilesException, GetFilename() );
      }
    }

    hid_t dataspace = H5Dget_space(particleHandle);

    //Select the Var.
    hsize_t start[2] = { static_cast<hsize_t>(startOffset), static_cast<hsize_t>(varIdx) };
    hsize_t count[2] = { static_cast<hsize_t>(nPoints), static_cast<hsize_t>(varDim) };
    H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, start, NULL, count, NULL);

    hsize_t dataDim[1] = {static_cast<hsize_t>(nPoints*varDim)};
    hid_t memspace = H5Screate_simple(1, dataDim, NULL);
    H5Sselect_all(memspace);

    //Read the variable from file.
    float *var = new float[nPoints*varDim];
    H5Dread(particleHandle, H5T_NATIVE_FLOAT, memspace, dataspace, H5P_DEFAULT, var );
    H5Sclose(memspace);

    //Select ID
    start[0] = startOffset;
    start[1] = VarNameToIndex( "id" );
    
    count[0] = nPoints;
    count[1] = 1;    
    H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, start, NULL, count, NULL);
    
    // Read in ID.
    dataDim[0] = nPoints;
    memspace = H5Screate_simple(1, dataDim, NULL);
    H5Sselect_all(memspace);
    float *ids = new float[nPoints];
    H5Dread(particleHandle, H5T_NATIVE_FLOAT, memspace, dataspace, H5P_DEFAULT, ids );
    
    H5Sclose(memspace);
    H5Sclose(dataspace);
    H5Dclose(particleHandle);
    H5Fclose(fileHandle);

#ifdef PARALLEL
    ParallelReadVariable( domain, varDim, var, ids );
#endif

    //Put the variables into the right order.
    for ( int i = 0; i < nPoints; i++ )
    {
        int id = (int)ids[i] - startOffset - 1;
        memcpy( (void *)&((*ptrVar)[i*varDim]), (void*)&var[id*varDim], varDim*sizeof(float) );
    }

    delete [] ids;
    delete [] var;
}

// ****************************************************************************
//  Method: avtGTCFileFormat::GetVar
//
//  Purpose:
//      Gets a scalar variable associated with this file.  Although VTK has
//      support for many different types, the best bet is vtkFloatArray, since
//      that is supported everywhere through VisIt.
//
//  Arguments:
//      domain     The index of the domain.  If there are NDomains, this
//                 value is guaranteed to be between 0 and NDomains-1,
//                 regardless of block origin.
//      varname    The name of the variable requested.
//
//  Programmer: pugmire -- generated by xml2avt
//  Creation:   Tue Nov 20 14:08:56 PST 2007
//
// ****************************************************************************

vtkDataArray *
avtGTCFileFormat::GetVar(int domain, const char *varname)
{
    const char *mName = "avtGTCFileFormat::GetVar: ";

    // Determine the variable index from the varname
    int varIdx = VarNameToIndex( varname );
    if ( varIdx == -1 )
        EXCEPTION1(InvalidVariableException, varname);
    
    debug4 << mName << "We want var " << varIdx << " for " << varname << endl;

    // Allocate the variable.
    vtkFloatArray *rv = vtkFloatArray::New();
    rv->SetNumberOfTuples(nPoints);
    float *ptrVar = (float *) rv->GetVoidPointer(0);
    
    ReadVariable( domain, varIdx, 1, &ptrVar );

    return rv;
}


// ****************************************************************************
//  Method: avtGTCFileFormat::GetVectorVar
//
//  Purpose:
//      Gets a vector variable associated with this file.  Although VTK has
//      support for many different types, the best bet is vtkFloatArray, since
//      that is supported everywhere through VisIt.
//
//  Arguments:
//      domain     The index of the domain.  If there are NDomains, this
//                 value is guaranteed to be between 0 and NDomains-1,
//                 regardless of block origin.
//      varname    The name of the variable requested.
//
//  Programmer: pugmire -- generated by xml2avt
//  Creation:   Tue Nov 20 14:08:56 PST 2007
//
// ****************************************************************************

vtkDataArray *
avtGTCFileFormat::GetVectorVar(int domain,const char *varname)
{
    EXCEPTION1(InvalidVariableException, varname);
}

// ****************************************************************************
//  Method: IndexToVarName
//
//  Purpose:
//      Given an index, return the variable name.
//
//  Arguments:
//      idx        Index of variable.
//
//  Programmer: Dave Pugmire
//  Creation:   Tue Nov 20 14:08:56 PST 2007
//
// ****************************************************************************

std::string
avtGTCFileFormat::IndexToVarName( int idx ) const
{
    std::string var = "";
    if ( idx == 3 )
        var = "v_par";
    else if ( idx == 4 )
        var = "v_perp";
    else if ( idx == 5 )
        var = "weight";
    else if ( idx == 6 )
        var = "id";
    return var;
}

// ****************************************************************************
//  Method: VarNameToIndex
//
//  Purpose:
//      Given a variable name, return the index.
//
//  Arguments:
//      idx        Index of variable.
//
//  Programmer: Dave Pugmire
//  Creation:   Tue Nov 20 14:08:56 PST 2007
//
// ****************************************************************************

int
avtGTCFileFormat::VarNameToIndex( const std::string &var ) const
{
    if ( var == "v_par" )
        return 3;
    else if ( var == "v_perp" )
        return 4;
    else if ( var == "weight" )
        return 5;
    else if ( var == "id" )
        return 6;
    
    return -1;
}

#if PARALLEL

// ****************************************************************************
// Method: avtGTCFileFormat::ParallelReadVariable
//
// Purpose: 
//   Sorts the variable into bins by ID and exchanges data with other processes.
//
//  Arguments:
//      domain        Domain
//      varDim        Dimension of the variable
//      var           Variable read by this process.
//      ids           IDs for the variables.
//
// Programmer: Dave Pugmire
// Creation:   Mon Dec  3 16:17:45 EST 2007
//
// Modifications:
//   
// ****************************************************************************

void
avtGTCFileFormat::ParallelReadVariable( int domain, int varDim, float *var, float *ids )
{
    // Sort the particles into bins. We need to build a package for
    // each processor.  

    //int ptCnt = nTotalPoints / nProcs;
    //int remainder = nTotalPoints % nProcs;

    float *myVar = var, *myIds = ids;

    parallelBuffer **particleArr = new parallelBuffer*[nProcs];
    for ( int i = 0; i < nProcs; i++ )
        particleArr[i] = new parallelBuffer( varDim+1 );

    BinData( varDim, particleArr, var, ids, &myVar, &myIds );
    int *dataShareMatrix = GetDataShareMatrix( particleArr );
    CommunicateData( varDim, dataShareMatrix, particleArr, &myVar, &myIds );

    //Cleanup.
    for ( int i = 0; i < nProcs; i++ )
        delete particleArr[i];
    delete [] particleArr;
    delete [] dataShareMatrix;    
}



// ****************************************************************************
// Method: avtGTCFileFormat::BinData
//
// Purpose: 
//   Bin the data based on ID. Data that belong on other processors is
//   placed into the parallelBuffer 'array'. Data that remains on
//   "this" process, will be copied towards the front of the var/ids
//   array. myVarsPtr and myIdsPtr track the end of this array.
//
//  Arguments:
//      dim           Dimension of the variable
//      array         Array of dynamic arrays for storing bined data.
//      var           Input data variables.
//      ids           IDs for the variables.
//      myVarsPtr     Upon exit, this points to the next location in vars where data
//                    can be stored.
//      myIdsPtr      Upon exit, this points to the next location in vars where data
//                    can be stored.      
//
// Programmer: Dave Pugmire
// Creation:   Mon Dec  3 16:17:45 EST 2007
//
// Modifications:
//   
// ****************************************************************************

void
avtGTCFileFormat::BinData( int dim, parallelBuffer **array, float *vars, float *ids,
                            float **myVarsPtr, float **myIdsPtr )
{
    int ptCnt = nTotalPoints / nProcs;
    int remainder = nTotalPoints % nProcs;

    float *ptrIDs = ids, *ptrVars = vars, *myVars = *myVarsPtr, *myIds = *myIdsPtr;
    float *data = new float[dim+1];

    for ( int i = 0; i < nPoints; i++ )
    {
        int id = ((int) *ptrIDs) - 1; //Make the id 0...N-1 for the math below.
        int whichProc;

        // Determine which processor this ID goes to.
        if ( id < (remainder * (ptCnt+1)) )
            whichProc = id / (ptCnt+1);
        else
        {
            int id2 = id - (remainder * (ptCnt+1));
            whichProc = remainder + id2/ptCnt;
        }

        if ( whichProc < 0 || whichProc >= nProcs )
        {
            char str[512];
            sprintf( str, "Bad Id mapping: %d ==> %d\n", id, whichProc );
            EXCEPTION1( InvalidDBTypeException, str );
        }

        // This is our data, so copy it to the front of the var, ids arrays.
        if ( whichProc == rank )
        {
            // ID
            *myIds++ = *ptrIDs++;
            for ( int j = 0; j < dim; j++ )
                *myVars++ = *ptrVars++;
        }
        // This data isn't ours. Stash it in the buffer to be sent.        
        else   
        {
            // ID
            data[0] = *ptrIDs++;
            for ( int j = 0; j < dim; j++ )
                data[j+1] = *ptrVars++;
            array[whichProc]->Add( data );
        }
    }
    
    delete [] data;

    *myIdsPtr = myIds;
    *myVarsPtr = myVars;
}

// ****************************************************************************
// Method: avtGTCFileFormat::GetDataShareMatrix
//
// Purpose: 
//   This computes an NxN matrix, where M[i,j] contains the number of values that
//   processor i has that need to be sent to j.
//
//  Arguments:
//      array           Array of dynamic arrays. This contains data that must be
//                      communicated.
//
// Programmer: Dave Pugmire
// Creation:   Mon Dec  3 16:17:45 EST 2007
//
// Modifications:
//   
// ****************************************************************************

int *
avtGTCFileFormat::GetDataShareMatrix( parallelBuffer **array )
{
    // Binning done, now tell everyone else how many particles we have for them.
    int *particleCnts = new int[nProcs], *gatherCnts = new int[nProcs*nProcs];
    
    for ( int i = 0; i < nProcs; i++ )
        particleCnts[i] = array[i]->Size();

    // Allgather is a little overkill, but there isn't much data.
    int err = MPI_Allgather( particleCnts, nProcs, MPI_INT,
                             gatherCnts, nProcs, MPI_INT, VISIT_MPI_COMM );
    if ( err != MPI_SUCCESS )
        EXCEPTION1(InvalidDBTypeException, "GTC Reader: MPI_Allgather() failure." );

    return gatherCnts;
}

// ****************************************************************************
// Method: avtGTCFileFormat::CommunicateData
//
// Purpose: 
//   Communicate the data to the owning processors via non blocking send/recv.
//
//  Arguments:
//      dim           Dimension of the variable
//      shareMatrix   Matrix with share data.
//      array         Array of dynamic arrays for storing bined data.
//      var           Input data variables.
//      ids           IDs for the variables.
//      myVarsPtr     Upon exit, this points to the next location in vars where data
//                    can be stored.
//      myIdsPtr      Upon exit, this points to the next location in vars where data
//                    can be stored.         
//
// Programmer: Dave Pugmire
// Creation:   Mon Dec  3 16:17:45 EST 2007
//
// Modifications:
//
//  Dave Pugmire, Thu Dec 20 16:23:48 EST 2007
//  Use MPI_Waitsome instead of MPI_Waitall. Process things as they become ready.
//   
//    Jeremy Meredith, Thu Aug  7 13:58:44 EDT 2008
//    MPI_Request does not have a well-specified type by the MPI spec.
//    I removed its use from within a printf, since it was debugging code.
//
// ****************************************************************************
void
avtGTCFileFormat::CommunicateData( int dim, int *shareMatrix, parallelBuffer **array,
                                   float **myVarsPtr, float **myIdsPtr )
{
    int err;
    
    // Everyone knows what to expect now. Do the send/recvs via nonblocking send/recv.
    std::vector<MPI_Request> requests;
    std::vector<int> requestRank;
    int numSends = 0;
    for ( int i = 0; i < nProcs; i++ )
    {
        MPI_Request req;
        int sz = (dim+1) * array[i]->Size();
        if ( sz == 0 )
            continue;

        err = MPI_Isend( array[i]->Get(0), sz, MPI_FLOAT, i, rank, VISIT_MPI_COMM, &req );
        if ( err != MPI_SUCCESS )
            EXCEPTION1(InvalidDBTypeException, "GTC Reader: MPI_Isend() failure." );
        char str[512]; sprintf( str, "%d: sending to %d [%d]\n", rank, i, sz );
        debug5 << str;
        
        requests.push_back( req );
        requestRank.push_back( i );
        numSends++;
    }

    //Do the recvs.
    float **bufs = new float*[nProcs];
    float *myVars = *myVarsPtr, *myIds = *myIdsPtr;
    int numRecvs = 0;
    
    for ( int i = 0; i < nProcs; i++ )
    {
        int sz = (dim+1) * shareMatrix[i*nProcs + rank];
        bufs[i] = NULL;
        if ( i == rank || sz == 0 )
            continue;

        bufs[i] = new float[sz];
        MPI_Request req;
        err = MPI_Irecv( bufs[i], sz, MPI_FLOAT, i, i, VISIT_MPI_COMM, &req );
        if ( err != MPI_SUCCESS )
            EXCEPTION1(InvalidDBTypeException, "GTC Reader: MPI_Irecv() failure." );
        
        char str[512]; sprintf( str, "%d: receiving from %d [%d]\n", rank, i, sz );
        debug5 << str;
        
        requests.push_back( req );
        requestRank.push_back( i );
        numRecvs++;
    }

    // Process the send/recvs as they complete.
    int numRequests = numSends+numRecvs;
    if ( numRequests > 0 )
    {
        int num, nTotalReq = numRequests, *idxArray = new int[numRequests];
        MPI_Status *statusArray = new MPI_Status[numRequests];
        
        while ( numRequests > 0 )
        {
            err = MPI_Waitsome( nTotalReq, &requests[0], &num, idxArray, statusArray );
            if ( err != MPI_SUCCESS || num == MPI_UNDEFINED )
                EXCEPTION1(InvalidDBTypeException, "GTC Reader: MPI_Waitany() failure." );

            debug5 << "Waitsome=: " << num << endl;
            for ( int i = 0; i < num; i++ )
            {
                int idx = idxArray[i];
                if ( idx < numSends )
                {
                    //Nothing to do for send.
                }
                else
                {
                    int src = requestRank[idx];
                    int cnt = shareMatrix[src*nProcs + rank];

                    //Copy ID, var.
                    float *bufPtr = bufs[src];
                    for ( int j = 0; j < cnt; j++ )
                    {
                        *myIds++ = *bufPtr++; // ID
                        for ( int k = 0; k < dim; k++ )
                            *myVars++ = *bufPtr++;
                    }
                    delete [] bufs[src];
                    bufs[src] = NULL;
                }
                requests[idx] = MPI_REQUEST_NULL;
            }
            
            numRequests -= num;
        }

        delete [] idxArray;
        delete [] statusArray;
    }


    //Cleanup.
    delete [] bufs;

    *myIdsPtr = myIds;
    *myVarsPtr = myVars;
}

// ****************************************************************************
// Method: parallelBuffer::parallelBuffer
//
// Purpose: 
//   Constructor for dynamic arrays for holding data shared across processors.
//
//  Arguments:
//      elemSz        The dimension of the tuples being held.
//  
// Programmer: Dave Pugmire
// Creation:   Mon Dec  3 16:17:45 EST 2007
//
// Modifications:
//   
// ****************************************************************************

parallelBuffer::parallelBuffer( int elemSz )
{
    pArray = NULL;
    buffSize = size = 0;
    elemSize = elemSz;
}


// ****************************************************************************
// Method: parallelBuffer::~parallelBuffer
//
// Purpose: 
//   Desctructor
//
//  Arguments:
//      data     data to add.
//  
// Programmer: Dave Pugmire
// Creation:   Mon Dec  3 16:17:45 EST 2007
//
// Modifications:
//   
// ****************************************************************************

parallelBuffer::~parallelBuffer()
{
    if ( pArray )
        free( pArray );
    
    pArray = NULL;
    buffSize = size = 0;
}

// ****************************************************************************
// Method: parallelBuffer::AddElement
//
// Purpose: 
//   Add an element to the array, growing the array size if needed.
//
//  Arguments:
//      data     data to add.
//  
// Programmer: Dave Pugmire
// Creation:   Mon Dec  3 16:17:45 EST 2007
//
// Modifications:
//   
// ****************************************************************************

void
parallelBuffer::AddElement( float *data )
{
    // Allocate array if needed.
    if ( buffSize == 0 )
    {
        buffSize = 64; //Experiment to see if this is too big/too small.
        pArray = (float *)malloc( buffSize*elemSize*sizeof(float) );
    }
    // Grow array if needed.
    else if ( size == buffSize )
    {
        buffSize = buffSize*2;
        pArray = (float *)realloc( pArray, buffSize*elemSize*sizeof(float) );
    }

    //Copy into array.
    memcpy( &pArray[size*elemSize], data, elemSize*sizeof(float) );
    size++;
}

#endif
