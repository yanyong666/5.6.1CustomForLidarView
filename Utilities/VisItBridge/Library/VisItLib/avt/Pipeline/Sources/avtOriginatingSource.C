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
//                           avtOriginatingSource.C                          //
// ************************************************************************* //

#include <avtOriginatingSource.h>

#include <avtMetaData.h>

#include <DebugStream.h>
#include <TimingsManager.h>


//
// Define static members.
//

LoadBalanceFunction  avtOriginatingSource::loadBalanceFunction     = NULL;
void                *avtOriginatingSource::loadBalanceFunctionArgs = NULL;
StreamingCheckFunction  avtOriginatingSource::streamingCheckFunction  = NULL;
void                *avtOriginatingSource::streamingCheckFunctionArgs = NULL;
InitializeProgressCallback
                     avtOriginatingSource::initializeProgressCallback = NULL;
void                *avtOriginatingSource::initializeProgressCallbackArgs=NULL;


// ****************************************************************************
//  Method: avtOriginatingSource constructor
//
//  Programmer: Hank Childs
//  Creation:   June 5, 2001
//
//  Modifications:
//
//    Hank Childs, Wed Feb  7 13:29:14 PST 2007
//    Initialize numberOfExecutions.
//
// ****************************************************************************

avtOriginatingSource::avtOriginatingSource()
{
    metadata = new avtMetaData(this);
    numberOfExecutions = 1;
}


// ****************************************************************************
//  Method: avtOriginatingSource destructor
//
//  Programmer: Hank Childs
//  Creation:   June 5, 2001
//
// ****************************************************************************

avtOriginatingSource::~avtOriginatingSource()
{
    if (metadata != NULL)
    {
        delete metadata;
        metadata = NULL;
    }
}


// ****************************************************************************
//  Method: avtOriginatingSource::GetMeshAuxiliaryData
//
//  Purpose:
//      Retrieves mesh auxiliary data from whatever the source to the pipeline
//      is (most likely a database).
//
//  Arguments:
//      auxType     The type of auxiliary data.
//      args        The arguments in some sort of struct.
//      spec        The pipeline specification.
//
//  Programmer: Hank Childs
//  Creation:   May 24, 2001
//
// ****************************************************************************

void
avtOriginatingSource::GetMeshAuxiliaryData(const char *type, void *args,
                          avtContract_p spec, VoidRefList &output)
{
    avtDataRequest_p data = BalanceLoad(spec);
    FetchMeshAuxiliaryData(type, args, data, output);
}


// ****************************************************************************
//  Method: avtOriginatingSource::GetVariableAuxiliaryData
//
//  Purpose:
//      Retrieves variable auxiliary data from whatever the source to the
//      pipeline is (most likely a database).
//
//  Arguments:
//      auxType     The type of auxiliary data.
//      args        The arguments in some sort of struct.
//      spec        The pipeline specification.
//
//  Programmer: Hank Childs
//  Creation:   May 28, 2001
//
// ****************************************************************************

void 
avtOriginatingSource::GetVariableAuxiliaryData(const char *type, void *args,
                          avtContract_p spec, VoidRefList &output)
{
    avtDataRequest_p data = BalanceLoad(spec);
    FetchVariableAuxiliaryData(type, args, data, output);
}


// ****************************************************************************
//  Method: avtOriginatingSource::GetMaterialAuxiliaryData
//
//  Purpose:
//      Retrieves material auxiliary data from whatever the source to the
//      pipeline is (most likely a database).
//
//  Arguments:
//      auxType     The type of auxiliary data.
//      args        The arguments in some sort of struct.
//      spec        The pipeline specification.
//
//  Programmer: Hank Childs
//  Creation:   May 28, 2001
//
// ****************************************************************************

void 
avtOriginatingSource::GetMaterialAuxiliaryData(const char *type, void *args,
                          avtContract_p spec, VoidRefList &output)
{
    avtDataRequest_p data = BalanceLoad(spec);
    FetchMaterialAuxiliaryData(type, args, data, output);
}


