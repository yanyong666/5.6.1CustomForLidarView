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

#ifndef AVTSIMULATIONINFORMATION_H
#define AVTSIMULATIONINFORMATION_H
#include <dbatts_exports.h>
#include <string>
#include <AttributeSubject.h>

class avtSimulationCommandSpecification;
class avtSimulationCommandSpecification;

// ****************************************************************************
// Class: avtSimulationInformation
//
// Purpose:
//    Contains information about simulation connections
//
// Notes:      Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

class DBATTS_API avtSimulationInformation : public AttributeSubject
{
public:
    enum RunMode
    {
        Unknown = 0,
        Running = 1,
        Stopped = 2
    };

    // These constructors are for objects of this class
    avtSimulationInformation();
    avtSimulationInformation(const avtSimulationInformation &obj);
protected:
    // These constructors are for objects derived from this class
    avtSimulationInformation(private_tmfs_t tmfs);
    avtSimulationInformation(const avtSimulationInformation &obj, private_tmfs_t tmfs);
public:
    virtual ~avtSimulationInformation();

    virtual avtSimulationInformation& operator = (const avtSimulationInformation &obj);
    virtual bool operator == (const avtSimulationInformation &obj) const;
    virtual bool operator != (const avtSimulationInformation &obj) const;
private:
    void Init();
    void Copy(const avtSimulationInformation &obj);
public:

    virtual const std::string TypeName() const;
    virtual bool CopyAttributes(const AttributeGroup *);
    virtual AttributeSubject *CreateCompatible(const std::string &) const;
    virtual AttributeSubject *NewInstance(bool) const;

    // Property selection methods
    virtual void SelectAll();
    void SelectHost();
    void SelectSecurityKey();
    void SelectOtherNames();
    void SelectOtherValues();
    void SelectGenericCommands();
    void SelectCustomCommands();
    void SelectMessage();

    // Property setting methods
    void SetHost(const std::string &host_);
    void SetPort(int port_);
    void SetSecurityKey(const std::string &securityKey_);
    void SetOtherNames(const stringVector &otherNames_);
    void SetOtherValues(const stringVector &otherValues_);
    void SetMode(RunMode mode_);
    void SetMessage(const std::string &message_);

    // Property getting methods
    const std::string  &GetHost() const;
          std::string  &GetHost();
    int                GetPort() const;
    const std::string  &GetSecurityKey() const;
          std::string  &GetSecurityKey();
    const stringVector &GetOtherNames() const;
          stringVector &GetOtherNames();
    const stringVector &GetOtherValues() const;
          stringVector &GetOtherValues();
    const AttributeGroupVector &GetGenericCommands() const;
          AttributeGroupVector &GetGenericCommands();
    RunMode            GetMode() const;
    const AttributeGroupVector &GetCustomCommands() const;
          AttributeGroupVector &GetCustomCommands();
    const std::string  &GetMessage() const;
          std::string  &GetMessage();


    // Attributegroup convenience methods
    void AddGenericCommands(const avtSimulationCommandSpecification &);
    void ClearGenericCommands();
    void RemoveGenericCommands(int i);
    int  GetNumGenericCommands() const;
    avtSimulationCommandSpecification &GetGenericCommands(int i);
    const avtSimulationCommandSpecification &GetGenericCommands(int i) const;

    void AddCustomCommands(const avtSimulationCommandSpecification &);
    void ClearCustomCommands();
    void RemoveCustomCommands(int i);
    int  GetNumCustomCommands() const;
    avtSimulationCommandSpecification &GetCustomCommands(int i);
    const avtSimulationCommandSpecification &GetCustomCommands(int i) const;

    // Enum conversion functions
    static std::string RunMode_ToString(RunMode);
    static bool RunMode_FromString(const std::string &, RunMode &);
protected:
    static std::string RunMode_ToString(int);
public:

    // Keyframing methods
    virtual std::string               GetFieldName(int index) const;
    virtual AttributeGroup::FieldType GetFieldType(int index) const;
    virtual std::string               GetFieldTypeName(int index) const;
    virtual bool                      FieldsEqual(int index, const AttributeGroup *rhs) const;


    // IDs that can be used to identify fields in case statements
    enum {
        ID_host = 0,
        ID_port,
        ID_securityKey,
        ID_otherNames,
        ID_otherValues,
        ID_genericCommands,
        ID_mode,
        ID_customCommands,
        ID_message,
        ID__LAST
    };

protected:
    AttributeGroup *CreateSubAttributeGroup(int index);
private:
    std::string          host;
    int                  port;
    std::string          securityKey;
    stringVector         otherNames;
    stringVector         otherValues;
    AttributeGroupVector genericCommands;
    int                  mode;
    AttributeGroupVector customCommands;
    std::string          message;

    // Static class format string for type map.
    static const char *TypeMapFormatString;
    static const private_tmfs_t TmfsStruct;
};
#define AVTSIMULATIONINFORMATION_TMFS "siss*s*a*ia*s"

#endif
