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

#include <ViewCurveAttributes.h>
#include <DataNode.h>

// ****************************************************************************
// Method: ViewCurveAttributes::ViewCurveAttributes
//
// Purpose: 
//   Init utility for the ViewCurveAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

void ViewCurveAttributes::Init()
{
    domainCoords[0] = 0;
    domainCoords[1] = 1;
    rangeCoords[0] = 0;
    rangeCoords[1] = 1;
    viewportCoords[0] = 0.1;
    viewportCoords[1] = 0.1;
    viewportCoords[2] = 0.9;
    viewportCoords[3] = 0.9;
    domainScale = 0;
    rangeScale = 0;

    ViewCurveAttributes::SelectAll();
}

// ****************************************************************************
// Method: ViewCurveAttributes::ViewCurveAttributes
//
// Purpose: 
//   Copy utility for the ViewCurveAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

void ViewCurveAttributes::Copy(const ViewCurveAttributes &obj)
{

    domainCoords[0] = obj.domainCoords[0];
    domainCoords[1] = obj.domainCoords[1];

    rangeCoords[0] = obj.rangeCoords[0];
    rangeCoords[1] = obj.rangeCoords[1];

    for(int i = 0; i < 4; ++i)
        viewportCoords[i] = obj.viewportCoords[i];

    domainScale = obj.domainScale;
    rangeScale = obj.rangeScale;

    ViewCurveAttributes::SelectAll();
}

// Type map format string
const char *ViewCurveAttributes::TypeMapFormatString = VIEWCURVEATTRIBUTES_TMFS;
const AttributeGroup::private_tmfs_t ViewCurveAttributes::TmfsStruct = {VIEWCURVEATTRIBUTES_TMFS};


// ****************************************************************************
// Method: ViewCurveAttributes::ViewCurveAttributes
//
// Purpose: 
//   Default constructor for the ViewCurveAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

ViewCurveAttributes::ViewCurveAttributes() : 
    AttributeSubject(ViewCurveAttributes::TypeMapFormatString)
{
    ViewCurveAttributes::Init();
}

// ****************************************************************************
// Method: ViewCurveAttributes::ViewCurveAttributes
//
// Purpose: 
//   Constructor for the derived classes of ViewCurveAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

ViewCurveAttributes::ViewCurveAttributes(private_tmfs_t tmfs) : 
    AttributeSubject(tmfs.tmfs)
{
    ViewCurveAttributes::Init();
}

// ****************************************************************************
// Method: ViewCurveAttributes::ViewCurveAttributes
//
// Purpose: 
//   Copy constructor for the ViewCurveAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

ViewCurveAttributes::ViewCurveAttributes(const ViewCurveAttributes &obj) : 
    AttributeSubject(ViewCurveAttributes::TypeMapFormatString)
{
    ViewCurveAttributes::Copy(obj);
}

// ****************************************************************************
// Method: ViewCurveAttributes::ViewCurveAttributes
//
// Purpose: 
//   Copy constructor for derived classes of the ViewCurveAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

ViewCurveAttributes::ViewCurveAttributes(const ViewCurveAttributes &obj, private_tmfs_t tmfs) : 
    AttributeSubject(tmfs.tmfs)
{
    ViewCurveAttributes::Copy(obj);
}

// ****************************************************************************
// Method: ViewCurveAttributes::~ViewCurveAttributes
//
// Purpose: 
//   Destructor for the ViewCurveAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

ViewCurveAttributes::~ViewCurveAttributes()
{
    // nothing here
}

// ****************************************************************************
// Method: ViewCurveAttributes::operator = 
//
// Purpose: 
//   Assignment operator for the ViewCurveAttributes class.
//
// Note:       Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

ViewCurveAttributes& 
ViewCurveAttributes::operator = (const ViewCurveAttributes &obj)
{
    if (this == &obj) return *this;

    ViewCurveAttributes::Copy(obj);

    return *this;
}

