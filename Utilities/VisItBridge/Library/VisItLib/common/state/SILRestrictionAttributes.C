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

#include <SILRestrictionAttributes.h>
#include <DataNode.h>

// ****************************************************************************
// Method: SILRestrictionAttributes::SILRestrictionAttributes
//
// Purpose: 
//   Init utility for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

void SILRestrictionAttributes::Init()
{
    topSet = -1;

    SILRestrictionAttributes::SelectAll();
}

// ****************************************************************************
// Method: SILRestrictionAttributes::SILRestrictionAttributes
//
// Purpose: 
//   Copy utility for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

void SILRestrictionAttributes::Copy(const SILRestrictionAttributes &obj)
{
    useSet = obj.useSet;
    topSet = obj.topSet;
    silAtts = obj.silAtts;

    SILRestrictionAttributes::SelectAll();
}

// Type map format string
const char *SILRestrictionAttributes::TypeMapFormatString = SILRESTRICTIONATTRIBUTES_TMFS;
const AttributeGroup::private_tmfs_t SILRestrictionAttributes::TmfsStruct = {SILRESTRICTIONATTRIBUTES_TMFS};


// ****************************************************************************
// Method: SILRestrictionAttributes::SILRestrictionAttributes
//
// Purpose: 
//   Default constructor for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

SILRestrictionAttributes::SILRestrictionAttributes() : 
    AttributeSubject(SILRestrictionAttributes::TypeMapFormatString)
{
    SILRestrictionAttributes::Init();
}

// ****************************************************************************
// Method: SILRestrictionAttributes::SILRestrictionAttributes
//
// Purpose: 
//   Constructor for the derived classes of SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

SILRestrictionAttributes::SILRestrictionAttributes(private_tmfs_t tmfs) : 
    AttributeSubject(tmfs.tmfs)
{
    SILRestrictionAttributes::Init();
}

// ****************************************************************************
// Method: SILRestrictionAttributes::SILRestrictionAttributes
//
// Purpose: 
//   Copy constructor for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

SILRestrictionAttributes::SILRestrictionAttributes(const SILRestrictionAttributes &obj) : 
    AttributeSubject(SILRestrictionAttributes::TypeMapFormatString)
{
    SILRestrictionAttributes::Copy(obj);
}

// ****************************************************************************
// Method: SILRestrictionAttributes::SILRestrictionAttributes
//
// Purpose: 
//   Copy constructor for derived classes of the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

SILRestrictionAttributes::SILRestrictionAttributes(const SILRestrictionAttributes &obj, private_tmfs_t tmfs) : 
    AttributeSubject(tmfs.tmfs)
{
    SILRestrictionAttributes::Copy(obj);
}

// ****************************************************************************
// Method: SILRestrictionAttributes::~SILRestrictionAttributes
//
// Purpose: 
//   Destructor for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

SILRestrictionAttributes::~SILRestrictionAttributes()
{
    // nothing here
}

// ****************************************************************************
// Method: SILRestrictionAttributes::operator = 
//
// Purpose: 
//   Assignment operator for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

SILRestrictionAttributes& 
SILRestrictionAttributes::operator = (const SILRestrictionAttributes &obj)
{
    if (this == &obj) return *this;

    SILRestrictionAttributes::Copy(obj);

    return *this;
}

// ****************************************************************************
// Method: SILRestrictionAttributes::operator == 
//
// Purpose: 
//   Comparison operator == for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

bool
SILRestrictionAttributes::operator == (const SILRestrictionAttributes &obj) const
{
    // Create the return value
    return ((useSet == obj.useSet) &&
            (topSet == obj.topSet) &&
            (silAtts == obj.silAtts));
}

// ****************************************************************************
// Method: SILRestrictionAttributes::operator != 
//
// Purpose: 
//   Comparison operator != for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

bool
SILRestrictionAttributes::operator != (const SILRestrictionAttributes &obj) const
{
    return !(this->operator == (obj));
}

