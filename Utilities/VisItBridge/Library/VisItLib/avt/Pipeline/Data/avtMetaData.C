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
//                                avtMetaData.C                              //
// ************************************************************************* //

#include <avtMetaData.h>

#include <stdlib.h>
#include <float.h>

#include <vtkDataSet.h>

#include <avtCommonDataFunctions.h>
#include <avtDomainBoundaries.h>
#include <avtDomainNesting.h>
#include <avtFacelist.h>
#include <avtHistogramSpecification.h>
#include <avtIdentifierSelection.h>
#include <avtIntervalTree.h>
#include <avtOriginatingSource.h>
#include <avtMixedVariable.h>
#include <avtTypes.h>

#include <ImproperUseException.h>

// ****************************************************************************
//  Method: avtMetaData constructor
//
//  Arguments:
//      s       The source this meta-data corresponds to.
//
//  Programmer: Hank Childs
//  Creation:   August 7, 2000
//
// ****************************************************************************

avtMetaData::avtMetaData(avtOriginatingSource *s)
{
    source = s;
}


// ****************************************************************************
//  Method: avtMetaData destructor
//
//  Purpose:
//      Defines the destructor.  Note: this should not be inlined in the header
//      because it causes problems for certain compilers.
//
//  Programmer: Hank Childs
//  Creation:   February 5, 2004
//
// ****************************************************************************

avtMetaData::~avtMetaData()
{
}


// ****************************************************************************
//  Method: avtMetaData::GetDataExtents
//
//  Purpose:
//      Gets the data extents for the dataset.  An interval tree is always
//      returned, even if it has to be calculated.
//
//  Returns:    An interval tree of the data extents.
//
//  Programmer: Hank Childs
//  Creation:   August 7, 2000
//
//  Modifications:
//
//    Hank Childs, Tue Jun  5 08:16:49 PDT 2001
//    Removed reference to domain lists.
//
//    Mark C. Miller, Mon Oct 18 13:02:37 PDT 2004
//    Added optional var arg
//
// ****************************************************************************

avtIntervalTree *
avtMetaData::GetDataExtents(const char *var)
{
    VoidRefList list;
    avtContract_p contract = GetContract();
    source->GetVariableAuxiliaryData(AUXILIARY_DATA_DATA_EXTENTS, (void*) var,
                                     contract, list);
    if (list.nList == 0)
    {
        return NULL;
    }
    if (list.nList != 1)
    {
        EXCEPTION0(ImproperUseException);
    }

    avtIntervalTree *tree = (avtIntervalTree *) *(list.list[0]);

    return tree;
}


// ****************************************************************************
//  Method: avtMetaData::GetSpatialExtents
//
//  Purpose:
//      Gets the spatial extents for the dataset.  An interval tree is always
//      returned, even if it has to be calculated.
//
//  Returns:    An interval tree of the spatial extents.
//     
//  Programmer: Hank Childs
//  Creation:   August 7, 2000
//
//  Modifications:
//
//    Hank Childs, Tue Jun  5 08:16:49 PDT 2001
//    Removed reference to domain lists.
//
//    Mark C. Miller, Mon Oct 18 13:02:37 PDT 2004
//    Added optional var arg
//
// ****************************************************************************

avtIntervalTree *
avtMetaData::GetSpatialExtents(const char *var)
{
    return GetSpatialExtents(-1, var);
}

avtIntervalTree *
avtMetaData::GetSpatialExtents(int timeSlice, const char *var)
{
    VoidRefList list;
    avtContract_p contract = GetContract(-1, timeSlice);
    source->GetMeshAuxiliaryData(AUXILIARY_DATA_SPATIAL_EXTENTS, (void*) var,
                                 contract, list);
    if (list.nList == 0)
    {
        return NULL;
    }
    if (list.nList != 1)
    {
        EXCEPTION0(ImproperUseException);
    }

    avtIntervalTree *tree = (avtIntervalTree *) *(list.list[0]);
    return tree;
}


// ****************************************************************************
//  Method: avtMetaData::GetHistogram
//
//  Purpose:
//      Gets the histogram corresponding to the input histogram specification.
//     
//  Programmer: Hank Childs
//  Creation:   February 25, 2008
//
// ****************************************************************************

bool
avtMetaData::GetHistogram(avtHistogramSpecification *hs)
{
    if (hs == NULL)
        EXCEPTION0(ImproperUseException);

    VoidRefList list;
    avtContract_p contract = GetContract();
    source->GetVariableAuxiliaryData(AUXILIARY_DATA_HISTOGRAM, (void *)hs,
                                     contract, list);
    VISIT_LONG_LONG *counts = hs->GetCounts();
    for (int i = 0 ; i < hs->GetTotalNumberOfBins() ; i++)
        if (counts[i] != 0)
            return true;

    return false;
}


// ****************************************************************************
//  Method: avtMetaData::GetIdentifiers
//
//  Purpose:
//      Gets the identifiers corresponding to a data selection.
//     
//  Programmer: Hank Childs
//  Creation:   February 25, 2008
//
// ****************************************************************************

