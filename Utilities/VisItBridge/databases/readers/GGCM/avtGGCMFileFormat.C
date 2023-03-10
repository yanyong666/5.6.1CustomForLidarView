/** avtGGCMFileFormat.C
 * Reads GGCM ("3df") files into VisIt */
#include <avtGGCMFileFormat.h>

#include <cassert>
#include <string>
#include <cstring>
#include <libggcm.h>
#include <ctype.h>
#include <vector>

/** VTK includes */
/**@{*/
#include <vtkFloatArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkPointData.h>
/**@}*/

/**@{*/
#include <avtDatabaseMetaData.h>
#include <avtSpatialBoxSelection.h>
#include <DBOptionsAttributes.h>
#include <DebugStream.h>
#include <Expression.h>
#include <InvalidDBTypeException.h>
#include <InvalidVariableException.h>
/**@}*/

/** takes a vector name ("B", "V", etc.) and derives the field names that
 * are used by the 3df file.  The list is NULL terminated. */
static void VectorNames(const char *vector, char *flds[3]);
static void DeriveGridFile(const char *tdf, char **grid);
static char *lcase(char *str);
static std::vector<std::string> DeriveVectors(const MHDdata *);
static std::vector<std::string> DeriveScalars(const MHDdata *);
static void *xmalloc(size_t size);

/** Method: avtGGCM constructor
 *  Programmer: tfogal -- generated by xml2avt
 *  Creation:   Thu Jun 1 13:38:54 PST 2006 */
avtGGCMFileFormat::avtGGCMFileFormat(
        const char *filename,
        DBOptionsAttributes *db)
                :
        avtMTSDFileFormat(&filename,1),
        fn_grid(NULL),
        selsApplied(NULL),
        restricted(false)
{
    dim[0] = dim[1] = dim[2] = 0;
    min[0] = min[1] = min[2] = 0.0;
    max[0] = max[1] = max[2] = 0.0;
    assert(filename != NULL);
    DeriveGridFile(filename, &this->fn_grid);
    if(this->fn_grid == NULL)
    {
        EXCEPTION1(InvalidDBTypeException, "Could not derive grid filename.");
    }
    this->selList.clear();
}

/** The second argument to DeriveGridFile is filled with the dynamically
 * allocated memory.
 * We define a destructor and do not simply use FreeUpResources because a grid
 * file is static for a set of timesteps. */
avtGGCMFileFormat::~avtGGCMFileFormat()
{
    /* 2nd arg to DeriveGridFile is filled w/ dynamically allocated memory */
    free(this->fn_grid);
}

/** Method: avtGGCMFileFormat::GetNTimesteps
 * Tells the rest of the code how many timesteps there are in this file.
 * Programmer: tfogal -- generated by xml2avt
 * Creation:   Thu Jun 1 13:38:54 PST 2006 */
int
avtGGCMFileFormat::GetNTimesteps(void)
{
    return 1;
}


/** Method: avtGGCMFileFormat::FreeUpResources
 * When VisIt is done focusing on a particular timestep, it asks that
 * timestep to free up any resources (memory, file descriptors) that
 * it has associated with it.  This method is the mechanism for doing
 * that.
 * Programmer: tfogal -- generated by xml2avt
 * Creation:   Thu Jun 1 13:38:54 PST 2006 */
void
avtGGCMFileFormat::FreeUpResources(void)
{
    /* every pointer we've allocated has been given to other portions of VisIt,
     * which should be responsible for it. */
}


// **************************************************************************** 
//  Method: avtGGCMFileFormat::PopulateDatabaseMetaData
//
//  This database meta-data object is like a table of contents for the
//  file.  By populating it, you are telling the rest of VisIt what
//  information it can request from you.
//
//  Programmer: tfogal -- generated by xml2avt
//  Creation:   Thu Jun 1 13:38:54 PST 2006 
//
//  Modifications:
//    Kathleen Bonnell, Wed Sep  6 14:15:31 PDT 2006
//    Changed topological_dimension from 2 to 3.
//
// **************************************************************************** 