// ****************************************************************************
// Method: ViewCurveAttributes::operator == 
//
// Purpose: 
//   Comparison operator == for the ViewCurveAttributes class.
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
ViewCurveAttributes::operator == (const ViewCurveAttributes &obj) const
{
    // Compare the domainCoords arrays.
    bool domainCoords_equal = true;
    for(int i = 0; i < 2 && domainCoords_equal; ++i)
        domainCoords_equal = (domainCoords[i] == obj.domainCoords[i]);

    // Compare the rangeCoords arrays.
    bool rangeCoords_equal = true;
    for(int i = 0; i < 2 && rangeCoords_equal; ++i)
        rangeCoords_equal = (rangeCoords[i] == obj.rangeCoords[i]);

    // Compare the viewportCoords arrays.
    bool viewportCoords_equal = true;
    for(int i = 0; i < 4 && viewportCoords_equal; ++i)
        viewportCoords_equal = (viewportCoords[i] == obj.viewportCoords[i]);

    // Create the return value
    return (domainCoords_equal &&
            rangeCoords_equal &&
            viewportCoords_equal &&
            (domainScale == obj.domainScale) &&
            (rangeScale == obj.rangeScale));
}

// ****************************************************************************
// Method: ViewCurveAttributes::operator != 
//
// Purpose: 
//   Comparison operator != for the ViewCurveAttributes class.
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
ViewCurveAttributes::operator != (const ViewCurveAttributes &obj) const
{
    return !(this->operator == (obj));
}

// ****************************************************************************
// Method: ViewCurveAttributes::TypeName
//
// Purpose: 
//   Type name method for the ViewCurveAttributes class.
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
ViewCurveAttributes::TypeName() const
{
    return "ViewCurveAttributes";
}

// ****************************************************************************
// Method: ViewCurveAttributes::CopyAttributes
//
// Purpose: 
//   CopyAttributes method for the ViewCurveAttributes class.
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
ViewCurveAttributes::CopyAttributes(const AttributeGroup *atts)
{
    if(TypeName() != atts->TypeName())
        return false;

    // Call assignment operator.
    const ViewCurveAttributes *tmp = (const ViewCurveAttributes *)atts;
    *this = *tmp;

    return true;
}

// ****************************************************************************
// Method: ViewCurveAttributes::CreateCompatible
//
// Purpose: 
//   CreateCompatible method for the ViewCurveAttributes class.
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
ViewCurveAttributes::CreateCompatible(const std::string &tname) const
{
    AttributeSubject *retval = 0;
    if(TypeName() == tname)
        retval = new ViewCurveAttributes(*this);
    // Other cases could go here too. 

    return retval;
}

// ****************************************************************************
// Method: ViewCurveAttributes::NewInstance
//
// Purpose: 
//   NewInstance method for the ViewCurveAttributes class.
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
ViewCurveAttributes::NewInstance(bool copy) const
{
    AttributeSubject *retval = 0;
    if(copy)
        retval = new ViewCurveAttributes(*this);
    else
        retval = new ViewCurveAttributes;

    return retval;
}

// ****************************************************************************
// Method: ViewCurveAttributes::SelectAll
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
ViewCurveAttributes::SelectAll()
{
    Select(ID_domainCoords,   (void *)domainCoords, 2);
    Select(ID_rangeCoords,    (void *)rangeCoords, 2);
    Select(ID_viewportCoords, (void *)viewportCoords, 4);
    Select(ID_domainScale,    (void *)&domainScale);
    Select(ID_rangeScale,     (void *)&rangeScale);
}

