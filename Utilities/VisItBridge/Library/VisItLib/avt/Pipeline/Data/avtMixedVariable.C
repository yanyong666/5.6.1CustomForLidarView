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
//                           avtMixedVariable.C                              //
// ************************************************************************* //

#include <avtMixedVariable.h>

// For NULL
#include <stdio.h>

#include <avtMaterial.h>

using std::string;


// ****************************************************************************
//  Method: avtMixedVariable constructor
//
//  Arguments:
//      b       The mixed variable buffer
//      ml      The number of elements in b.
//      vn      The variable name with this mixed var.
//
//  Programmer: Hank Childs
//  Creation:   September 27, 2001
//
//  Modifications:
//
//    Jeremy Meredith, Tue Dec  4 13:44:28 PST 2001
//    Added const to constructor arguments.
//
//    Hank Childs, Thu Jul  4 13:02:54 PDT 2002
//    Add variable names.
//
// ****************************************************************************

avtMixedVariable::avtMixedVariable(const float *b, int ml, string vn)
{
    mixlen  = ml;
    varname = vn;
    buffer = new float[mixlen];
    for (int i = 0 ; i < mixlen ; i++)
    {
        buffer[i] = b[i];
    }
}


// ****************************************************************************
//  Method: avtMixedVariable destructor
//
//  Programmer: Hank Childs
//  Creation:   September 27, 2001
//
// ****************************************************************************

avtMixedVariable::~avtMixedVariable()
{
    if (buffer != NULL)
    {
        delete [] buffer;
        buffer = NULL;
    }
}


// ****************************************************************************
//  Method: avtMixedVariable::Destruct
//
//  Purpose:
//      Destructs a mixed variable.  This is for void-reference purposes.
//
//  Programmer: Hank Childs
//  Creation:   September 24, 2002
//
// ****************************************************************************

void
avtMixedVariable::Destruct(void *p)
{
    avtMixedVariable *mv = (avtMixedVariable *) p;
    if (mv != NULL)
    {
        delete mv;
    }
}


// ****************************************************************************
//  Method: avtMixedVariable::GetValuesForZone
//
//  Purpose:
//    Constructs the per material value list for the given zone. 
//
//  Mote: This will return 0 for all materials in the non-mixed case.
//
//  Programmer: Cyrus Harrison
//  Creation:   January 30, 2008
//
// ****************************************************************************

void
avtMixedVariable::GetValuesForZone(int zone_id,
                                   avtMaterial *mat,
                                   std::vector<float> &vals)
{
    int n_mats = mat->GetNMaterials();
    vals.clear();
    for (int m=0; m<n_mats; m++)
        vals.push_back(0.0);
        
    const int *mat_list = mat->GetMatlist();
    const int *mix_mat  = mat->GetMixMat();
    const int *mix_next = mat->GetMixNext();
    
    // mixed case
    if(mat_list[zone_id] < 0)
    {
        int mix_idx = -mat_list[zone_id] - 1;
        while(mix_idx >=0)
        {
            vals[mix_mat[mix_idx]] = buffer[mix_idx];
            mix_idx = mix_next[mix_idx] -1;
        }
    } 
}


