#include <vtk_hdf5.h>

/**
 *
 * @file        VsReader.cpp
 *
 * @brief       Implementation reader of data for VSH5 schema
 *
 * @version $Id: VsReader.cpp 178 2014-01-31 22:33:46Z loverich $
 *
 * Copyright &copy; 2007-2008, Tech-X Corporation
 * See LICENSE file for conditions of use.
 *
 */

#include <sstream>
#include <stdexcept>

#include <VsReader.h>
#include <VsSchema.h>
#include <VsUtils.h>
#include "VsDataset.h"
#include "VsAttribute.h"
#include "VsMDVariable.h"
#include "VsMDMesh.h"
#include "VsMesh.h"
#include "VsVariable.h"
#include "VsVariableWithMesh.h"
#include "VsRectilinearMesh.h"
#include "VsUnstructuredMesh.h"
#include "VsStructuredMesh.h"
#include "VsUniformMesh.h"
#include "VsLog.h"
#include "VsRegistry.h"
#include "VsFilter.h"

#define __CLASS__ "VsReader::"


int VsReader::numInstances = 0;

VsReader::VsReader(const std::string& nm, VsRegistry* r) {

  numInstances++;
  VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "This VsReader is #"
                    << numInstances << std::endl;

  VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "VsReader::VsReader(" << nm << ") entering." << std::endl;

  if (numInstances > 1) {
    VsLog::warningLog()
      << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
      << "Warning!  More than one concurrent copy of VsReader." << std::endl;
    VsLog::warningLog()
      << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
      << "Warning!  Debug messages may be interleaved." << std::endl;
  }

  registry = r;
  
  // Read raw hdf5 metadata
  fileData = VsFilter::readFile(registry, nm);
  if (!fileData) {
    VsLog::errorLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                      << "Unable to load metadata from file." << std::endl;
//SVETA: need to reimplement
    //EXCEPTION1(InvalidFilesException, nm.c_str());
  }

  //debugging output of raw hdf5 objects
  registry->writeAllGroups();
  registry->writeAllDatasets();
  
  //Build "Core" (Mesh and Var) objects
  registry->buildGroupObjects();
  registry->buildDatasetObjects();

  //Build MD objects
  registry->buildMDMeshes();
  registry->buildMDVars();

  //Register "transformed" meshes, and then variables
  registry->buildTransformedMeshes();
  registry->buildTransformedVariables();
  
  //debugging output
  registry->writeAllMeshes();
  registry->writeAllExpressions();
  registry->writeAllVariables();
  registry->writeAllVariablesWithMesh();
  registry->writeAllMDMeshes();
  registry->writeAllMDVariables();

  //Do a third pass to create components
  registry->createComponents();

  // Was this even a vizschema file?
  if ((registry->numMeshes() == 0) &&
      (registry->numVariables() == 0) &&
      (registry->numExpressions() == 0) &&
      (registry->numVariablesWithMesh() == 0) &&
      (registry->numMDMeshes() == 0) &&
      (registry->numMDVariables() == 0)) {
    VsLog::errorLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                      << "file format not recognized." << std::endl;
// SVETA: NEED TO REIMPL
    //EXCEPTION1(InvalidFilesException, nm.c_str());
  }
  
  VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "exiting." << std::endl;
}


VsReader::~VsReader() {

  VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "Entering." << std::endl;
  
  if (fileData) {
    delete (fileData);
    fileData = NULL;
  }
  
  VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "Exiting." << std::endl;
}