void
avtGGCMFileFormat::PopulateDatabaseMetaData(avtDatabaseMetaData *md,
                                            int timeState)
{
    std::string meshname = "GGCM mesh";
    avtMeshType mt = AVT_RECTILINEAR_MESH;
    int nblocks = 1; /* must be 1 since this is an MTSD type dataset */
    int block_origin = 0;
    int spatial_dimension = 3;
    int topological_dimension = 3;
    double *extents = NULL;

    /** tell the MD object that we have a mesh */
    AddMeshToMetaData(md, meshname, mt, extents, nblocks, block_origin,
                      spatial_dimension, topological_dimension);


    avtCentering cent = AVT_NODECENT; /* node centered */
    int vector_dim = 3; /* 3 dimensional field */

    /* for large files, reading the metadata can actually take some time, for
     * technical reasons I won't expound.  On my system (7200rpm drives),
     * reading the metadata for a gigabyte file takes ~ 5 sec. */
    MHDdata *metadata = ggcm_read_metadata(this->GetFilename());

    std::vector<std::string> vectors;
    vectors = DeriveVectors(metadata);
    std::vector<std::string>::const_iterator iter;

    debug5 << vectors.size() << " vectors found in " << this->GetFilename()
           << std::endl;

    for(iter = vectors.begin(); iter != vectors.end(); iter++) {
        AddVectorVarToMetaData(md, (*iter).c_str(), meshname, cent, vector_dim);
    }

    std::vector<std::string> scalars;
    scalars = DeriveScalars(metadata);

    debug5 << scalars.size() << " scalars found in " << this->GetFilename()
           << std::endl;

    for(iter = scalars.begin(); iter != scalars.end(); iter++) {
        AddScalarVarToMetaData(md, (*iter).c_str(), meshname, cent);
    }
}


/** Method: avtGGCMFileFormat::GetMesh
 * Gets the mesh associated with this file.  The mesh is returned as a
 * derived type of vtkDataSet (ie vtkRectilinearGrid, vtkStructuredGrid,
 * vtkUnstructuredGrid, etc).
 * @param timestate The index of the timestate.  If GetNTimesteps returned
 *                  'N' time steps, this is guaranteed to be between 0 and N-1.
 * @param meshname  The name of the mesh of interest.  This can be ignored if
 *                  there is only one mesh.
 * Programmer: tfogal -- generated by xml2avt
 * Creation:   Thu Jun 1 13:38:54 PST 2006
 *
 * Modifications:
 *   Jeremy Meredith, Thu Aug  7 16:11:10 EDT 2008
 *   Assume ggcm grid won't modify our string literals, so cast to char*s.
 *
 *   Mark C. Miller, Wed Aug 22 15:30:09 PDT 2012
 *   Fixed leak of grid_data.
 */
vtkDataSet *
avtGGCMFileFormat::GetMesh(int timestate, const char *meshname)
{
    MHDdata *grid_data;
    float *grid[3], *gse[3];
    /* which fields to read in */
    const char *glist[] = {"gridx","gridy","gridz", NULL};

    /* should we only read a subset of the domain? */
    this->SetupDomain();

    /* do the read */
    grid_data = ggcm_read_mhd(this->fn_grid, glist);
    grid[0] = ggcm_grid(grid_data, (int*) &this->dim[0], (char*)"gridx");
    grid[1] = ggcm_grid(grid_data, (int*) &this->dim[1], (char*)"gridy");
    grid[2] = ggcm_grid(grid_data, (int*) &this->dim[2], (char*)"gridz");

    /* data must actually be three dimensional */
    assert(this->dim[0] > 0 && this->dim[1] > 0 && this->dim[2] > 0);

    /* convert the data to a GSE coordinate system */
    gse[0] = (float*) xmalloc(sizeof(float) * this->dim[0]);
    gse[1] = (float*) xmalloc(sizeof(float) * this->dim[1]);
    gse[2] = (float*) xmalloc(sizeof(float) * this->dim[2]);

    ggcm_grid_gse(grid[0], grid[1], grid[2],
                  gse[0],  gse[1],  gse[2],
                  this->dim[0], this->dim[1], this->dim[2]);
    /* don't need data in the old coordinate system anymore */
    free(grid[0]);
    free(grid[1]);
    free(grid[2]);
    ggcm_free_data(grid_data);

    /* put the data in a form VTK can use */
    vtkFloatArray *rgrid[3];
    rgrid[0] = vtkFloatArray::New();
    rgrid[1] = vtkFloatArray::New();
    rgrid[2] = vtkFloatArray::New();
    rgrid[0]->SetNumberOfComponents(1);
    rgrid[1]->SetNumberOfComponents(1);
    rgrid[2]->SetNumberOfComponents(1);
    rgrid[0]->SetNumberOfValues(this->dim[0]);
    rgrid[1]->SetNumberOfValues(this->dim[1]);
    rgrid[2]->SetNumberOfValues(this->dim[2]);
    rgrid[0]->SetArray(gse[0], this->dim[0], 0);
    rgrid[1]->SetArray(gse[1], this->dim[1], 0);
    rgrid[2]->SetArray(gse[2], this->dim[2], 0);
    /* for good measure */
    rgrid[0]->Squeeze();
    rgrid[1]->Squeeze();
    rgrid[2]->Squeeze();

    vtkRectilinearGrid *RGrid = vtkRectilinearGrid::New();
    RGrid->SetDimensions((int*)this->dim);
    RGrid->SetXCoordinates(rgrid[0]);
    RGrid->SetYCoordinates(rgrid[1]);
    RGrid->SetZCoordinates(rgrid[2]);

    return RGrid;
}