// ****************************************************************************
//  Method: avtOriginatingSource::GetSpeciesAuxiliaryData
//
//  Purpose:
//      Retrieves species auxiliary data from whatever the source to the
//      pipeline is (most likely a database).
//
//  Arguments:
//      auxType     The type of auxiliary data.
//      args        The arguments in some sort of struct.
//      spec        The pipeline specification.
//
//  Programmer: Jeremy Meredith
//  Creation:   June  8, 2004
//
// ****************************************************************************

void 
avtOriginatingSource::GetSpeciesAuxiliaryData(const char *type, void *args,
                          avtContract_p spec, VoidRefList &output)
{
    avtDataRequest_p data = BalanceLoad(spec);
    FetchSpeciesAuxiliaryData(type, args, data, output);
}


// ****************************************************************************
//  Method: avtOriginatingSource::Update
//
//  Purpose:
//      Performs an Update.  This is the termination of a pipeline.  The
//      load balancer is consulted at this point.
//
//  Arguments:
//      spec     The pipeline specification.
//
//  Returns:     A bool indicating if the input was changed.  This only returns
//               false now, but this may need more intelligence in the future.
//
//  Programmer:  Hank Childs
//  Creation:    May 28, 2001
//
//  Modifications:
//
//    Hank Childs, Mon Aug 13 15:23:08 PDT 2001
//    Made call to update progress.
//
//    Hank Childs, Thu Sep 25 08:12:27 PDT 2003
//    Reset the 'ErrorOccurred' field.
//
//    Hank Childs, Tue Sep 30 16:33:37 PDT 2003
//    Reset all of the validity fields, not just error occurred.
//
//    Hank Childs, Sun Dec  4 17:45:14 PST 2005
//    No longer issue progress.  All this does is clobber useful progress
//    reports.
// 
//    Hank Childs, Fri Nov 30 16:47:33 PST 2007
//    Add timing information.
//
//    Hank Childs, Thu Aug 26 16:57:24 PDT 2010
//    Cache the last contract.
//
// ****************************************************************************

bool
avtOriginatingSource::Update(avtContract_p contract)
{
    lastContract = contract;

    if (!ArtificialPipeline())
        GetOutput()->GetInfo().GetValidity().Reset();
    int t0 = visitTimer->StartTimer();
    avtDataRequest_p data = BalanceLoad(contract);
    visitTimer->StopTimer(t0, "Calling BalanceLoad in avtOrigSrc::Update");
    int t1 = visitTimer->StartTimer();
    bool rv = FetchData(data);
    visitTimer->StopTimer(t1, "Calling avtOrigSrc::FetchData");

    return rv;
}


// ****************************************************************************
//  Method: avtOriginatingSource::SetLoadBalancer
//
//  Purpose:
//      Sets the load balancer to be consulted when this terminating source
//      is about to get the actual data.
//
//  Arguments:
//      foo     The function that will call a load balancer.
//      args    The arguments for the function.
//
//  Programmer: Hank Childs
//  Creation:   May 28, 2001
//
// ****************************************************************************

void
avtOriginatingSource::SetLoadBalancer(LoadBalanceFunction foo, void *args)
{
    loadBalanceFunction     = foo;
    loadBalanceFunctionArgs = args;
}


// ****************************************************************************
//  Method: avtOriginatingSource::SetStreamingChecker
//
//  Purpose:
//      Sets the load balancer to be consulted to determine if the processing
//      type will be streaming.
//
//  Arguments:
//      foo     The function that will call a checker for streamer.
//      args    The arguments for the function.
//
//  Programmer: Jeremy Meredith
//  Creation:   September 19, 2001
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
avtOriginatingSource::SetStreamingChecker(StreamingCheckFunction foo,
                                          void *args)
{
    streamingCheckFunction     = foo;
    streamingCheckFunctionArgs = args;
}


// ****************************************************************************
//  Method: avtOriginatingSource::RegisterInitializeProgressCallback
//
//  Purpose:
//      Registers the InitializeProgressCallback.  This will be called before
//      any pipeline execution occurs to declare how many filters there are in
//      the whole pipeline.
//
//  Arguments:
//      pc      The initialize progress callback.
//      args    The arguments to the initialize progress callback.
//
//  Programmer: Hank Childs
//  Creation:   June 20, 2001
//
// ****************************************************************************

