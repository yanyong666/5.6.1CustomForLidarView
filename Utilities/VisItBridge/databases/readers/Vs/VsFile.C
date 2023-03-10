/*
 * VsFile.cpp
 *
 *  Created on: Apr 27, 2010
 *      Author: mdurant
 */

#include <vtk_hdf5.h>
#include <visit-hdf5.h>

#include "VsFile.h"
#include "VsLog.h"
#include "VsDataset.h"
#include "VsGroup.h"
#include "VsVariable.h"
#include "VsVariableWithMesh.h"
#include "VsRectilinearMesh.h"
#include "VsUniformMesh.h"
#include "VsUnstructuredMesh.h"
#include "VsStructuredMesh.h"

VsFile::VsFile(VsRegistry* r, std::string name, hid_t id):
  VsObject(r, NULL, name, id) {
  fileName = name;
  VsLog::debugLog() <<"File name is: " <<name <<std::endl;
  
// Code to determine dump number from file name
//  int underscoreLocation = fileName.find_last_of('_') + 1;
//  VsLog::debugLog() <<"dump number starts at position: " <<underscoreLocation <<std::endl;
//  if (underscoreLocation != -1) {
//    int periodLocation = fileName.find_first_of('.', underscoreLocation);
//    VsLog::debugLog() <<"period location is: " <<periodLocation <<std::endl;
//    if (periodLocation == -1) {
//      periodLocation = fileName.length() - 1;
//    }
//    VsLog::debugLog() <<"period location is: " <<periodLocation <<std::endl;
//        
//    std::string dumpNumberString = name.substr(underscoreLocation, (periodLocation - underscoreLocation));
//    VsLog::errorLog() <<"DumpNumberString is: " <<dumpNumberString <<std::endl;
//    dumpNumber = atoi(dumpNumberString.c_str());
//    VsLog::errorLog() <<"Dump number is: " <<dumpNumber <<std::endl;
//  }
  
// Quiet HDF5 stderr output
  H5Eset_auto(H5E_DEFAULT , 0, 0);
  
  
}

VsFile::~VsFile() {

// We clear all objects that were read from this file
// so that we can close the file itself.
// Note that this does not delete the registry object
// which will be deleted in ~avtVsFileFormat
  registry->deleteAllObjects();
  
// Check if anything is still open
  int cnt = (int)H5Fget_obj_count(getId(), H5F_OBJ_ALL);
// CNT will always be >= 1 because the FILE is still open
// So only output the error message if there is MORE than 1 open id
  if (cnt > 1) {
    VsLog::debugLog() << "File still has " << cnt << " open objects:" <<std::endl;
    std::vector<hid_t> objIds(cnt);
    int numObjs = (int)H5Fget_obj_ids(getId(), H5F_OBJ_ALL, cnt, &objIds[0]);
    char objName[1024];
    for (int i = 0; i < numObjs; ++i) {
      hid_t anobj = objIds[i];
      //Don't print a warning for the file itself, we know it's still open.
      if (anobj != getId()) {
        H5I_type_t ot = H5Iget_type(anobj);
        int status = (int)H5Iget_name(anobj, objName, 1024); (void) status;
        VsLog::debugLog() << "type = " << ot << ", name = " << objName <<", id = " <<anobj <<std::endl;;
      }
    }
    VsLog::debugLog() << std::endl;
  }

// Close this file
  H5Fclose(getId());
}

std::string VsFile::getName() const {
  return "/";
}

void VsFile::write() const {
  VsLog::debugLog() <<"File: " <<fileName <<std::endl;  
}

