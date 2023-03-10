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

#include <NameschemeAttributes.h>
#include <DataNode.h>

// This is a hack to work around fact that we cannot define variables
// of arbitrary type, like a map, in the state object itself. So, we
// do it using a map with key being the object's this pointer.
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

static map<const void*, Namescheme*> nameschemesMap;
static map<const void*, bool> getNameCalledMap;

// ****************************************************************************
// Method: NameschemeAttributes::NameschemeAttributes
//
// Purpose: 
//   Init utility for the NameschemeAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

void NameschemeAttributes::Init()
{

    NameschemeAttributes::SelectAll();
}

// ****************************************************************************
// Method: NameschemeAttributes::NameschemeAttributes
//
// Purpose: 
//   Copy utility for the NameschemeAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

void NameschemeAttributes::Copy(const NameschemeAttributes &obj)
{
    namescheme = obj.namescheme;
    externalArrayNames = obj.externalArrayNames;
    externalArrayOffsets = obj.externalArrayOffsets;
    externalArrayData = obj.externalArrayData;
    allExplicitNames = obj.allExplicitNames;
    explicitIds = obj.explicitIds;
    explicitNames = obj.explicitNames;

    NameschemeAttributes::SelectAll();
}

// Type map format string
const char *NameschemeAttributes::TypeMapFormatString = NAMESCHEMEATTRIBUTES_TMFS;
const AttributeGroup::private_tmfs_t NameschemeAttributes::TmfsStruct = {NAMESCHEMEATTRIBUTES_TMFS};


// ****************************************************************************
// Method: NameschemeAttributes::NameschemeAttributes
//
// Purpose: 
//   Default constructor for the NameschemeAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

NameschemeAttributes::NameschemeAttributes() : 
    AttributeSubject(NameschemeAttributes::TypeMapFormatString)
{
    NameschemeAttributes::Init();
}

// ****************************************************************************
// Method: NameschemeAttributes::NameschemeAttributes
//
// Purpose: 
//   Constructor for the derived classes of NameschemeAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

NameschemeAttributes::NameschemeAttributes(private_tmfs_t tmfs) : 
    AttributeSubject(tmfs.tmfs)
{
    NameschemeAttributes::Init();
}

// ****************************************************************************
// Method: NameschemeAttributes::NameschemeAttributes
//
// Purpose: 
//   Copy constructor for the NameschemeAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

NameschemeAttributes::NameschemeAttributes(const NameschemeAttributes &obj) : 
    AttributeSubject(NameschemeAttributes::TypeMapFormatString)
{
    NameschemeAttributes::Copy(obj);
}

// ****************************************************************************
// Method: NameschemeAttributes::NameschemeAttributes
//
// Purpose: 
//   Copy constructor for derived classes of the NameschemeAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

NameschemeAttributes::NameschemeAttributes(const NameschemeAttributes &obj, private_tmfs_t tmfs) : 
    AttributeSubject(tmfs.tmfs)
{
    NameschemeAttributes::Copy(obj);
}

// ****************************************************************************
// Method: NameschemeAttributes::~NameschemeAttributes
//
// Purpose: 
//   Destructor for the NameschemeAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

NameschemeAttributes::~NameschemeAttributes()
{
    // Clear any map entries for 'this' object.
    map<const void*, Namescheme*>::iterator it2 = nameschemesMap.find(this);
    if (it2 != nameschemesMap.end())
    {
        delete it2->second;
        nameschemesMap.erase(it2);
    }
    map<const void*, bool>::iterator it3 = getNameCalledMap.find(this);
    if (it3 != getNameCalledMap.end())
        getNameCalledMap.erase(it3);

}

// ****************************************************************************
// Method: NameschemeAttributes::operator = 
//
// Purpose: 
//   Assignment operator for the NameschemeAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

NameschemeAttributes& 
NameschemeAttributes::operator = (const NameschemeAttributes &obj)
{
    if (this == &obj) return *this;

    NameschemeAttributes::Copy(obj);

    return *this;
}