///////////////////////////////////////////////////////////////////////////////
// Persistence methods
///////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
// Method: ViewCurveAttributes::CreateNode
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
ViewCurveAttributes::CreateNode(DataNode *parentNode, bool completeSave, bool forceAdd)
{
    if(parentNode == 0)
        return false;

    ViewCurveAttributes defaultObject;
    bool addToParent = false;
    // Create a node for ViewCurveAttributes.
    DataNode *node = new DataNode("ViewCurveAttributes");

    if(completeSave || !FieldsEqual(ID_domainCoords, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("domainCoords", domainCoords, 2));
    }

    if(completeSave || !FieldsEqual(ID_rangeCoords, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("rangeCoords", rangeCoords, 2));
    }

    if(completeSave || !FieldsEqual(ID_viewportCoords, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("viewportCoords", viewportCoords, 4));
    }

    if(completeSave || !FieldsEqual(ID_domainScale, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("domainScale", domainScale));
    }

    if(completeSave || !FieldsEqual(ID_rangeScale, &defaultObject))
    {
        addToParent = true;
        node->AddNode(new DataNode("rangeScale", rangeScale));
    }


    // Add the node to the parent node.
    if(addToParent || forceAdd)
        parentNode->AddNode(node);
    else
        delete node;

    return (addToParent || forceAdd);
}

// ****************************************************************************
// Method: ViewCurveAttributes::SetFromNode
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
ViewCurveAttributes::SetFromNode(DataNode *parentNode)
{
    if(parentNode == 0)
        return;

    DataNode *searchNode = parentNode->GetNode("ViewCurveAttributes");
    if(searchNode == 0)
        return;

    DataNode *node;
    if((node = searchNode->GetNode("domainCoords")) != 0)
        SetDomainCoords(node->AsDoubleArray());
    if((node = searchNode->GetNode("rangeCoords")) != 0)
        SetRangeCoords(node->AsDoubleArray());
    if((node = searchNode->GetNode("viewportCoords")) != 0)
        SetViewportCoords(node->AsDoubleArray());
    if((node = searchNode->GetNode("domainScale")) != 0)
        SetDomainScale(node->AsInt());
    if((node = searchNode->GetNode("rangeScale")) != 0)
        SetRangeScale(node->AsInt());
}

///////////////////////////////////////////////////////////////////////////////
// Set property methods
///////////////////////////////////////////////////////////////////////////////

void
ViewCurveAttributes::SetDomainCoords(const double *domainCoords_)
{
    domainCoords[0] = domainCoords_[0];
    domainCoords[1] = domainCoords_[1];
    Select(ID_domainCoords, (void *)domainCoords, 2);
}

void
ViewCurveAttributes::SetRangeCoords(const double *rangeCoords_)
{
    rangeCoords[0] = rangeCoords_[0];
    rangeCoords[1] = rangeCoords_[1];
    Select(ID_rangeCoords, (void *)rangeCoords, 2);
}

void
ViewCurveAttributes::SetViewportCoords(const double *viewportCoords_)
{
    viewportCoords[0] = viewportCoords_[0];
    viewportCoords[1] = viewportCoords_[1];
    viewportCoords[2] = viewportCoords_[2];
    viewportCoords[3] = viewportCoords_[3];
    Select(ID_viewportCoords, (void *)viewportCoords, 4);
}

void
ViewCurveAttributes::SetDomainScale(int domainScale_)
{
    domainScale = domainScale_;
    Select(ID_domainScale, (void *)&domainScale);
}

void
ViewCurveAttributes::SetRangeScale(int rangeScale_)
{
    rangeScale = rangeScale_;
    Select(ID_rangeScale, (void *)&rangeScale);
}

///////////////////////////////////////////////////////////////////////////////
// Get property methods
///////////////////////////////////////////////////////////////////////////////

const double *
ViewCurveAttributes::GetDomainCoords() const
{
    return domainCoords;
}

double *
ViewCurveAttributes::GetDomainCoords()
{
    return domainCoords;
}

const double *
ViewCurveAttributes::GetRangeCoords() const
{
    return rangeCoords;
}

double *
ViewCurveAttributes::GetRangeCoords()
{
    return rangeCoords;
}

const double *
ViewCurveAttributes::GetViewportCoords() const
{
    return viewportCoords;
}

double *
ViewCurveAttributes::GetViewportCoords()
{
    return viewportCoords;
}

int
ViewCurveAttributes::GetDomainScale() const
{
    return domainScale;
}

