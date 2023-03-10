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
//                          avtMTSDFileFormatInterface.C                     //
// ************************************************************************* //

#include <avtMTSDFileFormatInterface.h>

#include <vector>

#include <avtDatabaseMetaData.h>
#include <avtIOInformation.h>
#include <avtMTSDFileFormat.h>
#include <avtParallel.h>

#include <BadIndexException.h>
#include <DebugStream.h>


using std::vector;


// ****************************************************************************
//  Method: avtMTSDFileFormatInterface constructor
//
//  Arguments:
//      lst      A list of MTSD file formats.  One for each domain.
//      nLst     The number of elements in list.
//
//  Notes:       The interface owns the file formats and the array they sit on
//               after this call.
//
//  Programmer:  Hank Childs
//  Creation:    October 8, 2001
//
//  Modifications:
//
//    Hank Childs, Mon Aug 16 16:22:56 PDT 2004
//    Tell each file format what its domain is.
//
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
//
// ****************************************************************************

avtMTSDFileFormatInterface::avtMTSDFileFormatInterface(avtMTSDFileFormat ***lst,
                                                       int ntsgroups,
                                                       int nblocks)
{
    chunks = lst;
    nTimestepGroups = ntsgroups;
    nBlocks = nblocks;

    for (int i = 0 ; i < nTimestepGroups ; i++)
    {
        for (int j = 0 ; j < nBlocks ; j++)
        {
            chunks[i][j]->SetDomain(j);
        }
    }
}


// ****************************************************************************
//  Method: avtMTSDFileFormatInterface destructor
//
//  Programmer: Hank Childs
//  Creation:   October 8, 2001
//
//  Modifications:
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
// ****************************************************************************

avtMTSDFileFormatInterface::~avtMTSDFileFormatInterface()
{
    if (chunks != NULL)
    {
        for (int i = 0 ; i < nTimestepGroups ; i++)
        {
            if (chunks[i] != NULL)
            {
                for (int j = 0 ; j < nBlocks ; j++)
                {
                    if (chunks[i][j] != NULL)
                    {
                        delete chunks[i][j];
                        chunks[i][j] = NULL;
                    }
                }
                delete[] chunks[i];
                chunks[i] = NULL;
            }
        }
        delete[] chunks;
        chunks = NULL;
    }
    tsPerGroup.clear();
}


// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::GetMesh
//
//  Purpose:
//      Gets the mesh from the file format.
//
//  Arguments:
//      ts      The desired time step.
//      dom     The domain.
//      mesh    The name of the mesh.
//
//  Returns:    The vtk mesh.
//
//  Progrmamer: Hank Childs
//  Creation:   October 8, 2001
//
//  Modifications:
//
//    Hank Childs, Mon Jan 26 09:13:53 PST 2009
//    Add support for readers that do their own domain decomposition.
//
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
// ****************************************************************************

vtkDataSet *
avtMTSDFileFormatInterface::GetMesh(int ts, int dom, const char *mesh)
{
    int tsGroup = GetTimestepGroupForTimestep(ts);
    int localTS = GetTimestepWithinGroup(ts);
    if (dom < 0 || dom >= nBlocks)
    {
        if (dom == PAR_Rank())
            // Format is doing its own domain decomposition.
            dom = 0;
        else
            EXCEPTION2(BadIndexException, dom, nBlocks);
    }

    return chunks[tsGroup][dom]->GetMesh(localTS, mesh);
}


// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::GetVar
//
//  Purpose:
//      Gets the var from the file format.
//
//  Arguments:
//      ts      The desired time step.
//      dom     The domain.
//      var     The name of the var.
//
//  Returns:    The vtk variable (field).
//
//  Progrmamer: Hank Childs
//  Creation:   October 8, 2001
//
//  Modifications:
//
//    Kathleen Bonnell, Fri Feb  8 11:03:49 PST 2002
//    vtkScalars has been deprecated in VTK 4.0, use vtkDataArray instead.
//
//    Hank Childs, Mon Jan 26 09:13:53 PST 2009
//    Add support for readers that do their own domain decomposition.
//
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
// ****************************************************************************