// ****************************************************************************
// Method: NameschemeAttributes::operator == 
//
// Purpose: 
//   Comparison operator == for the NameschemeAttributes class.
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
NameschemeAttributes::operator == (const NameschemeAttributes &obj) const
{
    // Create the return value
    return ((namescheme == obj.namescheme) &&
            (externalArrayNames == obj.externalArrayNames) &&
            (externalArrayOffsets == obj.externalArrayOffsets) &&
            (externalArrayData == obj.externalArrayData) &&
            (allExplicitNames == obj.allExplicitNames) &&
            (explicitIds == obj.explicitIds) &&
            (explicitNames == obj.explicitNames));
}

// ****************************************************************************
// Method: NameschemeAttributes::operator != 
//
// Purpose: 
//   Comparison operator != for the NameschemeAttributes class.
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
NameschemeAttributes::operator != (const NameschemeAttributes &obj) const
{
    return !(this->operator == (obj));
}

// ****************************************************************************
// Method: NameschemeAttributes::TypeName
//
// Purpose: 
//   Type name method for the NameschemeAttributes class.
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
NameschemeAttributes::TypeName() const
{
    return "NameschemeAttributes";
}

// ****************************************************************************
// Method: NameschemeAttributes::CopyAttributes
//
// Purpose: 
//   CopyAttributes method for the NameschemeAttributes class.
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
NameschemeAttributes::CopyAttributes(const AttributeGroup *atts)
{
    if(TypeName() != atts->TypeName())
        return false;

    // Call assignment operator.
    const NameschemeAttributes *tmp = (const NameschemeAttributes *)atts;
    *this = *tmp;

    return true;
}

// ****************************************************************************
// Method: NameschemeAttributes::CreateCompatible
//
// Purpose: 
//   CreateCompatible method for the NameschemeAttributes class.
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
NameschemeAttributes::CreateCompatible(const std::string &tname) const
{
    AttributeSubject *retval = 0;
    if(TypeName() == tname)
        retval = new NameschemeAttributes(*this);
    // Other cases could go here too. 

    return retval;
}

// ****************************************************************************
// Method: NameschemeAttributes::NewInstance
//
// Purpose: 
//   NewInstance method for the NameschemeAttributes class.
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
NameschemeAttributes::NewInstance(bool copy) const
{
    AttributeSubject *retval = 0;
    if(copy)
        retval = new NameschemeAttributes(*this);
    else
        retval = new NameschemeAttributes;

    return retval;
}

// ****************************************************************************
// Method: NameschemeAttributes::SelectAll
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
NameschemeAttributes::SelectAll()
{
    Select(ID_namescheme,           (void *)&namescheme);
    Select(ID_externalArrayNames,   (void *)&externalArrayNames);
    Select(ID_externalArrayOffsets, (void *)&externalArrayOffsets);
    Select(ID_externalArrayData,    (void *)&externalArrayData);
    Select(ID_allExplicitNames,     (void *)&allExplicitNames);
    Select(ID_explicitIds,          (void *)&explicitIds);
    Select(ID_explicitNames,        (void *)&explicitNames);
}

///////////////////////////////////////////////////////////////////////////////
// Persistence methods
///////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
// Method: NameschemeAttributes::CreateNode
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
NameschemeAttributes::CreateNode(DataNode *parentNode, bool completeSave, bool forceAdd)
{
    if(parentNode == 0)
        return false;

    NameschemeAttributes defaultObject;
    bool addToParent = false;
    // Create a node for NameschemeAttributes.
    DataNode *node = new DataNode("NameschemeAttributes");

    if(completeSave || !FieldsEqual(ID_namescheme, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("namescheme", namescheme));
    }

    if(completeSave || !FieldsEqual(ID_externalArrayNames, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("externalArrayNames", externalArrayNames));
    }

    if(completeSave || !FieldsEqual(ID_externalArrayOffsets, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("externalArrayOffsets", externalArrayOffsets));
    }

    if(completeSave || !FieldsEqual(ID_externalArrayData, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("externalArrayData", externalArrayData));
    }

    if(completeSave || !FieldsEqual(ID_allExplicitNames, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("allExplicitNames", allExplicitNames));
    }

    if(completeSave || !FieldsEqual(ID_explicitIds, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("explicitIds", explicitIds));
    }

    if(completeSave || !FieldsEqual(ID_explicitNames, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("explicitNames", explicitNames));
    }


    // Add the node to the parent node.
    if(addToParent || forceAdd)
        parentNode->AddNode(node);
    else
        delete node;

    return (addToParent || forceAdd);
}