void
avtOriginatingSource::RegisterInitializeProgressCallback(
                                     InitializeProgressCallback pc, void *args)
{
    initializeProgressCallback     = pc;
    initializeProgressCallbackArgs = args;
}


// ****************************************************************************
//  Method: avtOriginatingSource::FetchMeshAuxiliaryData
//
//  Purpose:
//      Defines an implementation for fetching auxiliary data about a mesh.
//      This implementation does nothing and is only defined so that derived
//      types don't need to define a method that potentially doesn't make
//      sense for that type.
//
//  Arguments:
//      dataType  The type of data about the mesh (extents, etc).
//      args      The arguments describing the data.
//      spec      The data we are interested in.
//
//  Returns:      The requested data as a void *.
//
//  Programmer:   Hank Childs
//  Creation:     May 28, 2001
//
// ****************************************************************************  
void
avtOriginatingSource::FetchMeshAuxiliaryData(const char *, void *,
                                   avtDataRequest_p, VoidRefList &output)
{
    output.nList = 0;
}


// ****************************************************************************
//  Method: avtOriginatingSource::FetchVariableAuxiliaryData
//
//  Purpose:
//      Defines an implementation for fetching auxiliary data about a variable.
//      This implementation does nothing and is only defined so that derived
//      types don't need to define a method that potentially doesn't make
//      sense for that type.
//
//  Arguments:
//      dataType  The type of data about the variable (extents, etc).
//      args      The arguments describing the data.
//      spec      The data we are interested in.
//
//  Returns:      The requested data as a void *.
//
//  Programmer:   Hank Childs
//  Creation:     May 28, 2001
//
// ****************************************************************************  
void 
avtOriginatingSource::FetchVariableAuxiliaryData(const char *,void *,
                                   avtDataRequest_p, VoidRefList &output)
{
    output.nList = 0;
}


// ****************************************************************************
//  Method: avtOriginatingSource::FetchMaterialAuxiliaryData
//
//  Purpose:
//      Defines an implementation for fetching auxiliary data about a material.
//      This implementation does nothing and is only defined so that derived
//      types don't need to define a method that potentially doesn't make
//      sense for that type.
//
//  Arguments:
//      dataType  The type of data about the material.
//      args      The arguments describing the data.
//      spec      The data we are interested in.
//
//  Returns:      The requested data as a void *.
//
//  Programmer:   Hank Childs
//  Creation:     May 28, 2001
//
// ****************************************************************************  
void
avtOriginatingSource::FetchMaterialAuxiliaryData(const char *,void *,
                                   avtDataRequest_p, VoidRefList &output)
{
    output.nList = 0;
}


// ****************************************************************************
//  Method: avtOriginatingSource::FetchSpeciesAuxiliaryData
//
//  Purpose:
//      Defines an implementation for fetching auxiliary data about a species.
//      This implementation does nothing and is only defined so that derived
//      types don't need to define a method that potentially doesn't make
//      sense for that type.
//
//  Arguments:
//      dataType  The type of data about the species.
//      args      The arguments describing the data.
//      spec      The data we are interested in.
//
//  Returns:      The requested data as a void *.
//
//  Programmer:   Jeremy Meredith
//  Creation:     June  8, 2004
//
// ****************************************************************************  
void
avtOriginatingSource::FetchSpeciesAuxiliaryData(const char *,void *,
                                   avtDataRequest_p, VoidRefList &output)
{
    output.nList = 0;
}


// ****************************************************************************
//  Method: avtOriginatingSource::BalanceLoad
//
//  Purpose:
//      This calls a function that will take in the input data specification 
//      and return the chunk of the data that pipeline should operate on for
//      this execution.
//      
//  Arguments:
//      spec     The pipeline specification
// 
//  Returns:     The data specification after load balancing.
//
//  Programmer:  Hank Childs
//  Creation:    May 28, 2001
//
//  Modifications:
//
//    Hank Childs, Wed Jun 20 17:26:46 PDT 2001
//    Added call to InitPipeline.
//
//    Hank Childs, Tue Jul 24 10:14:55 PDT 2001
//    Added condition to call InitPipeline to account for auxiliary data.
//
//    Hank Childs, Wed Oct  9 10:14:37 PDT 2002
//    Check to see if we are using all the domains as a courtesy to the
//    database.  (It may not have enough information if we are running in
//    parallel).
//
//    Hank Childs, Sun Mar  9 07:45:38 PST 2008
//    If we are doing on demand streaming, then indicate that we are streaming
//    in the output.
//
//    Hank Childs, Sun Sep 19 10:47:12 PDT 2010
//    Determine if data replication occurred.
//
// ****************************************************************************