/** Method: avtGGCMFileFormat::GetVar
 * Gets a scalar variable associated with this file.  Although VTK has
 * support for many different types, the best bet is vtkFloatArray, since
 * that is supported everywhere through VisIt.
 * @param timestate The index of the timestate.  If GetNTimesteps returned
 *                  'N' time steps, this is guaranteed to be between 0 and N-1.
 * @param varname   The name of the variable requested.
 * Programmer: tfogal -- generated by xml2avt
 * Mark C. Miller, Wed Aug 22, 2012: Fix leak of gse_scalar */
vtkDataArray *
avtGGCMFileFormat::GetVar(int timestate, const char *varname)
{
    /* C `libggcm' implementation */
    char *field[2];
    field[0] = lcase(strdup(varname));
    field[1] = NULL;

    debug1 << "avtGGCMFileFormat::GetVar(" << timestate << ", " << field[0]
           << ")" << std::endl;

    MHDdata *data;
    data = ggcm_read_mhd(this->GetFilename(), (const char **)field);

    debug1 << "\t... data read in" << std::endl;

    float ***scalar;
    scalar = ggcm_field_matrix(data, this->dim[0], this->dim[1], this->dim[2],
                               const_cast<char*>(field[0]));

    ggcm_free_data(data);

    float ***gse_scalar;
    gse_scalar = (float***) m_alloc(this->dim[0], this->dim[1], this->dim[2]);
    ggcm_mhd_gse(scalar, gse_scalar, this->dim[0], this->dim[1], this->dim[2],
                 const_cast<char*>(field[0]));
    m_free(scalar);

    free(field[0]);

    debug1 << "\t... converted to GSE coordinates" << std::endl;

    unsigned int i,j,k, idx=0;
    vtkFloatArray *var = vtkFloatArray::New();

    var->SetName(varname); /* will VisIt do this for me?  can't hurt... */
    var->SetNumberOfComponents(1);
    var->SetNumberOfValues(this->dim[0] * this->dim[1] * this->dim[2]);

    for(k=0; k < dim[2]; ++k) {
        for(j=0; j < dim[1]; ++j) {
            for(i=0; i < dim[0]; ++i) {
                var->InsertTuple(idx++, &gse_scalar[i][j][k]);
         }
      }
   }
    debug1 << "\t... " << idx << " values in VTK array" << std::endl;
   m_free(gse_scalar);

    return var;
}

/** Method: avtGGCMFileFormat::GetVectorVar
 * Gets a vector variable associated with this file.  Although VTK has
 * support for many different types, the best bet is vtkFloatArray, since
 * that is supported everywhere through VisIt.
 * @param timestate The index of the timestate.  If GetNTimesteps returned
 *                  'N' time steps, this is guaranteed to be between 0 and N-1.
 * @param varname   The name of the variable requested.
 * Programmer: tfogal -- generated by xml2avt
 * Creation:   Thu Jun 1 13:38:54 PST 2006 */