vtkDataArray *
avtMTSDFileFormatInterface::GetVar(int ts, int dom, const char *var)
{
    int tsGroup = GetTimestepGroupForTimestep(ts);
    int localTS = GetTimestepWithinGroup(ts);
    if (dom < 0 || dom >= nBlocks)
    {
        if (dom == PAR_Rank())
            // Format is doing its own domain decomposition.
            dom = 0;
        else
            EXCEPTION2(BadIndexException, dom, nBlocks);
    }

    return chunks[tsGroup][dom]->GetVar(localTS, var);
}


// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::GetVectorVar
//
//  Purpose:
//      Gets the var from the file format.
//
//  Arguments:
//      ts      The desired time step.
//      dom     The domain.
//      var     The name of the var.
//
//  Returns:    The vtk variable (field).
//
//  Progrmamer: Hank Childs
//  Creation:   October 8, 2001
//
//  Modifications:
//
//    Kathleen Bonnell, Fri Feb  8 11:03:49 PST 2002
//    vtkVectors has been deprecated in VTK 4.0, use vtkDataArray instead.
//
//    Hank Childs, Mon Jan 26 09:13:53 PST 2009
//    Add support for readers that do their own domain decomposition.
//
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
// ****************************************************************************

vtkDataArray *
avtMTSDFileFormatInterface::GetVectorVar(int ts, int dom, const char *var)
{
    int tsGroup = GetTimestepGroupForTimestep(ts);
    int localTS = GetTimestepWithinGroup(ts);
    if (dom < 0 || dom >= nBlocks)
    {
        if (dom == PAR_Rank())
            // Format is doing its own domain decomposition.
            dom = 0;
        else
            EXCEPTION2(BadIndexException, dom, nBlocks);
    }

    return chunks[tsGroup][dom]->GetVectorVar(localTS, var);
}


// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::GetAuxiliaryData
//
//  Purpose:
//      Gets the auxiliary data from the file format.
//
//  Arguments:
//      var     The variable.
//      ts      The time step.
//      dom     The domain.
//      type    The type of auxiliary data.
//      args    Any additional arguments.
//
//  Returns:    A void * of the auxiliary data.
//
//  Programmer: Hank Childs
//  Creation:   October 8, 2001
//
//  Modifications:
//
//    Hank Childs, Mon Jan 26 09:13:53 PST 2009
//    Add support for readers that do their own domain decomposition.
//
//
//    Hank Childs, Tue Jan 27 11:08:46 PST 2009
//    Change behavior with requesting meta data for all domains.
//
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
// ****************************************************************************

void *
avtMTSDFileFormatInterface::GetAuxiliaryData(const char *var, int ts, int dom,
                          const char *type, void *args, DestructorFunction &df)
{
    int tsGroup = GetTimestepGroupForTimestep(ts);
    int localTS = GetTimestepWithinGroup(ts);
    if (dom == -1)
    {
        debug5 << "Auxiliary data was requested of multiple timestep, "
               << "single domain file format.  Since the data was requested "
               << "for all domains, requesting it of the first domain" << endl;
        if (chunks[tsGroup][0] != NULL)
            return chunks[tsGroup][0]->GetAuxiliaryData(var, localTS,
                                                        type, args, df);
        else
        {
            debug5 << "REVERSAL: chunks[...][0] is NULL!  Returning NULL" << endl;
            return NULL;
        }
    }

    if (dom < 0 || dom >= nBlocks)
    {
        if (dom == PAR_Rank())
            // Format is doing its own domain decomposition.
            dom = 0;
        else
            EXCEPTION2(BadIndexException, dom, nBlocks);
    }

    return chunks[tsGroup][dom]->GetAuxiliaryData(var, localTS, type, args, df);
}


// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::CreateCacheNameIncludingSelections
//
//  Purpose:
//      Creates a name that can be used for caching, including any of the
//      selections that may be applied.
//
//  Arguments:
//      var     The variable.
//      ts      The time step.
//      dom     The domain.
//
//  Returns:    A mangled version of the name.
//
//  Programmer: Hank Childs
//  Creation:   December 20, 2011
//
//  Modifications:
//
//    Hank Childs, Wed Jan 18 12:34:45 PST 2012
//    Fixed issue with databases that provide their own parallelism.
//
// ****************************************************************************