// ****************************************************************************
// Method: NameschemeAttributes::SetFromNode
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
NameschemeAttributes::SetFromNode(DataNode *parentNode)
{
    if(parentNode == 0)
        return;

    DataNode *searchNode = parentNode->GetNode("NameschemeAttributes");
    if(searchNode == 0)
        return;

    DataNode *node;
    if((node = searchNode->GetNode("namescheme")) != 0)
        SetNamescheme(node->AsString());
    if((node = searchNode->GetNode("externalArrayNames")) != 0)
        SetExternalArrayNames(node->AsStringVector());
    if((node = searchNode->GetNode("externalArrayOffsets")) != 0)
        SetExternalArrayOffsets(node->AsIntVector());
    if((node = searchNode->GetNode("externalArrayData")) != 0)
        SetExternalArrayData(node->AsIntVector());
    if((node = searchNode->GetNode("allExplicitNames")) != 0)
        SetAllExplicitNames(node->AsStringVector());
    if((node = searchNode->GetNode("explicitIds")) != 0)
        SetExplicitIds(node->AsIntVector());
    if((node = searchNode->GetNode("explicitNames")) != 0)
        SetExplicitNames(node->AsStringVector());
}

///////////////////////////////////////////////////////////////////////////////
// Set property methods
///////////////////////////////////////////////////////////////////////////////

void
NameschemeAttributes::SetNamescheme(const std::string &namescheme_)
{
    namescheme = namescheme_;
    Select(ID_namescheme, (void *)&namescheme);
}

void
NameschemeAttributes::SetExternalArrayNames(const stringVector &externalArrayNames_)
{
    externalArrayNames = externalArrayNames_;
    Select(ID_externalArrayNames, (void *)&externalArrayNames);
}

void
NameschemeAttributes::SetExternalArrayOffsets(const intVector &externalArrayOffsets_)
{
    externalArrayOffsets = externalArrayOffsets_;
    Select(ID_externalArrayOffsets, (void *)&externalArrayOffsets);
}

void
NameschemeAttributes::SetExternalArrayData(const intVector &externalArrayData_)
{
    externalArrayData = externalArrayData_;
    Select(ID_externalArrayData, (void *)&externalArrayData);
}

void
NameschemeAttributes::SetAllExplicitNames(const stringVector &allExplicitNames_)
{
    allExplicitNames = allExplicitNames_;
    Select(ID_allExplicitNames, (void *)&allExplicitNames);
}

void
NameschemeAttributes::SetExplicitIds(const intVector &explicitIds_)
{
    explicitIds = explicitIds_;
    Select(ID_explicitIds, (void *)&explicitIds);
}

void
NameschemeAttributes::SetExplicitNames(const stringVector &explicitNames_)
{
    explicitNames = explicitNames_;
    Select(ID_explicitNames, (void *)&explicitNames);
}

///////////////////////////////////////////////////////////////////////////////
// Get property methods
///////////////////////////////////////////////////////////////////////////////

const std::string &
NameschemeAttributes::GetNamescheme() const
{
    return namescheme;
}

std::string &
NameschemeAttributes::GetNamescheme()
{
    return namescheme;
}

const stringVector &
NameschemeAttributes::GetExternalArrayNames() const
{
    return externalArrayNames;
}

stringVector &
NameschemeAttributes::GetExternalArrayNames()
{
    return externalArrayNames;
}