vtkDataArray *
avtGGCMFileFormat::GetVectorVar(int timestate, const char *varname)
{
    char *fields[4];
    VectorNames(varname, fields);

    fields[3] = NULL;
    debug4 << "avtGGCMFileFormat::GetVectorVar(" << timestate << ", {"
           << fields[0] << ", " << fields[1] << ", " << fields[2] << "})"
           << std::endl;


    MHDdata *data;
    data = ggcm_read_mhd(this->GetFilename(), (const char**)fields);

    float ***vector[3];
    vector[0] = ggcm_field_matrix(data, dim[0],dim[1],dim[2], fields[0]);
    vector[1] = ggcm_field_matrix(data, dim[0],dim[1],dim[2], fields[1]);
    vector[2] = ggcm_field_matrix(data, dim[0],dim[1],dim[2], fields[2]);

    ggcm_free_data(data);

    float ***vec_gse[3];
    /* X */
    vec_gse[0] = (float***) m_alloc(dim[0], dim[1], dim[2]);
    ggcm_mhd_gse(vector[0], vec_gse[0], dim[0],dim[1],dim[2], fields[0]);
    m_free(vector[0]);
    /* Y */
    vec_gse[1] = (float***) m_alloc(dim[0], dim[1], dim[2]);
    ggcm_mhd_gse(vector[1], vec_gse[1], dim[0],dim[1],dim[2], fields[1]);
    m_free(vector[1]);
    /* Z */
    vec_gse[2] = (float***) m_alloc(dim[0], dim[1], dim[2]);
    ggcm_mhd_gse(vector[2], vec_gse[2], dim[0],dim[1],dim[2], fields[2]);
    m_free(vector[2]);

    free(fields[0]);
    free(fields[1]);
    free(fields[2]);

    unsigned int i,j,k, idx=0;
    vtkFloatArray *var = vtkFloatArray::New();
    var->SetName(varname); /* will VisIt do this for me?  can't hurt... */

    var->SetNumberOfComponents(3);
    var->SetNumberOfTuples(this->dim[0] * this->dim[1] * this->dim[2]);

    float v[3];
    for(k=0; k < dim[2]; ++k) {
        for(j=0; j < dim[1]; ++j) {
            for(i=0; i < dim[0]; ++i) {
                v[0] = vec_gse[0][i][j][k];
                v[1] = vec_gse[1][i][j][k];
                v[2] = vec_gse[2][i][j][k];
                var->InsertTuple(idx++, v);
            }
        }
    }
    return var;
}

/** how VisIt gives subset contract info, to be used later */
void avtGGCMFileFormat::RegisterDataSelections(
                        const std::vector<avtDataSelection_p> &sels,
                        std::vector<bool> *selectionsApplied)
{
    unsigned int i;

    this->selList = sels;
    this->selsApplied = selectionsApplied;
    debug4 << "RegisterDataSelections(";
    if(sels.size() > 0) {
        for(i=0; i < sels.size()-1; ++i) {
            debug4 << sels[i]->GetType() << ", " << std::flush;
        }
        debug4 << sels[i]->GetType() << ")" << std::endl;
    } else {
        debug4 << sels.size() << " selections)" << std::endl;
    }
}

/** sets restriction based on contract information */
void avtGGCMFileFormat::SetupDomain()
{
    unsigned int i;
    this->restricted = false;
    for(i=0; i < selList.size(); ++i) {
        debug5 << "selection type: " << std::string(selList[i]->GetType())
                  << std::endl;

        avtSpatialBoxSelection *sel =
                     dynamic_cast<avtSpatialBoxSelection*>(*(selList[i]));
        sel->GetMins(this->min);
        sel->GetMaxs(this->max);
        this->restricted = true;

        debug2 << "restriction:"
               << '{' << min[0] << ',' << max[0] << "}, "
               << '{' << min[1] << ',' << max[1] << "}, "
               << '{' << min[2] << ',' << max[2] << "}" << std::endl;
        (*selsApplied)[i] = true;
    }
}

