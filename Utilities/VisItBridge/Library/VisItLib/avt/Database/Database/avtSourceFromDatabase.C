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
//                          avtSourceFromDatabase.C                          //
// ************************************************************************* //

#include <avtSourceFromDatabase.h>
#include <avtExecutionManager.h>

#include <vtkDataSet.h>

#include <string>
#include <vector>

#include <avtDatasetDatabase.h>
#include <avtDatabaseMetaData.h>
#include <avtDataset.h>
#include <avtSILRestriction.h>
#include <avtSILRestrictionTraverser.h>
#include <avtVariableCache.h>

#include <BadIndexException.h>
#include <DebugStream.h>
#include <ParsingExprList.h>
#include <TimingsManager.h>


using     std::string;
using     std::vector;

ExpressionList   *avtSourceFromDatabase::lastExprList = NULL;


// ****************************************************************************
//  Method: avtSourceFromDatabase constructor
//
//  Arguments:
//      db      The database this source is connected to.
//      vn      The name of the variable this source corresponds to.
//      ts      The timestep this variable corresponds to.
//
//  Programmer: Hank Childs
//  Creation:   August 9, 2000
//
//  Modifications:
//
//    Hank Childs, Fri May 11 08:44:23 PDT 2001
//    Hooked load balancer into hint mechanism for I/O optimization.
//
//    Hank Childs, Wed Jun  6 13:40:28 PDT 2001
//    Removed argument for the domains.
//
// ****************************************************************************

avtSourceFromDatabase::avtSourceFromDatabase(avtDatasetDatabase *db, 
                                             const char *vn, int ts)
{
    database = db;

    variable = new char[strlen(vn)+1];
    strcpy(variable, vn);
    timestep = ts;

    lastSpec = NULL;
}


// ****************************************************************************
//  Method: avtSourceFromDatabase destructor
//
//  Programmer: Hank Childs
//  Creation:   March 2, 2001
//
// ****************************************************************************