std::string
avtMTSDFileFormatInterface::CreateCacheNameIncludingSelections(std::string var, 
                                                               int ts, int dom)
{
    //
    // dom == -1 is used to indicate something that is valid over all domains.
    // Since there is only a single domain, all domains can be just the one
    // domain.
    //
    if (dom == -1)
    {
        dom = 0;
    }

    if (dom < 0 || dom >= nBlocks)
    {
        if (dom == PAR_Rank())
            // Format is doing its own domain decomposition.
            dom = 0;
        else
            EXCEPTION2(BadIndexException, dom, nBlocks);
    }

    int tsGroup = GetTimestepGroupForTimestep(ts);
    return chunks[tsGroup][dom]->CreateCacheNameIncludingSelections(var);
}


// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::GetFilename
//
//  Purpose:
//      Gets the name of the file we are using.
//
//  Arguments:
//      ts       The timestep
//
//  Programmer: Hank Childs
//  Creation:   March 12, 2002
//
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
// ****************************************************************************

const char *
avtMTSDFileFormatInterface::GetFilename(int ts)
{
    int tsGroup = GetTimestepGroupForTimestep(ts);
    return chunks[tsGroup][0]->GetFilename();
}


// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::SetDatabaseMetaData
//
//  Purpose:
//      Gets the database meta-data.  This is a tricky issue because we don't
//      want to read in the table of contents for every file.  Just read it
//      in for the first one and try to add some time step information.
//
//  Programmer: Hank Childs
//  Creation:   October 8, 2001
//
//  Modifications:
//
//    Hank Childs, Wed Mar  6 10:19:31 PST 2002
//    Do not honor extents from file formats if we know we have multiple
//    blocks.
//
//    Hank Childs, Mon Mar 11 09:42:39 PST 2002
//    Indicate our confidence in the cycle numbers.
//
//    Brad Whitlock, Wed May 14 09:43:16 PDT 2003
//    Added int argument to conform to new method prototype.
//
//    Brad Whitlock, Mon Oct 13 13:46:43 PST 2003
//    Added code to make sure that the times are also populated.
//
//    Mark C. Miller, Tue May 17 18:48:38 PDT 2005
//    Revamped the code to deal with getting cycles/times to make it more
//    consistent with other plugin types. Added anonymous bool for force read
//    all cycles/times to satisfy interface. Here, that bool can be ignored
//    because issues with opening many files, etc. to obtain that information
//    are not VisIt's concern. It is the plugin developers. Even if VisIt
//    should be concerned with it, there isn't much VisIt can do except pass
//    the request down to the plugin
//
//    Mark C. Miller, Tue May 31 20:12:42 PDT 2005
//    Replaced -INT_MAX & -DBL_MAX with INVALID_CYCLE and INVALID_TIME
//
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
//
//    Hank Childs, Sun May  9 17:53:17 CDT 2010
//    Register metadata object with all of the chunks.
//
//    Hank Childs, Fri Jul 23 22:38:54 PDT 2010
//    Add check for degenerate case where file format reader gets times array
//    really wrong, which was then causing crash.
//
//    Hank Childs, Fri Aug 27 13:46:28 PDT 2010
//    Don't overwrite values if they are not monotonic.
//
//    Hank Childs, Tue Aug 31 16:31:07 PDT 2010
//    Don't uniformly declare all times as inaccurate if they are out of 
//    sequence.
//
//    Hank Childs, Tue Apr 10 15:12:01 PDT 2012
//    Make use of argument for whether we should force reading of all
//    cycles and times.
//
//    Dave Pugmire, Fri Feb  8 17:22:01 EST 2013
//    Added support for ensemble databases. (multiple time values)
//
//    Kathleen Biagas, Wed Aug 26 16:32:52 PDT 2015
//    Correct order of arguments in calls to md->SetCycleIsAccurate and
//    md->SetTimeIsAccurate.
//
// ****************************************************************************

