/*****************************************************************************
*
* Copyright (c) 2000 - 2018, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* All rights reserved.
*
* This file is part of VisIt. For details, see http://www.llnl.gov/visit/. The
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
*    documentation and/or materials provided with the distribution.
*  - Neither the name of the UC/LLNL nor  the names of its contributors may be
*    used to  endorse or  promote products derived from  this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED.  IN  NO  EVENT  SHALL  THE  REGENTS  OF  THE  UNIVERSITY OF
* CALIFORNIA, THE U.S.  DEPARTMENT  OF  ENERGY OR CONTRIBUTORS BE  LIABLE  FOR
* ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

// ************************************************************************* //
//                            avtCEAucdFileFormat.h                           //
// ************************************************************************* //

#ifndef AVT_CEAucd_FILE_FORMAT_H
#define AVT_CEAucd_FILE_FORMAT_H

#include <avtSTMDFileFormat.h>

#include <string>
#include <vector>
#include <map>

class     vtkCEAucdReader;


// ****************************************************************************
//  Class: avtCEAucdFileFormat
//
//  Purpose:
//      Reads in CEAucd files as a plugin to VisIt.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Tue Nov 27 09:05:11 PDT 2007
//
// ****************************************************************************

class avtCEAucdFileFormat : public avtSTMDFileFormat
{
  public:
                       avtCEAucdFileFormat(const char *);
    virtual           ~avtCEAucdFileFormat() {;};

    virtual void      *GetAuxiliaryData(const char *var, int domain,
                                        const char *type, void *args, 
                                        DestructorFunction &);

    void               AddFileInThisDirectory(const std::string &);
    virtual int        GetCycle(void);
    virtual double     GetTime(void);

    virtual const char    *GetType(void)   { return "CEAucd"; };
    virtual void           FreeUpResources(void); 

    virtual vtkDataSet    *GetMesh(int, const char *);
    virtual vtkDataArray  *GetVar(int, const char *);
    virtual vtkDataArray  *GetVectorVar(int, const char *);

  protected:
    bool                      readRoot;
    bool                      setUpFiles;

    vtkCEAucdReader          *reader;
    bool                      readInData;
    int                       domainRead;

    std::string               fileOpened;
    std::string               stem;
    std::string               dir;
    std::string               root;
    std::vector<std::string>  domains;
    std::vector<std::string>  variables;
    std::map<std::string, bool> nodeCentered;

    int                       cycle;
    double                    dtime;
    std::vector<std::string>  matnames;
    int                       dimension;

    void                      ReadRootFile(void);
    void                      SetUpFiles(void);
    virtual void              PopulateDatabaseMetaData(avtDatabaseMetaData *);
    virtual void              RegisterVariableList(const char *,
                                          const std::vector<CharStrRef> &);
    void                      ReadInData(int);
};


#endif


