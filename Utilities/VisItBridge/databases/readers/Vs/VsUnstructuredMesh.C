/*
 * VsUnstructuredMesh.C
 *
 *  Created on: Apr 29, 2010
 *      Author: mdurant
 */

#include "VsUnstructuredMesh.h"
#include "VsSchema.h"
#include "VsDataset.h"
#include "VsUtils.h"
#include "VsGroup.h"
#include "VsLog.h"

#define __CLASS__ "VsUnstructuredMesh::"

VsUnstructuredMesh::VsUnstructuredMesh(VsGroup* group):VsMesh(group) {
  numPoints = 0;
  numCells = 0;
  splitPoints = false;
}

VsUnstructuredMesh::~VsUnstructuredMesh() {
}

/**
 * Need to search through all possible cell types and figure out which one
 * is being used.  Currently assumes only one cell type is being used.
 */
VsDataset* VsUnstructuredMesh::getCellInfo()
{

  VsDataset* connectivityMeta = 0;

  // For now users can have only one connectivity dataset.
  connectivityMeta = getLinesDataset();
  if(connectivityMeta!=NULL) return connectivityMeta;

  connectivityMeta = getPolygonsDataset();
  if(connectivityMeta!=NULL) return connectivityMeta;

  connectivityMeta = getTrianglesDataset();
  if(connectivityMeta!=NULL) return connectivityMeta;

  connectivityMeta = getQuadrilateralsDataset();
  if(connectivityMeta!=NULL) return connectivityMeta;

  connectivityMeta = getPolyhedraDataset();
  if(connectivityMeta!=NULL) return connectivityMeta;

  connectivityMeta = getTetrahedralsDataset();
  if(connectivityMeta!=NULL) return connectivityMeta;

  connectivityMeta = getPyramidsDataset();
  if(connectivityMeta!=NULL) return connectivityMeta;

  connectivityMeta = getPrismsDataset();
  if(connectivityMeta!=NULL) return connectivityMeta;

  connectivityMeta = getHexahedralsDataset();
  return connectivityMeta;
}

//Tweak for Nautilus
std::string VsUnstructuredMesh::getNodeCorrectionDatasetName() const {
  return makeCanonicalName(getFullName(), "localToGlobalNodeMapping");
}

bool VsUnstructuredMesh::hasNodeCorrectionData() const {
  std::string datasetName = getNodeCorrectionDatasetName();

  //Look for a node correction list
  VsDataset* nodeCorrectionDataset = registry->getDataset(datasetName);

  return (nodeCorrectionDataset != NULL);
}
//end tweak

size_t VsUnstructuredMesh::getNumPoints() const {
  return numPoints;
}

size_t VsUnstructuredMesh::getNumCells() const {
  return numCells;
}

bool VsUnstructuredMesh::usesSplitPoints() const {
  return splitPoints;
}

std::string VsUnstructuredMesh::getPointsDatasetName() const {
  //First see if the user has specified a name for the dataset
  std::string pointsName;
  getStringAttribute(VsSchema::Unstructured::vsPoints, &pointsName);
  if (!pointsName.empty()) {
    return makeCanonicalName(getFullName(), pointsName);
  }

  //if we didn't find vsPoints, try the default name
  return makeCanonicalName(getFullName(),
                           VsSchema::Unstructured::defaultPointsName);
}

std::string VsUnstructuredMesh::getPointsDatasetName(int i) const {
  
  std::string attributeName;
  switch (i) {
    case 0: attributeName = VsSchema::Unstructured::vsPoints0;
      break;
    case 1: attributeName = VsSchema::Unstructured::vsPoints1;
      break;
    case 2: attributeName = VsSchema::Unstructured::vsPoints2;
      break;
    default:

      VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "  
                        << "Requested index (" << i << ") is out of range."
                        << std::endl;
      return "";
  }
  
  std::string fullName;
  getStringAttribute(attributeName, &fullName); //might be empty
  //if the name was found, make sure it's fully qualified
  if (!fullName.empty()) {
    fullName = makeCanonicalName(getFullName(), fullName);
  }

  return fullName;
}

hid_t VsUnstructuredMesh::getDataType() const {
  VsDataset* pointsDataset = getPointsDataset();
  if (pointsDataset != NULL) {
    return pointsDataset->getType();
  }

  VsDataset* points0Dataset = getPointsDataset(0);
  if (points0Dataset != NULL) {
    return points0Dataset->getType();
  }
  
  return H5T_NATIVE_FLOAT;
}