void
avtMTSDFileFormatInterface::SetDatabaseMetaData(avtDatabaseMetaData *md,
    int timeState, bool forceReadAllCyclesTimes)
{

    GenerateTimestepCounts();

    //
    // Throw an exception if an invalid time state was requested.
    //
    if (timeState < 0 || timeState >= nTotalTimesteps)
    {
        EXCEPTION2(BadIndexException, timeState, nTotalTimesteps);
    }

    //
    // We know for sure that the number of states is the number of timesteps.
    //
    md->SetNumStates(nTotalTimesteps);

    //
    // Have a _domain timestep populate what variables, meshes, materials, etc
    // we can deal with for the whole database.  This is bad, but the
    // alternative (to read in all the files) is unattractive and we may not
    // know what to do with a variable that is in some domains and not others
    // anyways. This may wind up setting cycles/times, too.
    //
    int tsGroup = GetTimestepGroupForTimestep(timeState);
    int localTS = GetTimestepWithinGroup(timeState);
    int offset = 0;
    for (int i = 0 ; i < nTimestepGroups ; i++)
    {
        chunks[i][0]->SetTimeSliceOffset(offset);
        offset += tsPerGroup[i];
    }
    if (forceReadAllCyclesTimes)
        chunks[tsGroup][0]->SetReadAllCyclesAndTimes(true);
    else
        chunks[tsGroup][0]->SetReadAllCyclesAndTimes(false);
    chunks[tsGroup][0]->SetDatabaseMetaData(md, localTS);
    for (int i = 0 ; i < nTimestepGroups ; i++)
        if (i != tsGroup)
            chunks[i][0]->RegisterDatabaseMetaData(md);

    //
    // Note: In an MTXX format, a single file has multiple time steps in it
    // So, we don't have the same kinds of semantics we do with STXX databases
    // in, for example, trying to guess cycle numbers from file names
    //
    if (md->AreAllCyclesAccurateAndValid(nTotalTimesteps) != true)
    {
        vector<int> cycles;
        for (size_t i=0; i<(size_t)nTimestepGroups; i++)
        {
            vector<int> tmp;
            chunks[i][0]->FormatGetCycles(tmp);
            cycles.insert(cycles.end(),tmp.begin(),tmp.end());
        }
        bool cyclesLookGood = true;
        if (!isEnsemble)
        {
            for (size_t i = 0; i < cycles.size(); i++)
            {
                if ((i != 0) && (cycles[i] <= cycles[i-1]))
                {
                    cyclesLookGood = false;
                    break;
                }
            }
        }
        if (cycles.size() != (size_t)nTotalTimesteps)
            cyclesLookGood = false;
        if (cyclesLookGood == false)
        {
            std::vector<int> cyclesFromMassCall = cycles;
            cycles.clear();
            for (int i = 0; i < nTotalTimesteps; i++)
            {
                int tsg = GetTimestepGroupForTimestep(i);
                int lts = GetTimestepWithinGroup(i);
                int c = chunks[tsg][0]->FormatGetCycle(lts);
                if (c == avtFileFormat::INVALID_CYCLE && (int)cyclesFromMassCall.size() > i)
                    c = cyclesFromMassCall[i];

                if (c == avtFileFormat::INVALID_CYCLE)
                {
                    if (i == 0)
                        cycles.push_back(0);
                    else
                        cycles.push_back(cycles[i-1]+1);
                    md->SetCycleIsAccurate(false, i);
                }
                else
                    cycles.push_back(c);
            }
        }

        //
        // Ok, now put cycles into the metadata
        //
        md->SetCycles(cycles);
        if (cyclesLookGood)
            md->SetCyclesAreAccurate(cyclesLookGood);
    }

    if (md->AreAllTimesAccurateAndValid(nTotalTimesteps) != true)
    {
        // Set the times in the metadata.
        vector<double> times;
        for (int i=0; i<nTimestepGroups; i++)
        {
            vector<double> tmp;
            chunks[i][0]->FormatGetTimes(tmp);
            times.insert(times.end(),tmp.begin(),tmp.end());
        }
        bool timesLookGood = true;
        if (!isEnsemble)
        {
            for (size_t i = 0; i < times.size(); i++)
            {
                if ((i != 0) && (times[i] <= times[i-1]))
                {
                    timesLookGood = false;
                    break;
                }
            }
        }
        if (times.size() != (size_t)nTotalTimesteps)
            timesLookGood = false;

        if (0 && timesLookGood == false)
        {
            vector<double> timesFromMassCall = times;

            times.clear();
            for (int i = 0; i < nTotalTimesteps; i++)
            {
                int tsg = GetTimestepGroupForTimestep(i);
                int lts = GetTimestepWithinGroup(i);
                double t = chunks[tsg][0]->FormatGetTime(lts);
                if (t == avtFileFormat::INVALID_TIME && (int)timesFromMassCall.size() > i)
                    t = timesFromMassCall[i];

                if (t != avtFileFormat::INVALID_TIME)
                    times.push_back(t);
                else
                {
                    if (i == 0)
                        times.push_back(0.);
                    else
                        times.push_back(times[i-1]+0.0);  // same time
                    md->SetTimeIsAccurate(false, i);
                }
            }
        }

        //
        // Ok, now put times into the metadata
        //
        md->SetTimes(times);
        if (timesLookGood && times.size() > 0)
        {
            md->SetTimesAreAccurate(true);
            md->SetTemporalExtents(times[0], times[times.size() - 1]);
        }
    }

    //
    // Each one of these domains thinks that it only has one domain.  Overwrite
    // that with the true number of domains.
    //
    int nm = md->GetNumMeshes();
    for (int i = 0 ; i < nm ; i++)
    {
        md->SetBlocksForMesh(i, nBlocks);
    }

    if (nBlocks > 1)
    {
        md->UnsetExtents();
    }
}

// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::SetCycleTimeInDatabaseMetaData
//
//  Purpose: Populate cycle/time in metadata for all times if possible or
//    just current timestate
//
//  Programmer: Mark C. Miller 
//  Creation:   May 31, 2005 
//
//  Modifications:
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
// ****************************************************************************

void
avtMTSDFileFormatInterface::SetCycleTimeInDatabaseMetaData(
    avtDatabaseMetaData *md, int timeState)
{
    GenerateTimestepCounts();

    //
    // Throw an exception if an invalid time state was requested.
    //
    if (timeState < 0 || timeState >= nTotalTimesteps)
    {
        EXCEPTION2(BadIndexException, timeState, nTotalTimesteps);
    }

    vector<int> cycles;
    for (int i=0; i<nTimestepGroups; i++)
    {
        vector<int> tmp;
        chunks[i][0]->FormatGetCycles(tmp);
        cycles.insert(cycles.end(),tmp.begin(),tmp.end());
    }
    bool cyclesLookGood = true;
    if (!isEnsemble)
    {
        for (size_t i = 0; i < cycles.size(); i++)
        {
            if ((i != 0) && (cycles[i] <= cycles[i-1]))
            {
                cyclesLookGood = false;
                break;
            }
        }
    }
    if (cycles.size() != (size_t)nTotalTimesteps)
        cyclesLookGood = false;
    if (cyclesLookGood == false)
    {
        int tsg = GetTimestepGroupForTimestep(timeState);
        int lts = GetTimestepWithinGroup(timeState);
        int c = chunks[tsg][0]->FormatGetCycle(lts);
        if (c != avtFileFormat::INVALID_CYCLE)
        {
            md->SetCycle(timeState, c);
            md->SetCycleIsAccurate(true, timeState);
        }
    }
    else
    {
        md->SetCycles(cycles);
        md->SetCyclesAreAccurate(true);
    }

    vector<double> times;
    for (int i=0; i<nTimestepGroups; i++)
    {
        vector<double> tmp;
        chunks[i][0]->FormatGetTimes(tmp);
        times.insert(times.end(),tmp.begin(),tmp.end());
    }
    bool timesLookGood = true;
    for (size_t i = 0; i < times.size(); i++)
    {
        if ((i != 0) && (times[i] <= times[i-1]))
        {
            timesLookGood = false;
            break;
        }
    }
    if (times.size() != (size_t)nTotalTimesteps)
        timesLookGood = false;
    if (timesLookGood == false)
    {
        int tsg = GetTimestepGroupForTimestep(timeState);
        int lts = GetTimestepWithinGroup(timeState);
        double t = chunks[tsg][0]->FormatGetTime(lts);
        if (t != avtFileFormat::INVALID_TIME)
        {
            md->SetTime(timeState, t);
            md->SetTimeIsAccurate(true, timeState);
        }
    }
    else
    {
        md->SetTimes(times);
        md->SetTimesAreAccurate(true);
        md->SetTemporalExtents(times[0], times[times.size() - 1]);
    }
}

// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::FreeUpResources
//
//  Purpose:
//      Frees up the resources for the file (memory, file descriptors).
//
//  Programmer: Hank Childs
//  Creation:   October 8, 2001
//
//  Modifications:
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
// ****************************************************************************

