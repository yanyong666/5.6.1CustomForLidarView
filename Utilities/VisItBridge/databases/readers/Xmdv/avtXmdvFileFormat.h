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
//                             avtXmdvFileFormat.h                           //
// ************************************************************************* //

#ifndef AVT_Xmdv_FILE_FORMAT_H
#define AVT_Xmdv_FILE_FORMAT_H

#include <avtSTSDFileFormat.h>


class DBOptionsAttributes;


// ****************************************************************************
//  Class: avtXmdvFileFormat
//
//  Purpose:
//      Reads in Xmdv files as a plugin to VisIt.  Xmdv is a tools that
//      uses parallel coordinates to allow users to infer relations between
//      quantities.  The tool has a homegrown format that uses the extension
//      ".okc" to store parallel coordinates data.  This reader is useful
//      in VisIt for two reasons.  (1) It allows us to test "array" data that
//      comes directly from a file (as opposed to an expression).  (2) Since
//      the format is so simple, OKC files are a good way to export data from
//      VisIt into other tools.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Tue Jul 19 17:12:44 PST 2005
//
// ****************************************************************************

class avtXmdvFileFormat : public avtSTSDFileFormat
{
  public:
                       avtXmdvFileFormat(const char *filename, 
                                         DBOptionsAttributes *);
    virtual           ~avtXmdvFileFormat() {;};

    virtual const char    *GetType(void)   { return "Xmdv"; };
    virtual void           FreeUpResources(void); 

    virtual vtkDataSet    *GetMesh(const char *);
    virtual vtkDataArray  *GetVar(const char *);
    virtual vtkDataArray  *GetVectorVar(const char *);

  protected:
    bool                      readInData;
    int                       ncells;
    std::vector<float>        values;
    std::vector<std::string>  varnames;

    virtual void           PopulateDatabaseMetaData(avtDatabaseMetaData *);
    bool                   ReadXmdvFile(bool);
};


#endif