VsDataset* VsUnstructuredMesh::getPointsDataset(int i) const {
  std::string pointsName = getPointsDatasetName(i);
  if (pointsName.empty()) {
    return NULL;
  }

  VsDataset* answer = registry->getDataset(pointsName);

  return answer; //could be NULL
}

VsDataset* VsUnstructuredMesh::getPointsDataset() const  {
  std::string pointsName = getPointsDatasetName();
  if (pointsName.empty()) {
    return NULL;
  }

  VsDataset* answer = registry->getDataset(pointsName);

  return answer; //could be NULL
}

std::string VsUnstructuredMesh::getPolygonsDatasetName() const {
  //First see if the user has specified a name for the dataset
  std::string polygonsName;
  getStringAttribute(VsSchema::Unstructured::vsPolygons, &polygonsName);
  if (!polygonsName.empty()) {
    return makeCanonicalName(getFullName(), polygonsName);
  }

  return makeCanonicalName(getFullName(), VsSchema::Unstructured::defaultPolygonsName);
}

VsDataset* VsUnstructuredMesh::getPolygonsDataset() const {
  std::string polygonsName = getPolygonsDatasetName();
  if (polygonsName.empty()) {
    return NULL;
  }

  return registry->getDataset(polygonsName);
}

std::string VsUnstructuredMesh::getPolyhedraDatasetName() const  {
  //First see if the user has specified a name for the dataset
  std::string polyhedraName;
  getStringAttribute(VsSchema::Unstructured::vsPolyhedra, &polyhedraName);
  if (!polyhedraName.empty()) {
    return makeCanonicalName(getFullName(), polyhedraName);
  }

  return makeCanonicalName(getFullName(), VsSchema::Unstructured::defaultPolyhedraName);
}

VsDataset* VsUnstructuredMesh::getPolyhedraDataset() const {
  std::string polyhedraName = getPolyhedraDatasetName();
  if (polyhedraName.empty()) {
    return NULL;
  }

  return registry->getDataset(polyhedraName);
}

std::string VsUnstructuredMesh::getLinesDatasetName() const  {
  //First see if the user has specified a name for the dataset
  std::string linesName;
  getStringAttribute(VsSchema::Unstructured::vsLines, &linesName);
  if (!linesName.empty()) {
    return makeCanonicalName(getFullName(), linesName);
  }
  
  return makeCanonicalName(getFullName(), VsSchema::Unstructured::defaultLinesName);
}

VsDataset* VsUnstructuredMesh::getLinesDataset() const  {
  std::string linesName = getLinesDatasetName();
  if (linesName.empty()) {
    return NULL;
  }

  return registry->getDataset(linesName);
}

std::string VsUnstructuredMesh::getTrianglesDatasetName() const {
  //First see if the user has specified a name for the dataset
  std::string trianglesName;
  getStringAttribute(VsSchema::Unstructured::vsTriangles, &trianglesName);
  if (!trianglesName.empty()) {
    return makeCanonicalName(getFullName(), trianglesName);
  }

  return makeCanonicalName(getFullName(), VsSchema::Unstructured::defaultTrianglesName);
}

VsDataset* VsUnstructuredMesh::getTrianglesDataset() const {
  std::string trianglesName = getTrianglesDatasetName();
  if (trianglesName.empty()) {
    return NULL;
  }

  return registry->getDataset(trianglesName);
}

std::string VsUnstructuredMesh::getQuadrilateralsDatasetName() const {
  //First see if the user has specified a name for the dataset
  std::string quadrilateralsName;
  getStringAttribute(VsSchema::Unstructured::vsQuadrilaterals, &quadrilateralsName);
  if (!quadrilateralsName.empty()) {
    return makeCanonicalName(getFullName(), quadrilateralsName);
  }

  return makeCanonicalName(getFullName(), VsSchema::Unstructured::defaultQuadrilateralsName);
}

VsDataset* VsUnstructuredMesh::getQuadrilateralsDataset() const {
  std::string quadrilateralsName = getQuadrilateralsDatasetName();
  if (quadrilateralsName.empty()) {
    return NULL;
  }

  return registry->getDataset(quadrilateralsName);
}