avtDataRequest_p
avtOriginatingSource::BalanceLoad(avtContract_p contract)
{
    bool usesAllDomains =contract->GetDataRequest()->GetSIL().UsesAllDomains();

    //
    // If it shouldn't use load balancing, then it has to do with auxiliary
    // data coming through our meta-data mechanism.  Calling InitPipeline
    // would change the data attributes and it also causes an unnecessary
    // callback to our progress mechanism.
    //
    if (contract->ShouldUseLoadBalancing())
    {
        int t0 = visitTimer->StartTimer();
        InitPipeline(contract);
        visitTimer->StopTimer(t0, "InitPipeline");
    }
    else if (contract->DoingOnDemandStreaming())
    {
        GetOutput()->GetInfo().GetValidity().SetWhetherStreaming(true);
    }

    //
    // Allow the load balancer to split the load across processors.
    //
    bool dataReplicationOccurred = false;
    avtDataRequest_p rv = NULL;
    if (!UseLoadBalancer())
    {
        debug1 << "This source should not load balance the data." << endl;
        rv = contract->GetDataRequest();
    }
    else if (! contract->ShouldUseLoadBalancing())
    {
        debug1 << "This pipeline has indicated that no load balancing should "
               << "be used." << endl;
        rv = contract->GetDataRequest();
    }
    else if (loadBalanceFunction != NULL)
    {
        debug1 << "Using load balancer to reduce data." << endl;
        int t0 = visitTimer->StartTimer();
        rv = loadBalanceFunction(loadBalanceFunctionArgs, contract);
        visitTimer->StopTimer(t0, "Call loadBalanceFunction");
        dataReplicationOccurred =
                              contract->ReplicateSingleDomainOnAllProcessors();
    }
    else
    {
        debug1 << "No load balancer exists to reduce data." << endl;
        rv = contract->GetDataRequest();
    }

    //
    // Return the portion for this processor.
    //
    rv->SetUsesAllDomains(usesAllDomains);

    //
    // Tell the output if we are doing data replication.
    //
    if (dataReplicationOccurred)
        GetOutput()->GetInfo().GetAttributes().SetDataIsReplicatedOnAllProcessors(true);

    return rv;
}


// ****************************************************************************
//  Method: avtOriginatingSource::InitPipeline
//
//  Purpose:
//      Initializes the rest of the pipeline.  This determines if the data
//      request is all of the data and also makes a callback declaring
//      how many total filters there are.
//
//  Arguments:
//      spec    The pipeline specification for all of the nodes.
//
//  Programmer: Hank Childs
//  Creation:   June 20, 2001
//
//  Modifications:
//
//    Hank Childs, Tue Jul 31 11:32:57 PDT 2001
//    Do not re-initialize the pipeline if we have an artificial pipeline.
//
//    Hank Childs, Fri Sep 14 09:58:14 PDT 2001
//    Indicate if we are using all of the domains.
//
//    Jeremy Meredith, Thu Sep 20 01:03:48 PDT 2001
//    Added check for dynamic load balancing, which has only one stage.
//
//    Kathleen Bonnell, Fri Oct 12 12:07:01 PDT 2001 
//    Set IsThisDynamic.  
//
//    Hank Childs, Thu Oct 25 16:44:24 PDT 2001
//    Allow for the source to prevent dynamic load balancing.
//
//    Hank Childs, Fri Oct 26 15:35:37 PDT 2001
//    Allow for terminating sources to have stages.
//
//    Hank Childs, Wed Feb  7 13:29:14 PST 2007
//    If there will be multiple executions (like for the time loop filter), 
//    then we want to declare the correct number of stages.
//
//    Hank Childs, Tue Feb 19 19:45:43 PST 2008
//    Rename "dynamic" to "streaming", since we really care about whether we
//    are streaming, not about whether we are doing dynamic load balancing.
//    And the two are no longer synonymous.
//
// ****************************************************************************