avtIdentifierSelection *
avtMetaData::GetIdentifiers(std::vector<avtDataSelection *>ds)
{
    if (ds.size() == 0)
        EXCEPTION0(ImproperUseException);

    VoidRefList list;
    avtContract_p contract = GetContract();
    
    source->GetVariableAuxiliaryData(AUXILIARY_DATA_IDENTIFIERS, (void *)&ds,
                                     contract, list);

    if (list.nList == 0)
    {
        return NULL;
    }
    if (list.nList != 1)
    {
        EXCEPTION0(ImproperUseException);
    }

    avtIdentifierSelection *ids = (avtIdentifierSelection *) *(list.list[0]);
    return ids;
}


// ****************************************************************************
//  Method: avtMetaData::GetExternalFacelist
//
//  Purpose:
//      Gets the face list from the source.
//
//  Note:     Most meta-data objects are calculated if they are not available.
//            This is not true for facelists, so NULL is a possible output.
//
//  Returns:  The facelist for the dataset, if one exists.
//
//  Programmer: Hank Childs
//  Creation:   October 27, 2000
//
//  Modifications:
//
//    Brad Whitlock, Thu Apr 4 14:38:03 PST 2002
//    Changed CopyTo to a template function.
//
//    Hank Childs, Tue Sep 24 11:43:06 PDT 2002
//    Return a normal pointer to a facelist, not a reference counted one.
//
// ****************************************************************************

avtFacelist *
avtMetaData::GetExternalFacelist(int domain)
{
    VoidRefList list;
    avtContract_p contract = GetContract(domain);
    source->GetMeshAuxiliaryData(AUXILIARY_DATA_EXTERNAL_FACELIST, NULL,
                                 contract, list);
    if (list.nList == 0)
    {
        return NULL;
    }
    if (list.nList != 1)
    {
        EXCEPTION0(ImproperUseException);
    }

    return (avtFacelist *) *(list.list[0]);
}


// ****************************************************************************
//  Method: avtMetaData::GetMaterial
//
//  Purpose:
//      Gets an avtMaterial object.
//
//  Returns:  The material for the dataset, if one exists.
//
//  Programmer: Hank Childs
//  Creation:   August 12, 2003
//
//  Modifications:
//    Kathleen Bonnell, Mon Jun 28 08:05:38 PDT 2004
//    Added optional timestep argument.
//
//    Cyrus Harrison, Wed Jan 30 13:23:30 PST 2008
//    Added post ghost argument, providing access to ghost corrected Material
//    objects.
//    
// ****************************************************************************

avtMaterial *
avtMetaData::GetMaterial(int domain, int timestep, bool post_ghost)
{
    VoidRefList list;
    avtContract_p contract = GetContract(domain);

    //
    // If a timestep has been specified, set it in DataRequest.
    //
    if (timestep != -1)
        contract->GetDataRequest()->SetTimestep(timestep);

    // look for ghost corrected material object if needed
    if(post_ghost)
    {
        source->GetMaterialAuxiliaryData(AUXILIARY_DATA_POST_GHOST_MATERIAL, 
                                         NULL,
                                         contract,
                                         list);
    }
    else // look standard material object
    {
        source->GetMaterialAuxiliaryData(AUXILIARY_DATA_MATERIAL, 
                                         NULL,
                                         contract,
                                         list);    
    }
    
    if (list.nList == 0)
    {
        return NULL;
    }
    if (list.nList != 1)
    {
        EXCEPTION0(ImproperUseException);
    }

    return (avtMaterial *) *(list.list[0]);
}


// ****************************************************************************
//  Method: avtMetaData::GetSpecies
//
//  Purpose:
//      Gets an avtSpecies object.
//
//  Returns:  The species for the dataset, if one exists.
//
//  Programmer: Jeremy Meredith
//  Creation:   June  8, 2004
//
//  Modifications:
//    Kathleen Bonnell, Mon Jun 28 08:05:38 PDT 2004
//    Added optional timestep argument.
//
// ****************************************************************************

avtSpecies *
avtMetaData::GetSpecies(int domain, int timestep)
{
    VoidRefList list;
    avtContract_p contract = GetContract(domain);
    //
    // If a timestep has been specified, set it in DataRequest.
    //
    if (timestep != -1)
        contract->GetDataRequest()->SetTimestep(timestep);
    source->GetSpeciesAuxiliaryData(AUXILIARY_DATA_SPECIES, NULL, contract,list);
    if (list.nList == 0)
    {
        return NULL;
    }
    if (list.nList != 1)
    {
        EXCEPTION0(ImproperUseException);
    }
    return (avtSpecies *) *(list.list[0]);
}


// ****************************************************************************
//  Method: avtMetaData::GetContract
//
//  Purpose:
//      Gets a contract corresponding to all data.
//
//  Programmer: Hank Childs
//  Creation:   June 6, 2001
//
// ****************************************************************************

