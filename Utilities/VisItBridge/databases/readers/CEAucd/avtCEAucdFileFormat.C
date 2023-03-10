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
//                            avtCEAucdFileFormat.C                          //
// ************************************************************************* //

#include <avtCEAucdFileFormat.h>

#include <FichierU.h>
#include <vtkCEAucdReader.h>

#include <algorithm>
#include <snprintf.h>
#include <string>
#include <vector>

#include <FileFunctions.h>

#include <vtkCellData.h>
#include <vtkDemandDrivenPipeline.h>
#include <vtkExecutive.h>
#include <vtkFloatArray.h>
#include <vtkInformation.h>
#include <vtkPointData.h>
#include <vtkRectilinearGrid.h>
#include <vtkUnstructuredGrid.h>

#include <avtDatabaseMetaData.h>
#include <avtMaterial.h>

#include <DBOptionsAttributes.h>
#include <Expression.h>

#include <InvalidFilesException.h>
#include <InvalidVariableException.h>


using     std::string;
using     std::vector;


static void  CheckFileCallback(void *, const std::string &, bool, bool, long);


// ****************************************************************************
//  Method: avtCEAucdFileFormat constructor
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Tue Nov 27 09:05:11 PDT 2007
//
// ****************************************************************************

avtCEAucdFileFormat::avtCEAucdFileFormat(const char *filename)
    : avtSTMDFileFormat(&filename, 1)
{
    fileOpened = filename;
    readRoot   = false;
    setUpFiles = false;
    reader     = NULL;
    readInData = false;
    domainRead = -1;
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::GetCycle
//
//  Purpose:
//      Gets the cycle for this file.
//
//  Programmer: Hank Childs
//  Creation:   November 27, 2007
//
// ****************************************************************************

int
avtCEAucdFileFormat::GetCycle(void)
{
    if (!readRoot)
        ReadRootFile();

    return cycle;
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::GetTime
//
//  Purpose:
//      Gets the time for this file.
//
//  Programmer: Hank Childs
//  Creation:   November 27, 2007
//
// ****************************************************************************

double
avtCEAucdFileFormat::GetTime(void)
{
    if (!readRoot)
        ReadRootFile();

    return dtime;
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::ReadRootFile
//
//  Purpose:
//      Reads the root file using Thierry Carrard's FichierU class.
//
//  Programmer: Hank Childs
//  Creation:   November 27, 2007
//
// ****************************************************************************

void
avtCEAucdFileFormat::ReadRootFile(void)
{
    if (readRoot)
        return;

    if (!setUpFiles)
        SetUpFiles();

    if (root != "")
    {
        // For some reason, need to jump ahead 512 bytes for U_00000.
        // Ask Thierry.
        int amount = 0;
        if (strstr(root.c_str(), "U_00000") != NULL)
            amount = 512;
        FichierU fichieru(root, NULL, amount, 0);

        dtime     = fichieru.getTime();
        cycle     = fichieru.getNumCycle();
        dimension = fichieru.getDim();
        int nmat  = fichieru.getNbMat();
        for (int i = 0 ; i < nmat ; i++)
        {
            matnames.push_back(fichieru.getMatName(i));
        }
    }
    else
    {
        dtime = atof(stem.c_str());
        cycle = atoi(stem.c_str());
        dimension = 3;
    }

    readRoot = true;
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::SetUpFiles
//
//  Purpose:
//      Sets up files (stem, dir, root, domains).  Does this by looking at
//      fileOpened and by searching the directory.
//
//  Programmer: Hank Childs
//  Creation:   November 27, 2007
//
// ****************************************************************************

void
avtCEAucdFileFormat::SetUpFiles(void)
{
    if (setUpFiles)
        return;

    char slashChar = '/';
#if defined(_WIN32)
    slashChar = '\\';
#endif

    int last_slash = -1;
    while (fileOpened.find(slashChar, last_slash+1) != string::npos)
    {
        last_slash = (int)fileOpened.find(slashChar, last_slash+1);
    }

    dir = fileOpened.substr(0, last_slash);
    std::string filename = fileOpened.substr(last_slash+1);

    std::string extension = ".inp";
    char underscore = '_';
    int underscore_pos[2] = { 0, 0 };
    if (filename.find(underscore, 0) == string::npos)
    {
        EXCEPTION1(InvalidFilesException, fileOpened.c_str());
    }
    else
    {
        underscore_pos[0] = (int)filename.find(underscore, 0);
        if (filename.find(underscore, underscore_pos[0]+1) == string::npos)
        {
            // 1 underscore.  Either U_00000 (root file) or UCD_00000.inp
            // (1 file of a single-domain data set.)
            if (filename.find(extension, underscore_pos[0]+1) != string::npos)
            {
                string::size_type extension_pos = filename.find(extension, 
                                                  underscore_pos[0]+1);
                stem = filename.substr(underscore_pos[0]+1, 
                                       extension_pos-(underscore_pos[0]+1));
            }
            else
            {
                // This should be a root file of the form: U_XXXXX.
                stem = filename.substr(underscore_pos[0]+1);
            }
        }
        else
        {
            underscore_pos[1]=(int)filename.find(underscore, underscore_pos[0]+1);
            if (filename.find(underscore, underscore_pos[1]+1) != string::npos)
            {
                // Three underscores ... this blows the scheme ... time to
                // re-code.
                EXCEPTION1(InvalidFilesException, fileOpened.c_str());
            }
            stem = filename.substr(underscore_pos[0]+1, 
                                   underscore_pos[1] - (underscore_pos[0]+1));
        }
    }

    // Make sure the stem only contains numbers.
    for (size_t i = 0 ; i < stem.size() ; i++)
    {
        if (! isdigit(stem[i]))
        {
            EXCEPTION1(InvalidFilesException, fileOpened.c_str());
        }
    }
    
    FileFunctions::ReadAndProcessDirectory(dir, CheckFileCallback, this);

    setUpFiles = true;
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::AddFileInThisDirectory
//
//  Purpose:
//      This is called for each file in the directory.  This is called,
//      indirectly, by ReadAndProcessDirectory, which is a utility 
//      function that gets all of the files in a directory.
//
//  Arguments:
//      filenameWithDir  A filename that includes full directory qualification.
//
//  Programmer: Hank Childs
//  Creation:   November 27, 2007
//
// ****************************************************************************

void
avtCEAucdFileFormat::AddFileInThisDirectory(const std::string &filenameWithDir)
{
    char slashChar = '/';
#if defined(_WIN32)
    slashChar = '\\';
#endif

    int last_slash = -1;
    while (filenameWithDir.find(slashChar, last_slash+1) != string::npos)
    {
        last_slash = (int)filenameWithDir.find(slashChar, last_slash+1);
    }

    // Filename is filenameWithDir without the directory qualification.
    std::string filename = filenameWithDir.substr(last_slash+1);

    std::string rootCandidate = "U_" + stem;
    if (filename == rootCandidate)
        root = filenameWithDir;
    else
    {
        char underscore = '_';
        if (filename.find(underscore, 0) == string::npos)
            return; // can't be a match.
        size_t start = filename.find(underscore, 0)+1;
        size_t stop = string::npos;
        if (filename.find(underscore, start) == string::npos)
        {
            std::string extension = ".inp";
            if (filename.find(extension, start) == string::npos)
                return; // no second underscore, no extension.
            stop = filename.find(extension, start);
        }
        else
            stop = filename.find(underscore, start);
        std::string file_stem = filename.substr(start, stop-start);
        if (file_stem == stem)
            domains.push_back(filenameWithDir);
    }

    if (domains.size() > 1)
        std::sort(domains.begin(), domains.end());
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::FreeUpResources
//
//  Purpose:
//      When VisIt is done focusing on a particular timestep, it asks that
//      timestep to free up any resources (memory, file descriptors) that
//      it has associated with it.  This method is the mechanism for doing
//      that.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Tue Nov 27 09:05:11 PDT 2007
//
// ****************************************************************************

void
avtCEAucdFileFormat::FreeUpResources(void)
{
    // We're not caching anything at this time.
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::PopulateDatabaseMetaData
//
//  Purpose:
//      This database meta-data object is like a table of contents for the
//      file.  By populating it, you are telling the rest of VisIt what
//      information it can request from you.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Tue Nov 27 09:05:11 PDT 2007
//
// ****************************************************************************

void
avtCEAucdFileFormat::PopulateDatabaseMetaData(avtDatabaseMetaData *md)
{
    if (!readRoot)
        ReadRootFile();

    string meshname = "mesh";
    avtMeshType mt = AVT_UNSTRUCTURED_MESH;

    if (domains.size() == 0)
    {
        // Found no files with real data.
        EXCEPTION1(InvalidFilesException, fileOpened.c_str());
    }

    int nblocks = (int)domains.size();
    int block_origin = 0;
    int spatial_dimension = dimension;
    int topological_dimension = dimension;
    const double *extents = NULL;
    AddMeshToMetaData(md, meshname, mt, extents, nblocks, block_origin,
                      spatial_dimension, topological_dimension);

    vtkCEAucdReader *rdr = vtkCEAucdReader::New();
    vtkExecutive *e = rdr->GetExecutive();
    rdr->SetFileName(domains[0].c_str());
    vtkInformation *request = vtkInformation::New(); 
    request->Set(vtkDemandDrivenPipeline::REQUEST_INFORMATION());
    rdr->ProcessRequest(request, e->GetInputInformation(), 
                                 e->GetOutputInformation());

    for (int i = 0 ; i < rdr->GetNumberOfCellArrays() ; i++)
    {
        const char *name = rdr->GetCellArrayName(i);
        AddScalarVarToMetaData(md, name, meshname, AVT_ZONECENT);
        nodeCentered[name] = false;
    }
    for (int i = 0 ; i < rdr->GetNumberOfPointArrays() ; i++)
    {
        const char *name = rdr->GetPointArrayName(i);
        AddScalarVarToMetaData(md, name, meshname, AVT_NODECENT);
        nodeCentered[name] = true;
    }

    int nmats = (int)matnames.size();
    AddMaterialToMetaData(md, "materials", meshname, nmats, matnames);

    //
    // Set up expressions for vectors.
    //
    int numberOfCenterings = 2;
    for (int v = 0 ; v < numberOfCenterings ; v++)
    {
        vector<const char *> vnames;
        if (v == 0)
            for (int i = 0 ; i < rdr->GetNumberOfCellArrays() ; i++)
                vnames.push_back(rdr->GetCellArrayName(i));
        else
            for (int i = 0 ; i < rdr->GetNumberOfPointArrays() ; i++)
                vnames.push_back(rdr->GetPointArrayName(i));

        for (size_t i = 0 ; i < vnames.size() ; i++)
        {
            if (strstr(vnames[i], "[0]") != NULL)
            {
                if (strcmp(vnames[i], "frac_pres[0]") == 0)
                    continue;

                char iName[1024];
                strcpy(iName, vnames[i]);
                char *ptrI = strstr(iName, "[0]");

                char jName[1024];
                strcpy(jName, vnames[i]);
                char *ptrJ = strstr(jName, "[0]");
                ptrJ[1] = '1';
                bool foundJ = false;
    
                char kName[1024];
                strcpy(kName, vnames[i]);
                char *ptrK = strstr(kName, "[0]");
                ptrK[1] = '2';
                bool foundK = (dimension == 2 ? true : false);

                for (size_t j = 0 ; j < vnames.size() ; j++)
                {
                    if (strcmp(vnames[j], jName) == 0)
                        foundJ = true;
                    if (strcmp(vnames[j], kName) == 0)
                        foundK = true;
                    if (foundJ && foundK)
                        break;
                }
     
                if (foundJ && foundK)
                {
                    // VisIt doesn't like [0] and will turn it to {0}. 
                    // We should do the same.
                    ptrI[0] = '_';
                    ptrI[2] = '_';
                    ptrJ[0] = '_';
                    ptrJ[2] = '_';
                    ptrK[0] = '_';
                    ptrK[2] = '_';

                    Expression vec_expr;
                    char vec_name[1024];
                    int lengthBeforeIndex = ptrJ-jName;
                    strncpy(vec_name, jName, lengthBeforeIndex);
                    strcpy(vec_name + lengthBeforeIndex, 
                           jName + lengthBeforeIndex + strlen("{1}"));
                    vec_expr.SetName(vec_name);
                    char defn[1024];
                    if (dimension == 2)
                        SNPRINTF(defn, 1024, "{%s, %s}", iName, jName);
                    else
                        SNPRINTF(defn, 1024, "{%s, %s, %s}", 
                                       iName, jName, kName);
                    vec_expr.SetDefinition(defn);
                    vec_expr.SetType(Expression::VectorMeshVar);
                    md->AddExpression(&vec_expr);
                }
            }
        }
    }

    rdr->Delete();
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::GetMesh
//
//  Purpose:
//      Gets the mesh associated with this file.  The mesh is returned as a
//      derived type of vtkDataSet (ie vtkRectilinearGrid, vtkStructuredGrid,
//      vtkUnstructuredGrid, etc).
//
//  Arguments:
//      domain      The index of the domain.  If there are NDomains, this
//                  value is guaranteed to be between 0 and NDomains-1,
//                  regardless of block origin.
//      meshname    The name of the mesh of interest.  This can be ignored if
//                  there is only one mesh.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Tue Nov 27 09:05:11 PDT 2007
//
// ****************************************************************************

vtkDataSet *
avtCEAucdFileFormat::GetMesh(int domain, const char *meshname)
{
    if (!readInData || domain != domainRead)
        ReadInData(domain);

    vtkDataSet *mesh = reader->GetOutput();
    mesh->Register(NULL);
    return mesh;
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::GetVar
//
//  Purpose:
//      Gets a scalar variable associated with this file.  Although VTK has
//      support for many different types, the best bet is vtkFloatArray, since
//      that is supported everywhere through VisIt.
//
//  Arguments:
//      domain     The index of the domain.  If there are NDomains, this
//                 value is guaranteed to be between 0 and NDomains-1,
//                 regardless of block origin.
//      varname    The name of the variable requested.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Tue Nov 27 09:05:11 PDT 2007
//
// ****************************************************************************

vtkDataArray *
avtCEAucdFileFormat::GetVar(int domain, const char *varname)
{
    if (!readInData || domain != domainRead)
        ReadInData(domain);

    vtkDataSet *mesh = reader->GetOutput();
    vtkDataArray *arr = NULL;
    if (nodeCentered[varname])
        arr = mesh->GetPointData()->GetArray(varname);
    else
        arr = mesh->GetCellData()->GetArray(varname);
 
    if (arr == NULL)
    {
        EXCEPTION1(InvalidVariableException, varname);
    }

    arr->Register(NULL);
    return arr;
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::GetVectorVar
//
//  Purpose:
//      Gets a vector variable associated with this file.  Although VTK has
//      support for many different types, the best bet is vtkFloatArray, since
//      that is supported everywhere through VisIt.
//
//  Arguments:
//      domain     The index of the domain.  If there are NDomains, this
//                 value is guaranteed to be between 0 and NDomains-1,
//                 regardless of block origin.
//      varname    The name of the variable requested.
//
//  Programmer: childs -- generated by xml2avt
//  Creation:   Tue Nov 27 09:05:11 PDT 2007
//
// ****************************************************************************

vtkDataArray *
avtCEAucdFileFormat::GetVectorVar(int domain, const char *varname)
{
    EXCEPTION1(InvalidVariableException, varname);
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::RegisterVariableList
//
//  Purpose:
//      Receives a list of the variables to process.
//
//  Programmer: Hank Childs
//  Creation:   March 10, 2008
//
// ****************************************************************************

void
avtCEAucdFileFormat::RegisterVariableList(const char *primaryVariable,
                                  const std::vector<CharStrRef> &secondaryVars)
{
    if (!readRoot)
       ReadRootFile();

    variables.clear();
    variables.push_back(primaryVariable);
    for (size_t i = 0 ; i < secondaryVars.size() ; i++)
        variables.push_back(*(secondaryVars[i]));

    if (reader != NULL)
    {
        reader->Delete();
        reader = NULL;
    }
    
    readInData = false;
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::ReadInData
//
//  Purpose:
//      Reads in the data.  This is done before GetMesh or GetVar is called.
//
//  Programmer: Hank Childs
//  Creation:   March 10, 2008
//
// ****************************************************************************

void
avtCEAucdFileFormat::ReadInData(int domain)
{
    if (reader != NULL)
        reader->Delete();

    reader = vtkCEAucdReader::New();
    reader->SetFileName(domains[domain].c_str());
/*
 * I THINK THIS CODE TELLS VISIT ONLY TO READ CERTAIN ARRAYS, BUT IT DOESN'T
 * SEEM TO BE PLAYING WELL WITH THE VTK READER.
    reader->DisableAllCellArrays();
    reader->DisableAllPointArrays();
    for (int i = 0 ; i < variables.size() ; i++)
    {
        if (nodeCentered[variables[i]])
            reader->SetPointArrayStatus(variables[i].c_str(), 1);
        else
            reader->SetCellArrayStatus(variables[i].c_str(), 1);
    }
 */
    reader->Update();

    readInData = true;
    domainRead = domain;
}


// ****************************************************************************
//  Method: avtCEAucdFileFormat::GetAuxiliaryData
//
//  Purpose:
//      Gets the auxiliary data specified.
//
//  Arguments:
//      var        The variable of interest.
//      dom        The domain of interest.
//      type       The type of auxiliary data.
//      <unnamed>  The arguments for that type -- not used.
//      df         Destructor function.
//
//  Returns:    The auxiliary data.
//
//  Programmer: Hank Childs
//  Creation:   March 14, 2008
//
// ****************************************************************************


void *
avtCEAucdFileFormat::GetAuxiliaryData(const char *var, int dom,
                                        const char * type, void *,
                                        DestructorFunction &df)
{
    int   i;

    if (strcmp(type, AUXILIARY_DATA_MATERIAL) != 0)
        return NULL;

    int nMaterials = (int)matnames.size();
    if (nMaterials == 0)
        return NULL;

    std::vector<float *> mats(nMaterials);
    int nCells = 0;
    for (i = 0 ; i < nMaterials ; i++)
    {
        char name[1024];
        SNPRINTF(name, 1024, "frac_pres[%d]", i);
        vtkFloatArray *arr = (vtkFloatArray *) 
                            reader->GetOutput()->GetCellData()->GetArray(name);
        if (arr == NULL)
        {
            EXCEPTION0(ImproperUseException);
        }
        mats[i] = arr->GetPointer(0);
        nCells = arr->GetNumberOfTuples();
    }

    vector<int> material_list(nCells);
    vector<int> mix_mat;
    vector<int> mix_next;
    vector<int> mix_zone;
    vector<float> mix_vf;

    // Build the appropriate data structures
    for (i = 0; i < nCells; ++i)
    {
        int j;

        // First look for pure materials
        int nmats = 0;
        int lastMat = -1;
        for (j = 0; j < nMaterials; ++j)
        {
            if (mats[j][i] > 0)
            {
                nmats++;
                lastMat = j;
            }
        }

        if (nmats == 1)
        {
            material_list[i] = lastMat;
            continue;
        }

        // For unpure materials, we need to add entries to the tables.
        material_list[i] = -1 * (1 + (int)mix_zone.size());
        for (j = 0; j < nMaterials; ++j)
        {
            if (mats[j][i] <= 0)
                continue;
            // For each material that's present, add to the tables
            mix_zone.push_back(i);
            mix_mat.push_back(j);
            mix_vf.push_back(mats[j][i]);
            mix_next.push_back((int)mix_zone.size() + 1);
        }

        // When we're done, the last entry is a '0' in the mix_next
        mix_next[mix_next.size() - 1] = 0;
    }

    int mixed_size = (int)mix_zone.size();
    avtMaterial * mat = new avtMaterial(nMaterials, matnames, nCells,
                                        &(material_list[0]), mixed_size,
                                        &(mix_mat[0]), &(mix_next[0]),
                                        &(mix_zone[0]), &(mix_vf[0]));

    df = avtMaterial::Destruct;
    return (void*) mat;
}


// ****************************************************************************
//  Function: CheckFileCallback
//
//  Purpose:
//      This function is a callback to the method ReadAndProcessDirectory,
//      located in Utility.h.  It is called for each file in a given directory.
//      Once it receives a file, it feeds that file to the avtCEAucdFileFormat,
//      which then determines if it is part of the current time slice.
//
//  Programmer: Hank Childs
//  Creation:   November 27, 2007
//
// ****************************************************************************

static void
CheckFileCallback(void *args, const std::string &fname, bool isDir, 
                  bool perms, long filesize)
{
    avtCEAucdFileFormat *ff = (avtCEAucdFileFormat *) args;
    ff->AddFileInThisDirectory(fname);
}