/** takes a vector name ("B", "V", etc.) and derives the field names that
 * are used by the 3df file.  The list is NULL terminated.
 * For example given the input "V" this should produce {vx, vy, vz, NULL}. */
static void
VectorNames(const char *vector, char *flds[3])
{
    assert(vector);
    size_t len, i;

    len = strlen(vector);

    flds[0] = (char*) xmalloc(len + 2);
    flds[1] = (char*) xmalloc(len + 2);
    flds[2] = (char*) xmalloc(len + 2);
    for(i=0; i < len; ++i) {
        int c;
        c = tolower(vector[i]);
        flds[0][i] = (char)c;
        flds[1][i] = (char)c;
        flds[2][i] = (char)c;
    }
    flds[0][len] = 'x'; flds[0][len+1] = '\0';
    flds[1][len] = 'y'; flds[1][len+1] = '\0';
    flds[2][len] = 'z'; flds[2][len+1] = '\0';

    debug5 << "VectorNames() = {"
           << flds[0] << ", " << flds[1] << ", " << flds[2] << "}" << std::endl;
}

static void DeriveGridFile(const char *tdf, char **grid)
{
    char *tmp;

    assert(tdf);

    *grid = strdup(tdf);
    tmp = strrchr(*grid, 'f');
    if(tmp) {
        tmp -= 2; /* back 2 chars to '3' in "3df" */
        tmp[0] = 'g';
        tmp[1] = 'r';
        tmp[2] = 'i';
        tmp[3] = 'd';
        tmp[4] = '\0';
    }
    debug4 << "grid filename is " << *grid << std::endl;
}

static char *lcase(char *str)
{
    assert(str);
    char *start = str;
    while(*str) {
        *str = (int) tolower(*str);
        str++;
    }
    return start;
}

/** given the head of a MHDdata list, this searches the field names and creates
 * a list of fields which are probably vectors.
 * The algorithm that does this is rather dumb.  We scan to see if the
 * field_names are only changing in the last letter, and assume if that last
 * letter goes through a "y", "z" sequence than that creates a vector.
 * @param md Head of the MHDdata list.  Only the metadata need be loaded. */
static std::vector<std::string> DeriveVectors(const MHDdata *md)
{
    std::vector<std::string> vectors;
    const MHDdata *cur;
    const char *prev = "";
    size_t p_len; /* previous length */
    size_t c_len; /* current length */
    int i;

    for(cur = md; cur; cur = cur->next) {
        /* can't be same data if fields are different lengths */
        p_len = strlen(prev);
        c_len = strlen(cur->field_name);
        if(p_len == c_len) {
            if(prev[p_len-1] == 'y' && cur->field_name[c_len-1] == 'z') {
                char *tmp;
                tmp = strdup(prev);
                tmp[p_len-1] = '\0'; /* get rid of the 'y' */
                for (i=0; i < (int)p_len-1; ++i) tmp[i] = toupper(tmp[i]);
                std::string vec(tmp);
                vectors.push_back(vec);
                free(tmp);
            }
        }
        prev = cur->field_name;
    }
    return vectors;
}

/** same as the above, this time for scalar fields.
 * This algorithm assumes any field which doesn't end in "x", "y", or "z" is a
 * scalar field.
 * @param md Head of the MHDdata list.  Only the metadata needs to be loaded */
static std::vector<std::string> DeriveScalars(const MHDdata *md)
{
    std::vector<std::string> scalars;
    const MHDdata *cur;
    const char *s;
    size_t len;
    int i;

    for(cur = md; cur; cur = cur->next) {
        s = cur->field_name;
        len = strlen(s);
        if(s[len-1] != 'x' && s[len-1] != 'y' && s[len-1] != 'z') {
            char *tmp;
            tmp = strdup(s);
            for (i = 0; i < (int)len; ++i) tmp[i] = toupper(tmp[i]);
            std::string scalar(tmp);
            scalars.push_back(scalar);
            free(tmp);
        }
    }
    return scalars;
}

static void *xmalloc(size_t sz)
{
    assert(sz != 0);
    void *block = malloc(sz);
#ifdef DEBUG
    if(block == NULL) {
        perror("out of memory"); 
        abort();
    }
#endif
    return block;
}