std::string VsUnstructuredMesh::getTetrahedralsDatasetName() const {
  //First see if the user has specified a name for the dataset
  std::string tetrahedralsName;
  getStringAttribute(VsSchema::Unstructured::vsTetrahedrals, &tetrahedralsName);
  if (!tetrahedralsName.empty()) {
    return makeCanonicalName(getFullName(), tetrahedralsName);
  }

  return makeCanonicalName(getFullName(), VsSchema::Unstructured::defaultTetrahedralsName);
}

VsDataset* VsUnstructuredMesh::getTetrahedralsDataset() const {
  std::string tetrahedralsName = getTetrahedralsDatasetName();
  if (tetrahedralsName.empty()) {
    return NULL;
  }

  return registry->getDataset(tetrahedralsName);
}

std::string VsUnstructuredMesh::getPyramidsDatasetName() const  {
  //First see if the user has specified a name for the dataset
  std::string pyramidsName;
  getStringAttribute(VsSchema::Unstructured::vsPyramids, &pyramidsName);
  if (!pyramidsName.empty()) {
    return makeCanonicalName(getFullName(), pyramidsName);
  }

  return makeCanonicalName(getFullName(), VsSchema::Unstructured::defaultPyramidsName);
}

VsDataset* VsUnstructuredMesh::getPyramidsDataset() const {
  std::string pyramidsName = getPyramidsDatasetName();
  if (pyramidsName.empty()) {
    return NULL;
  }

  return registry->getDataset(pyramidsName);
}

std::string VsUnstructuredMesh::getPrismsDatasetName() const {
  //First see if the user has specified a name for the dataset
  std::string prismsName;
  getStringAttribute(VsSchema::Unstructured::vsPrisms, &prismsName);
  if (!prismsName.empty()) {
    return makeCanonicalName(getFullName(), prismsName);
  }

  return makeCanonicalName(getFullName(), VsSchema::Unstructured::defaultPrismsName);
}

VsDataset* VsUnstructuredMesh::getPrismsDataset() const {
  std::string prismsName = getPrismsDatasetName();
  if (prismsName.empty()) {
    return NULL;
  }

  return registry->getDataset(prismsName);
}

std::string VsUnstructuredMesh::getHexahedralsDatasetName() const  {
  //First see if the user has specified a name for the dataset
  std::string hexahedralsName;
  getStringAttribute(VsSchema::Unstructured::vsHexahedrals, &hexahedralsName);
  if (!hexahedralsName.empty()) {
    return makeCanonicalName(getFullName(), hexahedralsName);
  }

  return makeCanonicalName(getFullName(), VsSchema::Unstructured::defaultHexahedralsName);
}

VsDataset* VsUnstructuredMesh::getHexahedralsDataset() const {
  std::string hexahedralsName = getHexahedralsDatasetName();
  if (hexahedralsName.empty()) {
    return NULL;
  }

  return registry->getDataset(hexahedralsName);
}

VsUnstructuredMesh* VsUnstructuredMesh::buildUnstructuredMesh(VsGroup* group) {
  VsUnstructuredMesh* newMesh = new VsUnstructuredMesh(group);
  bool success = newMesh->initialize();
  
  if (success) {
    VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "  
                      << "Returning success." << std::endl;
    return newMesh;
  }

  delete (newMesh);
  newMesh = NULL;
    VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "  
                      << "Returning failure." << std::endl;
  return NULL;
}