const intVector &
NameschemeAttributes::GetExternalArrayOffsets() const
{
    return externalArrayOffsets;
}

intVector &
NameschemeAttributes::GetExternalArrayOffsets()
{
    return externalArrayOffsets;
}

const intVector &
NameschemeAttributes::GetExternalArrayData() const
{
    return externalArrayData;
}

intVector &
NameschemeAttributes::GetExternalArrayData()
{
    return externalArrayData;
}

const stringVector &
NameschemeAttributes::GetAllExplicitNames() const
{
    return allExplicitNames;
}

stringVector &
NameschemeAttributes::GetAllExplicitNames()
{
    return allExplicitNames;
}

const intVector &
NameschemeAttributes::GetExplicitIds() const
{
    return explicitIds;
}

intVector &
NameschemeAttributes::GetExplicitIds()
{
    return explicitIds;
}

const stringVector &
NameschemeAttributes::GetExplicitNames() const
{
    return explicitNames;
}

stringVector &
NameschemeAttributes::GetExplicitNames()
{
    return explicitNames;
}

///////////////////////////////////////////////////////////////////////////////
// Select property methods
///////////////////////////////////////////////////////////////////////////////

void
NameschemeAttributes::SelectNamescheme()
{
    Select(ID_namescheme, (void *)&namescheme);
}

void
NameschemeAttributes::SelectExternalArrayNames()
{
    Select(ID_externalArrayNames, (void *)&externalArrayNames);
}

void
NameschemeAttributes::SelectExternalArrayOffsets()
{
    Select(ID_externalArrayOffsets, (void *)&externalArrayOffsets);
}

void
NameschemeAttributes::SelectExternalArrayData()
{
    Select(ID_externalArrayData, (void *)&externalArrayData);
}

void
NameschemeAttributes::SelectAllExplicitNames()
{
    Select(ID_allExplicitNames, (void *)&allExplicitNames);
}

void
NameschemeAttributes::SelectExplicitIds()
{
    Select(ID_explicitIds, (void *)&explicitIds);
}

void
NameschemeAttributes::SelectExplicitNames()
{
    Select(ID_explicitNames, (void *)&explicitNames);
}

///////////////////////////////////////////////////////////////////////////////
// Keyframing methods
///////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
// Method: NameschemeAttributes::GetFieldName
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
NameschemeAttributes::GetFieldName(int index) const
{
    switch (index)
    {
    case ID_namescheme:           return "namescheme";
    case ID_externalArrayNames:   return "externalArrayNames";
    case ID_externalArrayOffsets: return "externalArrayOffsets";
    case ID_externalArrayData:    return "externalArrayData";
    case ID_allExplicitNames:     return "allExplicitNames";
    case ID_explicitIds:          return "explicitIds";
    case ID_explicitNames:        return "explicitNames";
    default:  return "invalid index";
    }
}

// ****************************************************************************
// Method: NameschemeAttributes::GetFieldType
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
NameschemeAttributes::GetFieldType(int index) const
{
    switch (index)
    {
    case ID_namescheme:           return FieldType_string;
    case ID_externalArrayNames:   return FieldType_stringVector;
    case ID_externalArrayOffsets: return FieldType_intVector;
    case ID_externalArrayData:    return FieldType_intVector;
    case ID_allExplicitNames:     return FieldType_stringVector;
    case ID_explicitIds:          return FieldType_intVector;
    case ID_explicitNames:        return FieldType_stringVector;
    default:  return FieldType_unknown;
    }
}

// ****************************************************************************
// Method: NameschemeAttributes::GetFieldTypeName
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
NameschemeAttributes::GetFieldTypeName(int index) const
{
    switch (index)
    {
    case ID_namescheme:           return "string";
    case ID_externalArrayNames:   return "stringVector";
    case ID_externalArrayOffsets: return "intVector";
    case ID_externalArrayData:    return "intVector";
    case ID_allExplicitNames:     return "stringVector";
    case ID_explicitIds:          return "intVector";
    case ID_explicitNames:        return "stringVector";
    default:  return "invalid index";
    }
}