int
ViewCurveAttributes::GetRangeScale() const
{
    return rangeScale;
}

///////////////////////////////////////////////////////////////////////////////
// Select property methods
///////////////////////////////////////////////////////////////////////////////

void
ViewCurveAttributes::SelectDomainCoords()
{
    Select(ID_domainCoords, (void *)domainCoords, 2);
}

void
ViewCurveAttributes::SelectRangeCoords()
{
    Select(ID_rangeCoords, (void *)rangeCoords, 2);
}

void
ViewCurveAttributes::SelectViewportCoords()
{
    Select(ID_viewportCoords, (void *)viewportCoords, 4);
}

///////////////////////////////////////////////////////////////////////////////
// Keyframing methods
///////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
// Method: ViewCurveAttributes::GetFieldName
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
ViewCurveAttributes::GetFieldName(int index) const
{
    switch (index)
    {
    case ID_domainCoords:   return "domainCoords";
    case ID_rangeCoords:    return "rangeCoords";
    case ID_viewportCoords: return "viewportCoords";
    case ID_domainScale:    return "domainScale";
    case ID_rangeScale:     return "rangeScale";
    default:  return "invalid index";
    }
}

// ****************************************************************************
// Method: ViewCurveAttributes::GetFieldType
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
ViewCurveAttributes::GetFieldType(int index) const
{
    switch (index)
    {
    case ID_domainCoords:   return FieldType_doubleArray;
    case ID_rangeCoords:    return FieldType_doubleArray;
    case ID_viewportCoords: return FieldType_doubleArray;
    case ID_domainScale:    return FieldType_scalemode;
    case ID_rangeScale:     return FieldType_scalemode;
    default:  return FieldType_unknown;
    }
}

// ****************************************************************************
// Method: ViewCurveAttributes::GetFieldTypeName
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
ViewCurveAttributes::GetFieldTypeName(int index) const
{
    switch (index)
    {
    case ID_domainCoords:   return "doubleArray";
    case ID_rangeCoords:    return "doubleArray";
    case ID_viewportCoords: return "doubleArray";
    case ID_domainScale:    return "scalemode";
    case ID_rangeScale:     return "scalemode";
    default:  return "invalid index";
    }
}

// ****************************************************************************
// Method: ViewCurveAttributes::FieldsEqual
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
ViewCurveAttributes::FieldsEqual(int index_, const AttributeGroup *rhs) const
{
    const ViewCurveAttributes &obj = *((const ViewCurveAttributes*)rhs);
    bool retval = false;
    switch (index_)
    {
    case ID_domainCoords:
        {  // new scope
        // Compare the domainCoords arrays.
        bool domainCoords_equal = true;
        for(int i = 0; i < 2 && domainCoords_equal; ++i)
            domainCoords_equal = (domainCoords[i] == obj.domainCoords[i]);

        retval = domainCoords_equal;
        }
        break;
    case ID_rangeCoords:
        {  // new scope
        // Compare the rangeCoords arrays.
        bool rangeCoords_equal = true;
        for(int i = 0; i < 2 && rangeCoords_equal; ++i)
            rangeCoords_equal = (rangeCoords[i] == obj.rangeCoords[i]);

        retval = rangeCoords_equal;
        }
        break;
    case ID_viewportCoords:
        {  // new scope
        // Compare the viewportCoords arrays.
        bool viewportCoords_equal = true;
        for(int i = 0; i < 4 && viewportCoords_equal; ++i)
            viewportCoords_equal = (viewportCoords[i] == obj.viewportCoords[i]);

        retval = viewportCoords_equal;
        }
        break;
    case ID_domainScale:
        {  // new scope
        retval = (domainScale == obj.domainScale);
        }
        break;
    case ID_rangeScale:
        {  // new scope
        retval = (rangeScale == obj.rangeScale);
        }
        break;
    default: retval = false;
    }

    return retval;
}

///////////////////////////////////////////////////////////////////////////////
// User-defined methods.
///////////////////////////////////////////////////////////////////////////////

