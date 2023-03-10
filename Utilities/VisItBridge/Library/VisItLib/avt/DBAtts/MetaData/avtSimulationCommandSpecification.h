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

#ifndef AVTSIMULATIONCOMMANDSPECIFICATION_H
#define AVTSIMULATIONCOMMANDSPECIFICATION_H
#include <dbatts_exports.h>
#include <string>
#include <AttributeSubject.h>


// ****************************************************************************
// Class: avtSimulationCommandSpecification
//
// Purpose:
//    Contains the specification for one command
//
// Notes:      Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

class DBATTS_API avtSimulationCommandSpecification : public AttributeSubject
{
    using AttributeSubject::SetValue;
    using AttributeSubject::GetValue;
public:
    enum RunMode
    {
        Unknown,
        Running,
        Stopped
    };
    enum CommandArgumentType
    {
        CmdArgNone,
        CmdArgInt,
        CmdArgFloat,
        CmdArgString
    };

    // These constructors are for objects of this class
    avtSimulationCommandSpecification();
    avtSimulationCommandSpecification(const avtSimulationCommandSpecification &obj);
protected:
    // These constructors are for objects derived from this class
    avtSimulationCommandSpecification(private_tmfs_t tmfs);
    avtSimulationCommandSpecification(const avtSimulationCommandSpecification &obj, private_tmfs_t tmfs);
public:
    virtual ~avtSimulationCommandSpecification();

    virtual avtSimulationCommandSpecification& operator = (const avtSimulationCommandSpecification &obj);
    virtual bool operator == (const avtSimulationCommandSpecification &obj) const;
    virtual bool operator != (const avtSimulationCommandSpecification &obj) const;
private:
    void Init();
    void Copy(const avtSimulationCommandSpecification &obj);
public:

    virtual const std::string TypeName() const;
    virtual bool CopyAttributes(const AttributeGroup *);
    virtual AttributeSubject *CreateCompatible(const std::string &) const;
    virtual AttributeSubject *NewInstance(bool) const;

    // Property selection methods
    virtual void SelectAll();
    void SelectName();
    void SelectClassName();
    void SelectParent();
    void SelectSignal();
    void SelectText();
    void SelectUiType();
    void SelectValue();

    // Property setting methods
    void SetName(const std::string &name_);
    void SetArgumentType(CommandArgumentType argumentType_);
    void SetClassName(const std::string &className_);
    void SetEnabled(bool enabled_);
    void SetParent(const std::string &parent_);
    void SetIsOn(bool isOn_);
    void SetSignal(const std::string &signal_);
    void SetText(const std::string &text_);
    void SetUiType(const std::string &uiType_);
    void SetValue(const std::string &value_);

    // Property getting methods
    const std::string &GetName() const;
          std::string &GetName();
    CommandArgumentType GetArgumentType() const;
    const std::string &GetClassName() const;
          std::string &GetClassName();
    bool              GetEnabled() const;
    const std::string &GetParent() const;
          std::string &GetParent();
    bool              GetIsOn() const;
    const std::string &GetSignal() const;
          std::string &GetSignal();
    const std::string &GetText() const;
          std::string &GetText();
    const std::string &GetUiType() const;
          std::string &GetUiType();
    const std::string &GetValue() const;
          std::string &GetValue();

    // Enum conversion functions
    static std::string RunMode_ToString(RunMode);
    static bool RunMode_FromString(const std::string &, RunMode &);
protected:
    static std::string RunMode_ToString(int);
public:
    static std::string CommandArgumentType_ToString(CommandArgumentType);
    static bool CommandArgumentType_FromString(const std::string &, CommandArgumentType &);
protected:
    static std::string CommandArgumentType_ToString(int);
public:

    // IDs that can be used to identify fields in case statements
    enum {
        ID_name = 0,
        ID_argumentType,
        ID_className,
        ID_enabled,
        ID_parent,
        ID_isOn,
        ID_signal,
        ID_text,
        ID_uiType,
        ID_value,
        ID__LAST
    };

private:
    std::string name;
    int         argumentType;
    std::string className;
    bool        enabled;
    std::string parent;
    bool        isOn;
    std::string signal;
    std::string text;
    std::string uiType;
    std::string value;

    // Static class format string for type map.
    static const char *TypeMapFormatString;
    static const private_tmfs_t TmfsStruct;
};
#define AVTSIMULATIONCOMMANDSPECIFICATION_TMFS "sisbsbssss"

#endif