int VsReader::getData( VsDataset* dataSet,
                       void* data,
                       // Use these variables for adjusting
                       // the read memory space.
                       std::string indexOrder, // Index ordering
                       int components,   // Index for a component
                       int* srcMins,     // start locations
                       int* srcCounts,   // number of entries
                       int* srcStrides,  // stride in memory

                       // Use these variables for adjusting
                       // the write memory space.
                       int  mdims,               // spatial dims (rank)
                       int* destSizes,           // overall memory size
                       int* destMins,            // start locations
                       int* destCounts,          // number of entries
                       int* destStrides ) const  // stride in memory
{
  // components = -2 No component array (i.e. scalar variable)
  // components = -1 Component array present but read all values.
  //                 (i.e. vector variable or point coordinates)
  // component >= 0  Component array present read single value at that index.

  if (dataSet == NULL) {
    VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                      << "Requested dataset is null?" << std::endl;
    return -1;
  }
  
  if (data == NULL) {
    VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                      << "Data storage is null?" << std::endl;
    return -1;
  }
  
  int err = 0;

  // No index ordering info so read all data.
  if (indexOrder.length() == 0)
    {
      err = H5Dread(dataSet->getId(), dataSet->getType(),
                    H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

      if (err < 0) {
        VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                          << "Error " << err 
                          << " in reading variable " << dataSet->getFullName()
                          << "." << std::endl;
        return err;
      }
    }

  // mins, counts, and/or strides so hyperslab.
  else
    {
      hid_t dataspace = H5Dget_space(dataSet->getId());

      int ndims = H5Sget_simple_extent_ndims(dataspace);

      if (ndims == 0) {
        VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                          << "Unable to load dimensions for variable."
                          << "Returning -1." << std::endl;
        return -1;
      }
    
      std::vector<hsize_t> dims(ndims);

      int ndim = H5Sget_simple_extent_dims(dataspace, &(dims[0]), NULL);

      if( ndim != ndims ) {
        VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                          << "Data dimensions not match. " << std::endl;
        return -1;
      }

      VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                        << "about to set up arguments." << std::endl;

      std::vector<hsize_t> count(ndims);
      std::vector<hsize_t> start(ndims);
      std::vector<hsize_t> stride(ndims);

      // FORTRAN ordering.
      if((indexOrder == VsSchema::compMinorFKey) ||
         (indexOrder == VsSchema::compMajorFKey))
        {
          // No components - [iz][iy][ix]
          if( components == -2 )
            {
              for (unsigned int i = 0; i< (unsigned int)ndims; i++)
                {
                  // Flip the ordering of the input.
                  int base = ndims - 1;

                  start[1] = (hsize_t) srcMins[base-i];
                  count[1] = (hsize_t) srcCounts[base-i];
                  stride[1] = (hsize_t) srcStrides[base-i];
          
                  VsLog::debugLog()
                    << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "For i = " << i
                    << ", start = " << start[i]
                    << ", count = " << count[i]
                    << ", stride = " << stride[i]
                    << std::endl;
                }
            }
          else if(indexOrder == VsSchema::compMajorFKey)
            {
              // Multiple components - [ic][iz][iy][ix] compMajorF
              for (unsigned int i = 0; i< (unsigned int)ndims-1; i++)
                {
                  // Flip the ordering of the input.
                  int base = ndims - 1;

                  start[i+1] = (hsize_t) srcMins[base-i];
                  count[i+1] = (hsize_t) srcCounts[base-i];
                  stride[i+1] = (hsize_t) srcStrides[base-i];
          
                  VsLog::debugLog()
                    << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "For i = " << i
                    << ", start = " << start[i+1]
                    << ", count = " << count[i+1]
                    << ", stride = " << stride[i+1]
                    << std::endl;
                }

              // Components present but read all
              if( components < 0 )
                {
                  start[0] = 0;
                  count[0] = dims[ndims-1];
                  stride[0] = 1;
                }
              else // Components present but read a single value
                {
                  start[0] = components;
                  count[0] = 1;
                  stride[0] = 1;
                }
            }
          else //if( indexOrder == VsSchema::compMinorFKey )
            {
              // Multiple components - [iz][iy][ix][ic] compMinorF
              for (unsigned int i = 0; i< (unsigned int)ndims-1; i++)
                {
                  // Flip the ordering of the input.
                  int base = ndims - 1;

                  start[i] = (hsize_t) srcMins[base-i];
                  count[i] = (hsize_t) srcCounts[base-i];
                  stride[i] = (hsize_t) srcStrides[base-i];
          
                  VsLog::debugLog()
                    << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "For i = " << i
                    << ", start = " << start[i]
                    << ", count = " << count[i]
                    << ", stride = " << stride[i]
                    << std::endl;
                }

              // Components present but read all
              if( components < 0 )
                {
                  start[ndims-1] = 0;
                  count[ndims-1] = dims[ndims-1];
                  stride[ndims-1] = 1;
                }
              else // Components present but read a single value
                {
                  start[ndims-1] = components;
                  count[ndims-1] = 1;
                  stride[ndims-1] = 1;
                }
            }
        }
      // C ordering.
      else //if((indexOrder == VsSchema::compMinorCKey) ||
        //   (indexOrder == VsSchema::compMajorCKey))
        {
          if( components == -2 )
            {
              // No components - [ix][iy][iz]
              for (unsigned int i = 0; i< (unsigned int)ndims; i++)
                {
                  start[i] = (hsize_t) srcMins[i];
                  count[i] = (hsize_t) srcCounts[i];
                  stride[i] = (hsize_t) srcStrides[i];
          
                  VsLog::debugLog()
                    << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "For i = " << i
                    << ", start = " << start[i]
                    << ", count = " << count[i]
                    << ", stride = " << stride[i]
                    << std::endl;
                }
            }

          else if(indexOrder == VsSchema::compMajorCKey)
            {
              // Multiple components - [ic][ix][iy][iz] compMajorC
              for (unsigned int i = 0; i< (unsigned int)ndims-1; i++)
                {
                  start[i+1] = (hsize_t) srcMins[i];
                  count[i+1] = (hsize_t) srcCounts[i];
                  stride[i+1] = (hsize_t) srcStrides[i];
          
                  VsLog::debugLog()
                    << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "For i = " << i
                    << ", start = " << start[i+1]
                    << ", count = " << count[i+1]
                    << ", stride = " << stride[i+1]
                    << std::endl;
                }

              // Components present but read all
              if( components < 0 )
                {
                  start[0] = 0;
                  count[0] = dims[ndims-1];
                  stride[0] = 1;
                }
              else // Components present but read a single value
                {
                  start[0] = components;
                  count[0] = 1;
                  stride[0] = 1;
                }
            }

          else if(indexOrder == VsSchema::compMinorCKey)
            {
              // Multiple components - [ix][iy][iz][ic] compMinorC
              for (unsigned int i = 0; i< (unsigned int)ndims-1; i++)
                {
                  start[i] = (hsize_t) srcMins[i];
                  count[i] = (hsize_t) srcCounts[i];
                  stride[i] = (hsize_t) srcStrides[i];
          
                  VsLog::debugLog()
                    << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "For i = " << i
                    << ", start = " << start[i]
                    << ", count = " << count[i]
                    << ", stride = " << stride[i]
                    << std::endl;
                }

              // Components present but read all
              if( components < 0 )
                {
                  start[ndims-1] = 0;
                  count[ndims-1] = dims[ndims-1];
                  stride[ndims-1] = 1;
                }
              else // Components present but read a single value
                {
                  start[ndims-1] = components;
                  count[ndims-1] = 1;
                  stride[ndims-1] = 1;
                }
            }
        }

      // Select subset of the data
      if( srcMins )
        err = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET,
                                  &(start[0]), &(stride[0]), &(count[0]), NULL);

      VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                        << "After selecting the hyperslab, err is " << err
                        << std::endl;

      // Create memory space for the data
      hid_t memspace;

      std::vector<hsize_t> destSize(ndims);

      std::vector<hsize_t> destCount(ndims);
      std::vector<hsize_t> destStart(ndims);
      std::vector<hsize_t> destStride(ndims);

      if( mdims == 0 )
        memspace = H5Screate_simple(ndims, &(count[0]), NULL);
      else
        {
          for( int i=0; i<mdims; ++i )
            {
              destSize[i] = (hsize_t) destSizes[i];

              destStart[i] = (hsize_t) destMins[i];
              destCount[i] = (hsize_t) destCounts[i];
              destStride[i] = (hsize_t) destStrides[i];
            }

          memspace = H5Screate_simple(mdims, &(destSize[0]), NULL);

          H5Sselect_hyperslab(memspace, H5S_SELECT_SET,
                              &(destStart[0]), &(destStride[0]), &(destCount[0]),
                              NULL);
        }

      // Read data
      err = H5Dread(dataSet->getId(), dataSet->getType(), memspace, dataspace,
                    H5P_DEFAULT, data);

      if (err < 0) {
        VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                          << ": error " << err
                          << " in reading dataset." << std::endl;
      }

      err = H5Sclose(memspace);
      err = H5Sclose(dataspace);
    }

  
  VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "Returning " << err << "." << std::endl;
  return err;
}