avtContract_p
avtMetaData::GetContract(void)
{
    //
    // -1 means get an object for the entire dataset.
    //
    return GetContract(-1);
}


// ****************************************************************************
//  Method: avtMetaData::GetContract
//
//  Purpose:
//      Gets a contract corresponding to all data.
//
//  Arguments:
//      domain  The identifier for the chunk we are interested in.
//
//  Programmer: Hank Childs
//  Creation:   June 6, 2001
//
// ****************************************************************************

avtContract_p
avtMetaData::GetContract(int domain, int timeSlice)
{
    //
    // Get the data from the source that represents all data.  Then override it
    // to have only the domain we are interested in.
    //
    avtDataRequest_p ds = source->GetFullDataRequest();
    avtDataRequest_p alldata = new avtDataRequest(ds, domain);

    //
    // Create a pipeline with index -1.  This is the index reserved for an
    // ad-hoc pipeline and it is only to meet the interface.  The load balancer
    // should be aware of this.
    //
    avtContract_p ps = new avtContract(alldata, -1);
    if( timeSlice != -1 )
        ps->GetDataRequest()->SetTimestep(timeSlice);
    ps->UseLoadBalancing(false);

    return ps;
}


// ****************************************************************************
//  Method: avtMetaData::GetMixedVar
//
//  Purpose:
//      Gets an avtMixedVariable object.
//
//  Returns:  The mixed variable for the dataset, if one exists.
//
//  Programmer: Kathleen Bonnell 
//  Creation:   July 1, 2004 
//
//  Modifications:
//
//    Cyrus Harrison, Wed Jan 30 13:23:30 PST 2008
//    Added variable name argument to GetMixedVar, allowing this method
//    to be used when the desired mixed var differs from the active
//    variable in the contract. 
//
//    Cyrus Harrison, Wed Jan 30 13:23:30 PST 2008
//    Added post ghost argument, providing access to ghost corrected Mixed 
//    Variable objects.
//    
// ****************************************************************************

avtMixedVariable *
avtMetaData::GetMixedVar(const char *varname, 
                         int domain, 
                         int timestep,
                         bool post_ghost)
{
    VoidRefList list;
    avtContract_p contract = GetContract(domain);
    //
    // If a timestep has been specified, set it in DataRequest.
    //
    if (timestep != -1)
        contract->GetDataRequest()->SetTimestep(timestep);

    // pass the varname, so the source can obtain the proper mixed var.
    
    // look for ghost corrected mixvar object if needed
    if(post_ghost)
    {
        source->GetVariableAuxiliaryData(
                                      AUXILIARY_DATA_POST_GHOST_MIXED_VARIABLE, 
                                         (void*)varname, 
                                         contract,
                                         list);
    }
    else
    {
        // look for standard mixvar object
        source->GetVariableAuxiliaryData(AUXILIARY_DATA_MIXED_VARIABLE, 
                                         (void*)varname, 
                                         contract,
                                         list);
    }
    
    
    if (list.nList == 0)
    {
        return NULL;
    }
    if (list.nList != 1)
    {
        EXCEPTION0(ImproperUseException);
    }

    return (avtMixedVariable *) *(list.list[0]);
}

// ****************************************************************************
//  Method: avtMetaData::GetDomainNesting
//
//  Purpose:
//      Gets the domain nesting information.  This is used for AMR patches.
//
//  Returns:    The domain nesting object
//
//  Programmer: Hank Childs
//  Creation:   August 5, 2010
//
// ****************************************************************************

avtDomainNesting *
avtMetaData::GetDomainNesting(void)
{
    VoidRefList list;
    avtContract_p spec = GetContract();
    source->GetMeshAuxiliaryData(AUXILIARY_DATA_DOMAIN_NESTING_INFORMATION,
                                 (void*) "any_mesh", spec, list);

    if (list.nList == 0)
    {
        return NULL;
    }
    if (list.nList != 1)
    {
        EXCEPTION0(ImproperUseException);
    }

    avtDomainNesting *dni = (avtDomainNesting *) (*(list.list[0]));

    return dni;
}


// ****************************************************************************
//  Method: avtMetaData::GetDomainBoundaries
//
//  Purpose:
//      Gets the domain boundary information.
//
//  Returns:    The domain boundary object
//
//  Programmer: Hank Childs
//  Creation:   August 5, 2010
//
// ****************************************************************************

avtDomainBoundaries *
avtMetaData::GetDomainBoundaries(void)
{
    VoidRefList list;
    avtContract_p spec = GetContract();

    source->GetMeshAuxiliaryData(AUXILIARY_DATA_DOMAIN_BOUNDARY_INFORMATION,
                                 (void*) "any_mesh", spec, list);

    if (list.nList == 0)
    {
        return NULL;
    }
    if (list.nList != 1)
    {
        EXCEPTION0(ImproperUseException);
    }

    avtDomainBoundaries *dbi = (avtDomainBoundaries *) *(list.list[0]);

    return dbi;
}