void
avtMTSDFileFormatInterface::FreeUpResources(int ts, int)
{
    if (ts == -1)
    {
        for (int i = 0 ; i < nTimestepGroups ; i++)
        {
            for (int j = 0 ; j < nBlocks ; j++)
            {
                chunks[i][j]->FreeUpResources();
            }
        }
    }
}

// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::ActivateTimestep
//
//  Purpose: Notify the format of our intention to read data for a given
//  timestep. This gives the format an opportunity to do whatever 
//  parallel collective work it might need to for the given timestep
//
//  Programmer: Mark C. Miller 
//  Creation:   February 23, 2004 
//
//  Modifications:
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
//
// ****************************************************************************
void
avtMTSDFileFormatInterface::ActivateTimestep(int ts)
{
    GenerateTimestepCounts();

    int tsGroup = GetTimestepGroupForTimestep(ts);
    int localTS = GetTimestepWithinGroup(ts);
    for (int i = 0 ; i < nBlocks ; i++)
    {
        chunks[tsGroup][i]->ActivateTimestep(localTS);
    }
}

// ****************************************************************************
//  Method: avtMTSDFileFormatInterface::PopulateIOInformation
//
//  Purpose: Populate information regarding domain's assignment to files 
//
//  Programmer: Mark C. Miller 
//  Creation:   March 16, 2004 
//
//  Modifications:
//    Jeremy Meredith, Thu Jan 28 13:11:07 EST 2010
//    MTSD files can now be grouped not just into a faux MD format by having
//    more than one block, but also into a longer sequence of MT files,
//    each chunk with one or more timesteps.
//
//    Brad Whitlock, Thu Jun 19 11:07:28 PDT 2014
//    Pass the mesh name.
//
// ****************************************************************************

bool
avtMTSDFileFormatInterface::PopulateIOInformation(int ts, const std::string &meshname,
    avtIOInformation &ioInfo)
{
    int tsGroup = GetTimestepGroupForTimestep(ts);
    int localTS = GetTimestepWithinGroup(ts);
    bool retval = false;
    for (int i = 0 ; i < nBlocks ; i++)
    {
        retval |= chunks[tsGroup][i]->PopulateIOInformation(localTS, meshname, ioInfo);
    }
    return retval;
}


// ****************************************************************************
// Method:  avtMTSDFileFormatInterface::GetTimestepGroupForTimestep
//
// Purpose:
//   Find the timestep group containing the given timestep.
//
// Arguments:
//   ts         the timestep
//
// Programmer:  Jeremy Meredith
// Creation:    January 28, 2010
//
// ****************************************************************************
int
avtMTSDFileFormatInterface::GetTimestepGroupForTimestep(int ts)
{
    int group = 0;
    while (group < (int)tsPerGroup.size() &&
           tsPerGroup[group] <= ts)
    {
        ts -= tsPerGroup[group];
        ++group;
    }
    if (group >= (int)tsPerGroup.size())
    {
        EXCEPTION2(BadIndexException, group, (int)tsPerGroup.size());
    }
    return group;
}

// ****************************************************************************
// Method:  avtMTSDFileFormatInterface::GetTimestepWithinGroup
//
// Purpose:
//   Find the "local timestep", i.e. the index within the timestep-group
//   containing the given timestep.
//
// Arguments:
//   ts         the timestep
//
// Programmer:  Jeremy Meredith
// Creation:    January 28, 2010
//
// ****************************************************************************
int
avtMTSDFileFormatInterface::GetTimestepWithinGroup(int ts)
{
    int group = GetTimestepGroupForTimestep(ts);
    int base = 0;
    for (int i=0; i<group; i++)
        base += tsPerGroup[i];
    return ts - base;
}

// ****************************************************************************
// Method:  avtMTSDFileFormatInterface::GenerateTimestepCounts
//
// Purpose:
//   Generate the local information used to map local<->global time steps.
//
// Arguments:
//   none
//
// Programmer:  Jeremy Meredith
// Creation:    January 28, 2010
//
// ****************************************************************************
void
avtMTSDFileFormatInterface::GenerateTimestepCounts()
{
    //
    // Count up the time steps from each timestep group
    //
    tsPerGroup.clear();
    nTotalTimesteps = 0;
    for (int i=0; i<nTimestepGroups; i++)
    {
        int n = chunks[i][0]->GetNTimesteps();
        tsPerGroup.push_back(n);
        nTotalTimesteps += n;
    }
}
