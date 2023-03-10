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
//                             avtXmdvWriter.h                               //
// ************************************************************************* //

#ifndef AVT_Xmdv_WRITER_H
#define AVT_Xmdv_WRITER_H

#include <avtDatabaseWriter.h>

#include <visitstream.h>

#include <string>
#include <vector>

class DBOptionsAttributes;


// ****************************************************************************
//  Class: avtXmdvWriter
//
//  Purpose:
//      A module that writes out Xmdv files.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Tue Jul 19 17:12:44 PST 2005
//
//  Modifications:
//
//    Hank Childs, Tue Sep 27 10:21:36 PDT 2005
//    Use virtual inheritance.
//
//    Jeremy Meredith, Tue Mar 27 15:10:21 EDT 2007
//    Added nblocks to OpenFile and save it so we don't have to 
//    trust the meta data.
//
//    Kathleen Bonnell, Mon Apr 16 11:38:34 PDT 2007 
//    Add varCentering.
//
// ****************************************************************************

class
avtXmdvWriter : virtual public avtDatabaseWriter
{
  public:
                   avtXmdvWriter(DBOptionsAttributes *);
    virtual       ~avtXmdvWriter() {;};

  protected:
    std::string    stem;
    bool           onlyOneBlock;
    std::vector<std::string> scalars;
    std::vector<std::string> vectors;
    bool           varsAreNodal;
    avtCentering   varCentering;
    bool           writeOutCoordinates;
    int            nblocks;

    virtual void   OpenFile(const std::string &, int);
    virtual void   WriteHeaders(const avtDatabaseMetaData *,
                                const std::vector<std::string> &, 
                                const std::vector<std::string> &,
                                const std::vector<std::string> &);
    virtual void   WriteChunk(vtkDataSet *, int);
    virtual void   CloseFile(void);
    virtual void   WriteRootFile();
};


#endif