avtSourceFromDatabase::~avtSourceFromDatabase()
{
    if (variable != NULL)
    {
        delete [] variable;
        variable = NULL;
    }
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::FetchDataset
//
//  Purpose:
//      Fetches the specified dataset.  This means going to the database and
//      requesting it.
//
//  Arguments:
//      spec    A specification of the dataset.
//      tree    A place to put the dataset.
//
//  Returns:    Whether or not the data has changed upstream (this always
//              returns false).
//
//  Programmer: Hank Childs
//  Creation:   June 6, 2001
//
//  Modifications:
//
//    Hank Childs, Wed Jul 25 15:40:54 PDT 2001
//    Made the database always return true so that dynamic load balancing will
//    be happier.  This is a temporary thing.
//
//    Hank Childs, Thu Jul 26 19:23:15 PDT 2001
//    Put in the hooks to definitively say if the request is different from
//    the previous one by comparing SILs.
//
//    Kathleen Bonnell, Wed Sep 19 13:45:33 PDT 2001 
//    Prune the tree if the request is different. 
//    
//    Hank Childs, Mon Mar 11 13:39:35 PST 2002
//    Add cycle and time.
//
//    Brad Whitlock, Wed Jul 3 13:41:18 PST 2002
//    Replaced the "/" string with SLASH_STRING so it will work on Windows.
//
//    Jeremy Meredith, Thu Aug 29 15:21:10 PDT 2002
//    I replaced the strstr call with a strpbrk so that I could remove
//    visit-config from the included headers.
//
//    Hank Childs, Fri Nov 22 16:52:14 PST 2002
//    Use a SIL restriction traverser to get the domain list.
//
//    Hank Childs, Fri May 16 10:14:40 PDT 2003
//    Catch exceptions thrown by the database.
//
//    Hank Childs, Thu Sep 25 16:30:09 PDT 2003
//    Make sure the variable being sent up to the database is a variable
//    contained within that database.
//
//    Hank Childs, Tue Sep 30 15:37:44 PDT 2003
//    Made sure input downstream would be digestible by all filters.
//
//    Hank Childs, Fri Oct 17 21:56:36 PDT 2003
//    Don't crash when NULL filenames are encountered.
//
//    Hank Childs, Sun Mar  6 11:15:11 PST 2005
//    Add special support for NeedBoundarySurfaces in lieu of fix for '5723.
//
//    Mark C. Miller, Tue May 31 20:12:42 PDT 2005
//    Made call to GetMetaData set flag to force it to read the current
//    timeState's cycle/time information.
//
//    Hank Childs, Fri Sep 23 10:23:01 PDT 2005
//    No longer use the DBVariable to make a new data specification.  The EEF
//    will guarantee that the variable list coming to the database will 
//    contain only variables that the database is familiar with.
//
//    Hank Childs, Wed Jul 25 14:16:36 PDT 2007
//    Renamed method: NeedBoundarySurfaces -> GetBoundarySurfaceRepresentation.
//
//    Hank Childs, Mon Dec  1 15:41:49 PST 2008
//    Removed code thought to be no longer necessary to prune trees.
//
//    Hank Childs, Tue Jan 11 10:35:35 PST 2011
//    Set the output data set's time index. 
//
// ****************************************************************************

bool
avtSourceFromDatabase::FetchDataset(avtDataRequest_p spec,
                                    avtDataTree_p &tree)
{
    TRY
    {
        tree = database->GetOutput(spec, this);
    }
    CATCH2(VisItException, e)
    {
        debug1 << "Catching the exception at the database level." << endl;
        avtDataValidity &v = GetOutput()->GetInfo().GetValidity();
        v.ErrorOccurred();
        string tmp = e.Message(); // Otherwise there is a const problem.
        v.SetErrorMessage(tmp);
        tree = new avtDataTree();
    }
    ENDTRY

    bool rv;
    if (*lastSpec == NULL)
    {
        rv = true;
    }
    else if (**lastSpec == **spec)
    {
        rv = false;
    }
    else
    {
        rv = true;
    }

/* THE CALL TO GetDomainList IS NOT CHEAP AND I DO NOT BELIEVE THIS CODE
 * IS DOING ANYTHING MEANINGFUL ANYMORE, SO I'M COMMENTING IT OUT.
 * (I am not removing it because I want to leave a visible reminder for)
 * (the next few months in case something weird happens.)
    if (rv)
    {
        vector<int> list;
        avtSILRestrictionTraverser trav(spec->GetRestriction());
        trav.GetDomainList(list);
        int t0 = visitTimer->StartTimer();
        tree = tree->PruneTree(list);
        visitTimer->StopTimer(t0, "PruneTree from avtSourceFromDatabase");
    }
    else
 */
    {
        //
        // PruneTree does an additional service that is a bit hidden.  If the
        // output from the database is NULL, many of our filters will choke
        // on that.  PruneTree puts it in a more digestible form.  The "right"
        // thing to do in this case is to not call GetOutput on the database
        // when the last data specification is the same as the current one.
        // However, I'm a bit scared to do that because it may break other
        // things.  In addition, if the database threw an error, we are not
        // catching that.  So, if an error was thrown, and we did re-work the
        // logic to not call "GetOutput" unnecessarily, we would have to
        // remember the error and re-call it in that case.
        //
        if ((*tree == NULL) ||
            (!tree->HasData() && (tree->GetNChildren() == 0)))
        {
            tree = new avtDataTree();
        }
    }

    const bool forceReadAllCyclesTimes = false;
    const bool forceReadThisStateCycleTime = true;
    int timestep = spec->GetTimestep();
    avtDatabaseMetaData *md = database->GetMetaData(timestep,
                                                    forceReadAllCyclesTimes,
                                                    forceReadThisStateCycleTime);
    avtDataAttributes &atts = GetOutput()->GetInfo().GetAttributes();
    if (md->IsCycleAccurate(timestep))
    {
        int cycle = md->GetCycles()[timestep];
        atts.SetCycle(cycle);
    }
    if (md->IsTimeAccurate(timestep))
    {
        double dtime = md->GetTimes()[timestep];
        atts.SetTime(dtime);
    }
    atts.SetTimeIndex(timestep);

    const char *filename = database->GetFilename(timestep);
    if (filename != NULL)
    {
        const char *latest, *greatest;
        greatest = filename;
        latest   = filename;
        while (latest != NULL)
        {
            latest = strpbrk(greatest, "\\/");
            if (latest != NULL)
            {
                greatest = latest+1;
            }
        }
        atts.SetFilename(string(greatest));
    }

    // '5723 BEGIN
    bool addBoundarySurf = false;
    if (*lastSpec != NULL && lastSpec->GetBoundarySurfaceRepresentation() &&
        strcmp(spec->GetVariable(), lastSpec->GetVariable()) == 0)
        addBoundarySurf = true;
    // '5723 END

    //
    // We can't just share a reference to that specification, because it might
    // change.  No good copy constructor for data specification, so make use
    // a two-step process.
    //
    lastSpec = new avtDataRequest(spec, 0);
    lastSpec = spec;

    // '5723 BEGIN
    if (addBoundarySurf)
        lastSpec->TurnBoundarySurfaceRepresentationOn();
    // '5723 END

    return rv;
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::FetchVariableAuxiliaryData
//
//  Purpose:
//      Goes to the database to get auxiliary data.
//
//  Arguments:
//      type    The type of auxiliary data.
//      args    Additional arguments.
//      spec    The data specification.
//      output  A place to put the output void * reference pointers.
//
//  Programmer: Hank Childs
//  Creation:   March 1, 2001
//
// ****************************************************************************

void
avtSourceFromDatabase::FetchVariableAuxiliaryData(const char *type, void *args,
                              avtDataRequest_p spec, VoidRefList &output)
{
    database->GetAuxiliaryData(spec, output, type, args);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::FetchMeshAuxiliaryData
//
//  Purpose:
//      Goes to the database to get auxiliary data.
//
//  Arguments:
//      type    The type of auxiliary data.
//      args    Additional arguments.
//      spec    The data specification.
//      output  A place to put the output void * reference pointers.
//
//  Programmer: Hank Childs
//  Creation:   March 1, 2001
//
//  Modifications:
//    Kathleen Bonnell, Mon Jun 28 07:48:55 PDT 2004
//    Use timestep from DataRequest.
//
// ****************************************************************************

void
avtSourceFromDatabase::FetchMeshAuxiliaryData(const char *type, void *args,
                              avtDataRequest_p spec, VoidRefList &output)
{
    //
    // We only have hooks to the variable defined on the mesh.  We can get
    // the mesh name by accessing the database object.
    //
    string mn = database->GetMetaData(spec->GetTimestep())->MeshForVar(variable);
    avtDataRequest_p newspec =new avtDataRequest(spec, mn.c_str());
    database->GetAuxiliaryData(newspec, output, type, args);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::FetchMaterialAuxiliaryData
//
//  Purpose:
//      Goes to the database to get auxiliary data.
//
//  Arguments:
//      type    The type of auxiliary data.
//      args    Additional arguments.
//      dl      The domains to get the data for.
//
//  Returns:    The data as void *.
//
//  Programmer: Hank Childs
//  Creation:   April 10, 2001
//
//  Modifications:
//    Kathleen Bonnell, Mon Jun 28 07:48:55 PDT 2004
//    Use timestep from DataRequest.
//
// ****************************************************************************

void 
avtSourceFromDatabase::FetchMaterialAuxiliaryData(const char *type, void *args,
                              avtDataRequest_p spec, VoidRefList &output)
{
    //
    // We only have hooks to the variable defined on the material.  We can get
    // the material name by accessing the database object.
    //
    string mn   = database->GetMetaData(spec->GetTimestep())->MeshForVar(variable);
    string mat  = database->GetMetaData(spec->GetTimestep())->MaterialOnMesh(mn);
    avtDataRequest_p newspec =new avtDataRequest(spec, mat.c_str());
    database->GetAuxiliaryData(newspec, output, type, args);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::FetchSpeciesAuxiliaryData
//
//  Purpose:
//      Goes to the database to get species auxiliary data.
//
//  Arguments:
//      type    The type of auxiliary data.
//      args    Additional arguments.
//      dl      The domains to get the data for.
//
//  Returns:    The data as void *.
//
//  Programmer: Jeremy Meredith
//  Creation:   June  8, 2004
//
//  Modifications:
//    Kathleen Bonnell, Mon Jun 28 07:48:55 PDT 2004
//    Use timestep from DataRequest.
//
// ****************************************************************************

void 
avtSourceFromDatabase::FetchSpeciesAuxiliaryData(const char *type, void *args,
                              avtDataRequest_p spec, VoidRefList &output)
{
    //
    // We only have hooks to the variable defined on the species.  We can get
    // the species name by accessing the database object.
    //
    string mn   = database->GetMetaData(spec->GetTimestep())->MeshForVar(variable);
    string sp   = database->GetMetaData(spec->GetTimestep())->SpeciesOnMesh(mn);
    avtDataRequest_p newspec =new avtDataRequest(spec, sp.c_str());
    database->GetAuxiliaryData(newspec, output, type, args);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::GetFullDataRequest
//
//  Purpose:
//      Gets the full database specification for this variable on this
//      database.
//
//  Programmer: Hank Childs
//  Creation:   June 13, 2001
//
//  Modifications:
//
//    Hank Childs, Thu Jul 26 12:59:02 PDT 2001
//    Added logic to set the top set.
//
//    Hank Childs, Thu Sep 25 16:27:38 PDT 2003
//    If a 'last spec' is available, use its variable.
//
//    Hank Childs, Sun Mar  6 11:15:11 PST 2005
//    Add special support for NeedBoundarySurfaces in lieu of fix for '5723.
//
//    Hank Childs, Fri Sep 23 10:23:01 PDT 2005
//    Return a data specification with the original variable.  This is the
//    variable that the pipeline is plotting.
//
//    Hank Childs, Wed Jul 25 14:16:36 PDT 2007
//    Renamed method: NeedBoundarySurfaces -> GetBoundarySurfaceRepresentation.
//
// ****************************************************************************

avtDataRequest_p
avtSourceFromDatabase::GetFullDataRequest(void)
{
    const char *acting_var = variable;
    if (*lastSpec != NULL)
        acting_var = lastSpec->GetOriginalVariable();

    avtSIL *sil = database->GetSIL(timestep);
    avtSILRestriction_p silr = new avtSILRestriction(sil);

    //
    // Indicate which mesh is the mesh we are interested in.
    //
    avtDatabaseMetaData *md = database->GetMetaData(timestep);
    string mesh = md->MeshForVar(variable);
    silr->SetTopSet(mesh.c_str());

    avtDataRequest_p rv =
                          new avtDataRequest(acting_var, timestep, silr);

    // '5723 BEGIN
    if (*lastSpec != NULL)
        if (lastSpec->GetBoundarySurfaceRepresentation())
            rv->TurnBoundarySurfaceRepresentationOn();
    // '5723 END

    return rv;
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::InvalidateZones
//
//  Purpose:
//      Goes to its output and indicates that the zones are now invalid.
//
//  Programmer: Hank Childs
//  Creation:   August 13, 2001
//
// ****************************************************************************

void
avtSourceFromDatabase::InvalidateZones()
{
    avtDataObject_p output = GetOutput();
    avtDataValidity &validity = output->GetInfo().GetValidity();
    validity.InvalidateZones();
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::DatabaseProgress
//
//  Purpose:
//      Makes a callback indicating the progress.
//
//  Arguments:
//      cur       The current domain.
//      tot       The total number of domains.
//      desc      The description of the database progress.
//
//  Programmer:   Hank Childs
//  Creation:     August 13, 2001
//
// ****************************************************************************

void
avtSourceFromDatabase::DatabaseProgress(int cur, int tot, const char *desc)
{
    UpdateProgress(cur, tot, "Reading from database", desc);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::NumStagesForFetch
//
//  Purpose:
//      Determines how many stages the database wants to do a fetch in.
//
//  Arguments:
//      spec    A data specification (sometimes needed to determine if
//                                    matsel is done, etc).
//
//  Returns:    The number of stages the database will do during the fetch.
//
//  Programmer: Hank Childs
//  Creation:   October 26, 2001
//
// ****************************************************************************

int
avtSourceFromDatabase::NumStagesForFetch(avtDataRequest_p spec)
{
    return database->NumStagesForFetch(spec);
}

// ****************************************************************************
//  Method: avtSourceFromDatabase::Query
//
//  Purpose:
//      Allows the database to return information about a particular cell and 
//      variables as specified by PickAttributes. 
//
//  Arguments:
//      pa      The PickAttributes that hold query information.
//
//  Programmer: Kathleen Bonnell
//  Creation:   November 15, 2002 
//
// ****************************************************************************

void
avtSourceFromDatabase::Query(PickAttributes *pa)
{
    database->Query(pa);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::FindElementForPoint
//
//  Purpose:
//      Allows the database to return the cell containing a point, or the 
//      closest node to the point (which one specified by elType).
//
//  Returns:
//      True for successful search, false otherwise.
// 
//  Arguments:
//    var       The variable to use in searching the database.
//    ts        The timestep to use in searching the database.
//    dom       The domain to use in searching the database.
//    elType    Specified which type of element (node, zone) to search for.
//    pt        The point to use in searching the database.
//    elNum     A place to store the zone or node number associated with the
//              point pt.
//
//  Programmer: Kathleen Bonnell
//  Creation:   November 13, 2003 
//
// ****************************************************************************

bool
avtSourceFromDatabase::FindElementForPoint(const char *var, const int ts,
    const int dom, const char *elType, double pt[3], int &elNum)
{
    return database->FindElementForPoint(var, ts, dom, elType, pt, elNum);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::GetDomainName
//
//  Purpose:
//      Allows the database to return a string representing the name
//      of the given domain. 
//
//  Arguments:
//    var       The variable to use in searching the database.
//    ts        The timestep to use in searching the database.
//    dom       The domain to use in searching the database.
//    domName   A place to store the domain name. 
//
//  Programmer: Kathleen Bonnell
//  Creation:   December 22, 2003 
//
// ****************************************************************************
void
avtSourceFromDatabase::GetDomainName(const std::string &var, const int ts,
    const int dom, std::string &domName)
{
    database->GetDomainName(var, ts, dom, domName);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::QueryCoords
//
//  Purpose:    A query to find the coordinates of a zone or node. 
//
//  Arguments:
//    var       The variable to use in searching the database.
//    dom       The domain to use in searching the database.
//    id        The zone or node to use in searching the database.
//    ts        The timestep to use in searching the database.
//    coord     A place to store the zone center. 
//    forZone   A flag indicating if the query is for zone or node. 
//
//  Returns:    True if the query was successful, false otherwise.
//
//  Programmer: Kathleen Bonnell
//  Creation:   May 25, 2004 
//
//  Modifications:
//    Kathleen Bonnell, Thu Jun 10 18:15:11 PDT 2004
//    Renamed from QueryZoneCenter to QueryCoords, added bool arg. 
//
//    Kathleen Bonnell, Thu Dec 16 17:11:19 PST 2004
//    Added another bool arg.
//
//    Kathleen Bonnell, Tue Jan 25 07:59:28 PST 2005 
//    Added const char* arg to QueryCoords.
//
// ****************************************************************************

bool
avtSourceFromDatabase::QueryCoords(const std::string &var, const int dom,
    const int zone, const int ts, double coord[3], const bool forZone,
    const bool useGlobalId, const char *mn)
{
    return database->QueryCoords(var, dom, zone, ts, coord, forZone, useGlobalId, mn);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::GetSIL
//
//  Purpose:
//    Returns the SIL for the given timestate. 
//
//  Arguments:
//    ts        The timestep to use in searching the database.
//
//  Programmer: Kathleen Bonnell
//  Creation:   January 3, 2005 
//
// ****************************************************************************

avtSIL*
avtSourceFromDatabase::GetSIL(int timestate)
{
    return database->GetSIL(timestate);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::ReleaseData
//
//  Purpose:
//      Releases the data associated with the input database.
//
//  Programmer: Hank Childs
//  Creation:   February 27, 2005
//
//  Modifications:
//
//    Hank Childs, Tue Feb 19 19:45:43 PST 2008
//    Rename "dynamic" to "streaming", since we really care about whether we
//    are streaming, not about whether we are doing dynamic load balancing.
//    And the two are no longer synonymous.
//
// ****************************************************************************

void
avtSourceFromDatabase::ReleaseData(void)
{
    avtOriginatingDatasetSource::ReleaseData();
    if (GetOutput()->GetInfo().GetValidity().AreWeStreaming())
    {
        database->FreeUpResources();
    }
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::CanDoStreaming
//
//  Purpose:
//      Determine whether or not streaming is possible.
//
//  Programmer: Hank Childs
//  Creation:   June 12, 2008
//
// ****************************************************************************

bool
avtSourceFromDatabase::CanDoStreaming(avtContract_p contract)
{
    return database->CanDoStreaming(contract->GetDataRequest());
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::StoreArbitraryVTKObject
//
//  Purpose:
//      Stores an arbitrary VTK object in the cache.
//
//  Programmer: Hank Childs
//  Creation:   November 26, 2010
//
//  Modifications:
//
//    Hank Childs, Mon Jan 10 20:42:58 PST 2011
//    Add support for expressions.
//
//    Alister Maguire, Mon Jan  2 13:02:11 PST 2017
//    Added a mutex lock around CacheVTKObject for thread
//    safety.
//
// ****************************************************************************

void
avtSourceFromDatabase::StoreArbitraryVTKObject(const char *name, int domain,
                                              int ts, const char *type,
                                              vtkObject *obj)
{
    std::string name2 = ManageExpressions(name);
    avtVariableCache *cache = database->GetCache();
    VisitMutexLock("avtSourceFromDatabase::StoreArbitraryVTKObject"); 
    cache->CacheVTKObject(name2.c_str(), type, ts, domain, "", obj);
    VisitMutexUnlock("avtSourceFromDatabase::StoreArbitraryVTKObject");
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::FetchArbitraryVTKObject
//
//  Purpose:
//      Fetches an arbitrary VTK object from the cache.
//
//  Programmer: Hank Childs
//  Creation:   November 26, 2010
//
//  Modifications:
//
//    Hank Childs, Mon Jan 10 20:42:58 PST 2011
//    Add support for expressions.
//
// ****************************************************************************

vtkObject *
avtSourceFromDatabase::FetchArbitraryVTKObject(const char *name, int domain,
                                              int ts, const char *type)
{
    std::string name2 = ManageExpressions(name);
    avtVariableCache *cache = database->GetCache();
    return cache->GetVTKObject(name2.c_str(), type, ts, domain, "");
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::StoreArbitraryRefPtr
//
//  Purpose:
//      Stores an arbitrary ref_ptr object in the cache.
//
//  Programmer: Hank Childs
//  Creation:   November 26, 2010
//
//  Modifications:
//
//    Hank Childs, Mon Jan 10 20:42:58 PST 2011
//    Add support for expressions.
//
// ****************************************************************************

void
avtSourceFromDatabase::StoreArbitraryRefPtr(const char *name, int domain,
                                              int ts, const char *type,
                                              void_ref_ptr obj)
{
    std::string name2 = ManageExpressions(name);
    avtVariableCache *cache = database->GetCache();
    cache->CacheVoidRef(name2.c_str(), type, ts, domain, obj);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::FetchArbitraryRefPtr
//
//  Purpose:
//      Fetches an arbitrary ref_ptr object from the cache.
//
//  Programmer: Hank Childs
//  Creation:   November 26, 2010
//
//  Modifications:
//
//    Hank Childs, Mon Jan 10 20:42:58 PST 2011
//    Add support for expressions.
//
// ****************************************************************************

void_ref_ptr
avtSourceFromDatabase::FetchArbitraryRefPtr(const char *name, int domain,
                                              int ts, const char *type)
{
    std::string name2 = ManageExpressions(name);
    avtVariableCache *cache = database->GetCache();
    return cache->GetVoidRef(name2.c_str(), type, ts, domain);
}


// ****************************************************************************
//  Method: avtSourceFromDatabase::ManageExpressions
//
//  Purpose:
//      Expressions require special care to cache in the database, since the
//      expression list may change.  This method checks to see (1) if the
//      variable passed in as an argument is an expression (and, if so,
//      mangles it) and (2) if the expression list has changed (and, if so,
//      clears expressions in the variable cache).
//
//  Programmer: Hank Childs
//  Creation:   January 10, 2011
//
// ****************************************************************************

std::string
avtSourceFromDatabase::ManageExpressions(const char *name)
{
    ExpressionList *curList = ParsingExprList::Instance()->GetList();
    if (curList == NULL)
        return std::string(name);

    // check to see if this variable is an expression.
    if (curList->operator[](name) == NULL)
        return std::string(name);
 
    // TODO. If we thread safe the cache we can add back. DC
#ifndef VISIT_THREADS
    if (lastExprList == NULL)
        lastExprList = new ExpressionList(*curList);

    if (*lastExprList != *curList)
    {
        // The expression list changed ... clear all expressions out of the 
        // database cache.
        avtVariableCache *cache = database->GetCache();
        cache->ClearVariablesWithString("__AVT_EXPR__");
        *lastExprList = *curList;
    }
#endif // VISIT_THREADS

    // mangle in a prefix to say this is an expression.
    char str[1024];
    sprintf(str, "__AVT_EXPR__%s", name);
    return std::string(str);
}