void
avtOriginatingSource::InitPipeline(avtContract_p spec)
{
    if (!ArtificialPipeline())
    {
        if (!CanDoStreaming())
        {
            spec->NoStreaming();
        }

        //
        // Determine if the data specification is *all* of the data.  This is
        // important to things like facelists.
        //
        avtDataRequest_p data = spec->GetDataRequest();
        bool uad = data->GetSIL().UsesAllData();
        GetOutput()->GetInfo().GetValidity().SetUsingAllData(uad);
    
        bool uadom = data->GetSIL().UsesAllDomains();
        GetOutput()->GetInfo().GetValidity().SetUsingAllDomains(uadom);

        if (initializeProgressCallback != NULL &&
            streamingCheckFunction       != NULL)
        {
            //
            // Each filter is a stage, plus a stage to get the data.
            // With streaming, there are only calculate/send stages.
            //
            int nstages;
            if (streamingCheckFunction(streamingCheckFunctionArgs,spec))
            {
                nstages = 1;
                GetOutput()->GetInfo().GetValidity().SetWhetherStreaming(true);
            }
            else
            {
                int sourceStages = NumStagesForFetch(data);
                nstages = spec->GetNFilters() + sourceStages;
                GetOutput()->GetInfo().GetValidity().SetWhetherStreaming(false);
                if (numberOfExecutions > 1)
                    nstages *= numberOfExecutions;
            }
            bool shouldIssue = false;
            if (numberOfExecutions <= 1)
                shouldIssue = true;
            else if (numberOfExecutions > 1)
            {
                if (!haveIssuedProgress)
                {
                    shouldIssue = true;
                    haveIssuedProgress = true;
                }
            }
            if (shouldIssue)
                initializeProgressCallback(initializeProgressCallbackArgs,
                                           nstages);
        }
    }
}


// ****************************************************************************
//  Method: avtOriginatingSource::GetFullDataRequest
//
//  Purpose:
//      Gets a full data description.  Since this doesn't make as much sense
//      for some sources as others (say a database), it is defined here to
//      return a dummied up description.
//
//  Returns:    A totally generic data description.
//
//  Programmer: Hank Childs
//  Creation:   June 6, 2001
//
// ****************************************************************************

avtDataRequest_p
avtOriginatingSource::GetFullDataRequest(void)
{
    return new avtDataRequest("dummy_specification", -1, -1);
}


// ****************************************************************************
//  Method: avtOriginatingSource::GetGeneralContract
//
//  Purpose:
//       Gets a pipeline that the load balancer knows not to mess with.
//
//  Programmer: Hank Childs
//  Creation:   June 6, 2001
//
//  Modifications:
//
//    Hank Childs, Wed Dec 22 12:56:36 PST 2010
//    Make a "general" contract use the same values for streaming.
//
// ****************************************************************************

avtContract_p
avtOriginatingSource::GetGeneralContract(void)
{
    avtDataRequest_p data = GetFullDataRequest();
    avtContract_p rv = new avtContract(data, 0);
    if (*lastContract != NULL)
    {
        rv->SetOnDemandStreaming(lastContract->DoingOnDemandStreaming());
        rv->UseLoadBalancing(lastContract->ShouldUseLoadBalancing());
    }

    return rv;
}


// ****************************************************************************
//  Method: avtOriginatingSource::GetSelectionsForLastExecution
//
//  Purpose:
//     Gets the selections used in the previous execution.
//
//  Programmer: Hank Childs
//  Creation:   December 11, 2012
//
// ****************************************************************************

std::vector<avtDataSelection_p> 
avtOriginatingSource::GetSelectionsForLastExecution(void)
{
    if (*lastContract == NULL)
    {
        std::vector<avtDataSelection_p> emptySelectionList;
        return emptySelectionList;
    }

    return lastContract->GetDataRequest()->GetAllDataSelections();
}