bool VsUnstructuredMesh::initialize() {

  VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "Entering" <<std::endl;
  
  VsDataset* pointsDataset = getPointsDataset();

  if (pointsDataset != NULL) {
    splitPoints = false;

    if( isCompMinor() )
    {
      numPoints = pointsDataset->getDims()[0];
      numSpatialDims = pointsDataset->getDims()[1];
    }
    else
    {
      numSpatialDims = pointsDataset->getDims()[0];
      numPoints = pointsDataset->getDims()[1];
    }
  }
  else {
    splitPoints = true;

    // It's possible to have multiple points datasets
    VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "  
                      << "Path = " << getPath() << "  "
                      << "vsPoints0 = " << getPointsDatasetName(0) << "  "
                      << "vsPoints1 = " << getPointsDatasetName(1) << "  "
                      << "vsPoints2 = " << getPointsDatasetName(2) << std::endl;
    
    if (!getPointsDataset(0)) {
      VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "  
                        << "Unable to load points data set 0.  Returning false."
                        << std::endl;
      return false;
    }
    
    if (!getPointsDataset(1) && getPointsDataset(2)) {
      VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "  
                        << "Unable to load points data set 1.  Returning false."
                        << std::endl;
      return false;
    }
    
    // The spatial dimension is the number of point datasets.
    numSpatialDims = 0;

    for( int i=0; i<3; ++i ) {
      if( getPointsDataset(i) )
        numSpatialDims = i;
      else
        break;
    }

    // Make sure all point dataset have the same number of coordinates.
    numPoints = getPointsDataset(0)->getDims()[0];

    for( int i=1; i<numSpatialDims; ++i ) {
      if( numPoints != getPointsDataset(0)->getDims()[0] ) {
        VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "  
                          << "Number of coordinates in point dataset " << i
                          << "  does not match the number of point expected "
                          << numPoints << ".  Returning false."
                          << std::endl;
        return false;
      }
    }
  }

  // If no connectivity assume a point mesh.
  if( isPointMesh() )
  {
    numCells = numPoints;
    numTopologicalDims = 0;
  }
  else
  {
    // For now users can have only one connectivity dataset.
    VsDataset* connectivityMeta = 0;
    std::string connectivityDatasetName;

    if( (connectivityMeta = getLinesDataset())) {
      connectivityDatasetName = getLinesDatasetName();
      numTopologicalDims = 1;
    } else if( (connectivityMeta = getPolygonsDataset()) ) {
      connectivityDatasetName = getPolygonsDatasetName();
      numTopologicalDims = 2;
    } else if( (connectivityMeta = getTrianglesDataset()) ) {
      connectivityDatasetName = getTrianglesDatasetName();
      numTopologicalDims = 2;
    } else if( (connectivityMeta = getQuadrilateralsDataset()) ) {
      connectivityDatasetName = getQuadrilateralsDatasetName();
      numTopologicalDims = 2;
    } else if( (connectivityMeta = getPolyhedraDataset()) ) {
      connectivityDatasetName = getPolyhedraDatasetName();
      numTopologicalDims = 3;
    } else if( (connectivityMeta = getTetrahedralsDataset()) ) {
      connectivityDatasetName = getTetrahedralsDatasetName();
      numTopologicalDims = 3;
    } else if( (connectivityMeta = getPyramidsDataset()) ) {
      connectivityDatasetName = getPyramidsDatasetName();
      numTopologicalDims = 3;
    } else if( (connectivityMeta = getPrismsDataset()) ) {
      connectivityDatasetName = getPrismsDatasetName();
      numTopologicalDims = 3;
    } else if( (connectivityMeta = getHexahedralsDataset()) ){
      connectivityDatasetName = getHexahedralsDatasetName();
      numTopologicalDims = 3;
    }

    VsDataset* connectivityDataset =
      registry->getDataset(connectivityDatasetName); (void) connectivityDataset;
    
    std::vector<int> connectivityDims = connectivityMeta->getDims();
    
    if( isCompMinor() )
    {
      numCells = connectivityDims[0];
    }
    else
    {
      numCells = connectivityDims[1];
    }
  }

  VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "Unstructured Mesh " << getShortName() << " "
                    << "has num spatial dims = "
                    << numSpatialDims << " and "
                    << "has num topological dims = "
                    << numTopologicalDims << std::endl;

  return initializeRoot();
}

bool VsUnstructuredMesh::isPointMesh() const {
  return ((getPolygonsDataset() == NULL) &&
          (getPolyhedraDataset() == NULL) &&
    
          (getLinesDataset() == NULL) &&
          (getTrianglesDataset() == NULL) &&
          (getQuadrilateralsDataset() == NULL) &&
          
          (getTetrahedralsDataset() == NULL) &&
          (getPyramidsDataset() == NULL) &&
          (getPrismsDataset() == NULL) &&
          (getHexahedralsDataset() == NULL));
}

std::string VsUnstructuredMesh::getKind() const {
  return VsSchema::Unstructured::key;
}

void VsUnstructuredMesh::getCellDims(std::vector<int>& dims) const
{
  VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "entering" << std::endl;
  
  dims.resize(1);
  dims[0] = (int) numCells;

  VsLog::debugLog() << __CLASS__ << __FUNCTION__ << "  " << __LINE__ << "  "
                    << "exiting" << std::endl;  
}