// ****************************************************************************
// Method: SILRestrictionAttributes::TypeName
//
// Purpose: 
//   Type name method for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

const std::string
SILRestrictionAttributes::TypeName() const
{
    return "SILRestrictionAttributes";
}

// ****************************************************************************
// Method: SILRestrictionAttributes::CopyAttributes
//
// Purpose: 
//   CopyAttributes method for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

bool
SILRestrictionAttributes::CopyAttributes(const AttributeGroup *atts)
{
    if(TypeName() != atts->TypeName())
        return false;

    // Call assignment operator.
    const SILRestrictionAttributes *tmp = (const SILRestrictionAttributes *)atts;
    *this = *tmp;

    return true;
}

// ****************************************************************************
// Method: SILRestrictionAttributes::CreateCompatible
//
// Purpose: 
//   CreateCompatible method for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

AttributeSubject *
SILRestrictionAttributes::CreateCompatible(const std::string &tname) const
{
    AttributeSubject *retval = 0;
    if(TypeName() == tname)
        retval = new SILRestrictionAttributes(*this);
    // Other cases could go here too. 

    return retval;
}

// ****************************************************************************
// Method: SILRestrictionAttributes::NewInstance
//
// Purpose: 
//   NewInstance method for the SILRestrictionAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

AttributeSubject *
SILRestrictionAttributes::NewInstance(bool copy) const
{
    AttributeSubject *retval = 0;
    if(copy)
        retval = new SILRestrictionAttributes(*this);
    else
        retval = new SILRestrictionAttributes;

    return retval;
}

// ****************************************************************************
// Method: SILRestrictionAttributes::SelectAll
//
// Purpose: 
//   Selects all attributes.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

void
SILRestrictionAttributes::SelectAll()
{
    Select(ID_useSet,  (void *)&useSet);
    Select(ID_topSet,  (void *)&topSet);
    Select(ID_silAtts, (void *)&silAtts);
}

///////////////////////////////////////////////////////////////////////////////
// Persistence methods
///////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
// Method: SILRestrictionAttributes::CreateNode
//
// Purpose: 
//   This method creates a DataNode representation of the object so it can be saved to a config file.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

bool
SILRestrictionAttributes::CreateNode(DataNode *parentNode, bool completeSave, bool forceAdd)
{
    if(parentNode == 0)
        return false;

    SILRestrictionAttributes defaultObject;
    bool addToParent = false;
    // Create a node for SILRestrictionAttributes.
    DataNode *node = new DataNode("SILRestrictionAttributes");

    if(completeSave || !FieldsEqual(ID_useSet, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("useSet", useSet));
    }

    if(completeSave || !FieldsEqual(ID_topSet, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("topSet", topSet));
    }

    if(completeSave || !FieldsEqual(ID_silAtts, &defaultObject))
    {
        DataNode *silAttsNode = new DataNode("silAtts");
        if(silAtts.CreateNode(silAttsNode, completeSave, false))
        {
            addToParent = true;
            node->AddNode(silAttsNode);
        }
        else
            delete silAttsNode;
    }


    // Add the node to the parent node.
    if(addToParent || forceAdd)
        parentNode->AddNode(node);
    else
        delete node;

    return (addToParent || forceAdd);
}

// ****************************************************************************
// Method: SILRestrictionAttributes::SetFromNode
//
// Purpose: 
//   This method sets attributes in this object from values in a DataNode representation of the object.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

void
SILRestrictionAttributes::SetFromNode(DataNode *parentNode)
{
    if(parentNode == 0)
        return;

    DataNode *searchNode = parentNode->GetNode("SILRestrictionAttributes");
    if(searchNode == 0)
        return;

    DataNode *node;
    if((node = searchNode->GetNode("useSet")) != 0)
        SetUseSet(node->AsUnsignedCharVector());
    if((node = searchNode->GetNode("topSet")) != 0)
        SetTopSet(node->AsInt());
    if((node = searchNode->GetNode("silAtts")) != 0)
        silAtts.SetFromNode(node);
}