// ****************************************************************************
// Method: NameschemeAttributes::FieldsEqual
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
NameschemeAttributes::FieldsEqual(int index_, const AttributeGroup *rhs) const
{
    const NameschemeAttributes &obj = *((const NameschemeAttributes*)rhs);
    bool retval = false;
    switch (index_)
    {
    case ID_namescheme:
        {  // new scope
        retval = (namescheme == obj.namescheme);
        }
        break;
    case ID_externalArrayNames:
        {  // new scope
        retval = (externalArrayNames == obj.externalArrayNames);
        }
        break;
    case ID_externalArrayOffsets:
        {  // new scope
        retval = (externalArrayOffsets == obj.externalArrayOffsets);
        }
        break;
    case ID_externalArrayData:
        {  // new scope
        retval = (externalArrayData == obj.externalArrayData);
        }
        break;
    case ID_allExplicitNames:
        {  // new scope
        retval = (allExplicitNames == obj.allExplicitNames);
        }
        break;
    case ID_explicitIds:
        {  // new scope
        retval = (explicitIds == obj.explicitIds);
        }
        break;
    case ID_explicitNames:
        {  // new scope
        retval = (explicitNames == obj.explicitNames);
        }
        break;
    default: retval = false;
    }

    return retval;
}

///////////////////////////////////////////////////////////////////////////////
// User-defined methods.
///////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//  Method: AddExternalArray
//
//  Purpose: Add an array of integer values that is referenced from the
//  namescheme expression.
//
//  Programmer: Mark C. Miller
//  Creation:   August 28, 2009
//
// ****************************************************************************

void NameschemeAttributes::AddExternalArray(const char * name, const int *data, int len)
{
    CheckIfGetNameCalled();

    externalArrayNames.push_back(name);
    externalArrayOffsets.push_back(externalArrayData.size());
    for (int i = 0; i < len; i++)
        externalArrayData.push_back(data[i]);
}

// ****************************************************************************
// See AddExternalArray
// ****************************************************************************
void NameschemeAttributes::AddExternalArray(const string &name, const vector<int> &data)
{
    AddExternalArray(name.c_str(), &data[0], (int) data.size());
}

// ****************************************************************************
//  Method: SetAllNames
//
//  Purpose: Set ALL names to explicit values.
//
//  In this case, since every name in the namescheme is an explicitly
//  numerated name, it is assumed the names passed here are in 1:1 with
//  the natural numbers so that GetName(n) returns the name at index n
//  passed here.
//
//  Programmer: Mark C. Miller
//  Creation:   August 28, 2009
//
// ****************************************************************************
void NameschemeAttributes::SetAllNames(const char *const *const names, int count)
{
    CheckIfGetNameCalled();

    for (int i = 0; i < count; i++)
        allExplicitNames.push_back(names[i]);
}

// ****************************************************************************
//  Method: SetName
//
//  Purpose: Explicitly set name for index i in the namescheme.
//
//  Programmer: Mark C. Miller
//  Creation:   August 28, 2009
//
// ****************************************************************************

void NameschemeAttributes::SetName(int i, const char *name)
{
    CheckIfGetNameCalled();

    explicitIds.push_back(i);
    explicitNames.push_back(name);
}

// ****************************************************************************
// See SetName
// ****************************************************************************
void NameschemeAttributes::SetName(int i, const string &n)
{
    SetName(i, n.c_str());
}

// ****************************************************************************
//  Method: GetName.
//
//  Purpose: Main workhorse of this method. Compute a name from the namescheme.
//
//  The FIRST call to this method indicates that caller is done building up
//  the object and ready to start using it to compute names.
//
//  Programmer: Mark C. Miller
//  Creation:   August 28, 2009
//
//    Mark C. Miller, Tue Dec  8 14:33:27 PST 2009
//    Added const qualification to map keys. Const qualified the method itself.
// ****************************************************************************

