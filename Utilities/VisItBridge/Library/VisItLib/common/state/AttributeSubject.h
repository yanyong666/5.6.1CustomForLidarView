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

#ifndef ATTRIBUTESUBJECT_H
#define ATTRIBUTESUBJECT_H
#include <state_exports.h>
#include <AttributeGroup.h>
#include <Subject.h>

// ****************************************************************************
// Class: AttributeSubject
//
// Purpose:
//   This is a base class for state objects that can be transmitted
//   across a connection. It inherits an interface from Subject so it
//   can tell its observers when it changes.
//
// Notes:      
//
// Programmer: Brad Whitlock
// Creation:   Mon Aug 7 12:52:00 PDT 2000
//
// Modifications:
//   Kathleen Bonnell, Fri Oct 19 15:33:35 PDT 2001
//   Added virtual method VarChangeRequiresReset.
//
//   Brad Whitlock, Mon Feb 11 15:26:34 PST 2002
//   Added a new method to create compatible types.
//
//   Brad Whitlock, Wed Jul 23 11:15:49 PDT 2003
//   Added a new method to create a new instance that does not have to
//   be initialized from the calling object.
//
//   Kathleen Bonnell, Tue Jun 20 16:02:38 PDT 2006
//   Added virtual method TypeName so that derived classes can have their
//   names printed in log files. 
//
//   Burlen Loring, Thu Sep 24 11:35:22 PDT 2015
//   I removed a compiler warning.
//
// ****************************************************************************

class STATE_API AttributeSubject : public AttributeGroup, public Subject
{
public:
    AttributeSubject(const char *);
    virtual ~AttributeSubject();
    virtual void SelectAll() = 0;
    virtual const std::string TypeName() const; 
    virtual void Notify();
    virtual AttributeSubject *CreateCompatible(const std::string &) const;
    virtual AttributeSubject *NewInstance(bool) const { return 0; };

    virtual bool VarChangeRequiresReset(void) { return false; };
};

#endif