///////////////////////////////////////////////////////////////////////////////
// Set property methods
///////////////////////////////////////////////////////////////////////////////

void
SILRestrictionAttributes::SetUseSet(const unsignedCharVector &useSet_)
{
    useSet = useSet_;
    Select(ID_useSet, (void *)&useSet);
}

void
SILRestrictionAttributes::SetTopSet(int topSet_)
{
    topSet = topSet_;
    Select(ID_topSet, (void *)&topSet);
}

void
SILRestrictionAttributes::SetSilAtts(const SILAttributes &silAtts_)
{
    silAtts = silAtts_;
    Select(ID_silAtts, (void *)&silAtts);
}

///////////////////////////////////////////////////////////////////////////////
// Get property methods
///////////////////////////////////////////////////////////////////////////////

const unsignedCharVector &
SILRestrictionAttributes::GetUseSet() const
{
    return useSet;
}

unsignedCharVector &
SILRestrictionAttributes::GetUseSet()
{
    return useSet;
}

int
SILRestrictionAttributes::GetTopSet() const
{
    return topSet;
}

const SILAttributes &
SILRestrictionAttributes::GetSilAtts() const
{
    return silAtts;
}

SILAttributes &
SILRestrictionAttributes::GetSilAtts()
{
    return silAtts;
}

///////////////////////////////////////////////////////////////////////////////
// Select property methods
///////////////////////////////////////////////////////////////////////////////

void
SILRestrictionAttributes::SelectUseSet()
{
    Select(ID_useSet, (void *)&useSet);
}

void
SILRestrictionAttributes::SelectSilAtts()
{
    Select(ID_silAtts, (void *)&silAtts);
}

///////////////////////////////////////////////////////////////////////////////
// Keyframing methods
///////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
// Method: SILRestrictionAttributes::GetFieldName
//
// Purpose: 
//   This method returns the name of a field given its index.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

std::string
SILRestrictionAttributes::GetFieldName(int index) const
{
    switch (index)
    {
    case ID_useSet:  return "useSet";
    case ID_topSet:  return "topSet";
    case ID_silAtts: return "silAtts";
    default:  return "invalid index";
    }
}

// ****************************************************************************
// Method: SILRestrictionAttributes::GetFieldType
//
// Purpose: 
//   This method returns the type of a field given its index.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

AttributeGroup::FieldType
SILRestrictionAttributes::GetFieldType(int index) const
{
    switch (index)
    {
    case ID_useSet:  return FieldType_ucharVector;
    case ID_topSet:  return FieldType_int;
    case ID_silAtts: return FieldType_att;
    default:  return FieldType_unknown;
    }
}

// ****************************************************************************
// Method: SILRestrictionAttributes::GetFieldTypeName
//
// Purpose: 
//   This method returns the name of a field type given its index.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

std::string
SILRestrictionAttributes::GetFieldTypeName(int index) const
{
    switch (index)
    {
    case ID_useSet:  return "ucharVector";
    case ID_topSet:  return "int";
    case ID_silAtts: return "att";
    default:  return "invalid index";
    }
}

// ****************************************************************************
// Method: SILRestrictionAttributes::FieldsEqual
//
// Purpose: 
//   This method compares two fields and return true if they are equal.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

bool
SILRestrictionAttributes::FieldsEqual(int index_, const AttributeGroup *rhs) const
{
    const SILRestrictionAttributes &obj = *((const SILRestrictionAttributes*)rhs);
    bool retval = false;
    switch (index_)
    {
    case ID_useSet:
        {  // new scope
        retval = (useSet == obj.useSet);
        }
        break;
    case ID_topSet:
        {  // new scope
        retval = (topSet == obj.topSet);
        }
        break;
    case ID_silAtts:
        {  // new scope
        retval = (silAtts == obj.silAtts);
        }
        break;
    default: retval = false;
    }

    return retval;
}

///////////////////////////////////////////////////////////////////////////////
// User-defined methods.
///////////////////////////////////////////////////////////////////////////////