#define EA(N) &externalArrayData[externalArrayOffsets[N]]

const char *NameschemeAttributes::GetName(int n) const
{
    // First, determine if this is FIRST call to GetName on this object
    map<const void*, bool>::const_iterator cit4 = getNameCalledMap.find(this);
    if (cit4 == getNameCalledMap.end())
    {
        // This is first call. Indicate that in the map.
        getNameCalledMap[this] = true;
    }

    // First, see if we have ALL explicit names defined and, if so,
    // ensure 'n' is in correct range for it.
    if ((size_t)n < allExplicitNames.size())
        return allExplicitNames[n].c_str();
        
    // Next, see if we have an explicit names map for this object.
    if (explicitIds.size())
    {
        // If there is an explicit name for this entry, return it.
        for (size_t i = 0; i < explicitIds.size(); i++)
        {
            if (explicitIds[i] == n)
                return explicitNames[i].c_str();
        }
    }

    // If we get here, we didn't have an explicit name, so
    // use our Namescheme class to compute the name.
    static char tmpname[128];
    if (namescheme == "")
    {
        SNPRINTF(tmpname, sizeof(tmpname), "unknown_name_%d", n);
        return tmpname;
    }

    // First, lets see if we've already got one constructed for this object.
    Namescheme *ns = 0;
    map<const void*, Namescheme*>::const_iterator cit3 = nameschemesMap.find(this);
    if (cit3 != nameschemesMap.end())
    {
        ns = cit3->second;
    }
    else
    {
        // We didn't find a Namescheme object. That means we've yet to create
        // it for this object. Create it now.

        // We could make this switch larger to accomodate a larger number
        // of external array references. But, for now, we're setting a 
        // limit at 4. If we want to increase it, we need to make sure the
        // cooresponding Namescheme class can support it.
        switch(externalArrayNames.size())
        {
            case 0: ns = new Namescheme(namescheme.c_str()); break;
            case 1: ns = new Namescheme(namescheme.c_str(), EA(0)); break;
            case 2: ns = new Namescheme(namescheme.c_str(), EA(0), EA(1)); break;
            case 3: ns = new Namescheme(namescheme.c_str(), EA(0), EA(1), EA(2)); break;
            case 4: ns = new Namescheme(namescheme.c_str(), EA(0), EA(1), EA(2), EA(3)); break;
            default: { EXCEPTION0(StateObjectException); }
        }

        // Store the Namescheme pointer in the map for this object so
        // we can find it again when we need to.
        nameschemesMap[this] = ns;
    }

    // Compute the name from the Namescheme
    const char *retval = ns ? ns->GetName(n) : 0;
    if (retval && retval[0] != '\0') return retval;

    // Ok, we can't compute a name. Make one up.
    SNPRINTF(tmpname, sizeof(tmpname), "unknown_name_%d", n);
    return tmpname;
}

// ****************************************************************************
// See SetAllNames
// ****************************************************************************
void NameschemeAttributes::SetAllNames(const vector<string> &names)
{
    CheckIfGetNameCalled();

    for (size_t i = 0; i < names.size(); i++)
        allExplicitNames.push_back(names[i]);
}

// ****************************************************************************
//  Method: CheckIfGetNameCalled.
//
//  Purpose: An attempt to call GetName on this object is an indication
//  that the object's specification is 'complete' in that no more calls to
//  either add external arrays or set explict names will be made. This
//  method is merely a helper function to ensure that behavior is enforced.
//
//  Programmer: Mark C. Miller
//  Creation:   August 28, 2009
//
//    Mark C. Miller, Tue Dec  8 14:33:27 PST 2009
//    Added const qualification to map key
// ****************************************************************************

void NameschemeAttributes::CheckIfGetNameCalled()
{
    map<const void*,bool>::const_iterator cit = getNameCalledMap.find(this);
    if (cit == getNameCalledMap.end())
        return;
    EXCEPTION0(StateObjectException);
}