// ****************************************************************************
//  Method: avtOriginatingSource::CanDoStreaming
//
//  Purpose:
//      Returns whether or not this source can do dynamic load balancing.
//
//  Programmer: Hank Childs
//  Creation:   October 25, 2001
//
//  Modifications:
//
//    Hank Childs, Tue Feb 19 19:45:43 PST 2008
//    Rename "dynamic" to "streaming", since we really care about whether we
//    are streaming, not about whether we are doing dynamic load balancing.
//    And the two are no longer synonymous.
//
// ****************************************************************************

bool
avtOriginatingSource::CanDoStreaming(void)
{
    //
    // We'd like to do it unless we have a reason not to.
    //
    return true;
}


// ****************************************************************************
//  Method: avtOriginatingSource::NumStagesForFetch
//
//  Purpose:
//      This returns how many stages there are for the fetch.  Some databases
//      have more than one stage (read from file format, perform material
//      selection, etc).
//
//  Returns:    A good default number of stages (1).
//
//  Programmer: Hank Childs
//  Creation:   October 26, 2001
//
// ****************************************************************************

int
avtOriginatingSource::NumStagesForFetch(avtDataRequest_p)
{
    return 1;
}


// ****************************************************************************
//  Method: avtOriginatingSource::StoreArbitraryVTKObject
//
//  Purpose:
//      Stores an arbitrary VTK object in the cache.
//
//  Programmer: Hank Childs
//  Creation:   November 26, 2010
//
// ****************************************************************************

void
avtOriginatingSource::StoreArbitraryVTKObject(const char *name, int domain,
                                              int ts, const char *type,
                                              vtkObject *obj)
{
    debug1 << "Asked to store object, but don't know how to do that." << endl;
    debug1 << "This means caching will not work; this case is not expected"
           << endl;
}


// ****************************************************************************
//  Method: avtOriginatingSource::FetchArbitraryVTKObject
//
//  Purpose:
//      Fetches an arbitrary VTK object from the cache.
//
//  Programmer: Hank Childs
//  Creation:   November 26, 2010
//
// ****************************************************************************

vtkObject *
avtOriginatingSource::FetchArbitraryVTKObject(const char *name, int domain,
                                              int ts, const char *type)
{
    debug1 << "Asked to fetch object, but don't know how to do that." << endl;
    debug1 << "This means caching will not work; this case is not expected"
           << endl;
    return NULL;
}


// ****************************************************************************
//  Method: avtOriginatingSource::StoreArbitraryRefPtr
//
//  Purpose:
//      Stores an arbitrary ref_ptr object in the cache.
//
//  Programmer: Hank Childs
//  Creation:   November 26, 2010
//
// ****************************************************************************

void
avtOriginatingSource::StoreArbitraryRefPtr(const char *name, int domain,
                                              int ts, const char *type,
                                              void_ref_ptr obj)
{
    debug1 << "Asked to store object, but don't know how to do that." << endl;
    debug1 << "This means caching will not work; this case is not expected"
           << endl;
}


// ****************************************************************************
//  Method: avtOriginatingSource::FetchArbitraryRefPtr
//
//  Purpose:
//      Fetches an arbitrary ref_ptr object from the cache.
//
//  Programmer: Hank Childs
//  Creation:   November 26, 2010
//
// ****************************************************************************

void_ref_ptr
avtOriginatingSource::FetchArbitraryRefPtr(const char *name, int domain,
                                              int ts, const char *type)
{
    debug1 << "Asked to fetch object, but don't know how to do that." << endl;
    debug1 << "This means caching will not work; this case is not expected"
           << endl;
    void_ref_ptr vrp;
    return vrp;
}


// ****************************************************************************
//  Method: avtOriginatingSource::ResetAllExtents
//
//  Purpose:
//      This is the termination of a pipeline, nothing to do here.
//
//  Programmer:  Kathleen Biagas
//  Creation:    June 5, 2017
//
//  Modifications:
//
// ****************************************************************************

void
avtOriginatingSource::ResetAllExtents()
{
    ; // nothing to do here
}

