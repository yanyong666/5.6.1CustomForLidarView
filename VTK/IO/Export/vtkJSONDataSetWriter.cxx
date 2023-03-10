/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkJSONDataSetWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkJSONDataSetWriter.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"

#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtksys/MD5.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTypeInt32Array.h"
#include "vtkTypeInt64Array.h"
#include "vtkTypeUInt32Array.h"
#include "vtkTypeUInt64Array.h"

#include <fstream>
#include <sstream>
#include <string>

vtkStandardNewMacro(vtkJSONDataSetWriter);

// ----------------------------------------------------------------------------

vtkJSONDataSetWriter::vtkJSONDataSetWriter()
{
  this->FileName = nullptr;
  this->ValidStringCount = 1;
}

// ----------------------------------------------------------------------------

vtkJSONDataSetWriter::~vtkJSONDataSetWriter()
{
  delete[] this->FileName;
}

// ----------------------------------------------------------------------------

vtkDataSet* vtkJSONDataSetWriter::GetInput()
{
  return vtkDataSet::SafeDownCast(this->Superclass::GetInput());
}

// ----------------------------------------------------------------------------

vtkDataSet* vtkJSONDataSetWriter::GetInput(int port)
{
  return vtkDataSet::SafeDownCast(this->Superclass::GetInput(port));
}

// ----------------------------------------------------------------------------

std::string vtkJSONDataSetWriter::WriteDataSetAttributes(
  vtkDataSetAttributes* fields, const char* className)
{
  vtkIdType activeTCoords = -1;
  vtkIdType activeScalars = -1;
  vtkIdType activeNormals = -1;
  vtkIdType activeGlobalIds = -1;
  vtkIdType activeTensors = -1;
  vtkIdType activePedigreeIds = -1;
  vtkIdType activeVectors = -1;

  vtkIdType nbFields = fields->GetNumberOfArrays();

  if (nbFields == 0)
  {
    return "";
  }

  std::stringstream jsonSnippet;
  jsonSnippet << "  \"" << className << "\": {"
              << "\n    \"vtkClass\": \"vtkDataSetAttributes\","
              << "\n    \"arrays\": [\n";
  for (vtkIdType idx = 0; idx < nbFields; idx++)
  {
    if (idx)
    {
      jsonSnippet << ",\n";
    }
    vtkDataArray* field = fields->GetArray(idx);
    jsonSnippet << "      { \"data\": " << this->WriteArray(field, "vtkDataArray") << "}";

    // Update active field if any
    activeTCoords = field == fields->GetTCoords() ? idx : activeTCoords;
    activeScalars = field == fields->GetScalars() ? idx : activeScalars;
    activeNormals = field == fields->GetNormals() ? idx : activeNormals;
    activeGlobalIds = field == fields->GetGlobalIds() ? idx : activeGlobalIds;
    activeTensors = field == fields->GetTensors() ? idx : activeTensors;
    activePedigreeIds = field == fields->GetPedigreeIds() ? idx : activePedigreeIds;
    activeVectors = field == fields->GetVectors() ? idx : activeVectors;
  }
  jsonSnippet << "\n    ],\n"
              << "    \"activeTCoords\": " << activeTCoords << ",\n"
              << "    \"activeScalars\": " << activeScalars << ",\n"
              << "    \"activeNormals\": " << activeNormals << ",\n"
              << "    \"activeGlobalIds\": " << activeGlobalIds << ",\n"
              << "    \"activeTensors\": " << activeTensors << ",\n"
              << "    \"activePedigreeIds\": " << activePedigreeIds << ",\n"
              << "    \"activeVectors\": " << activeVectors << "\n"
              << "  }";

  return jsonSnippet.str();
}

// ----------------------------------------------------------------------------

std::string vtkJSONDataSetWriter::WriteArray(
  vtkDataArray* array, const char* className, const char* arrayName)
{
  bool needConvert;
  std::string id = vtkJSONDataSetWriter::GetUID(array, needConvert);
  std::stringstream arrayPath;
  arrayPath << this->FileName << "/data/" << id.c_str();
  bool success = vtkJSONDataSetWriter::WriteArrayAsRAW(array, arrayPath.str().c_str());

  if (!success)
  {
    return "{}";
  }

  const char* INDENT = "    ";
  std::stringstream ss;
  ss << "{\n"
     << INDENT << "  \"vtkClass\": \"" << className << "\",\n"
     << INDENT << "  \"name\": \"" << this->GetValidString(arrayName == nullptr ? array->GetName() : arrayName) << "\",\n"
     << INDENT << "  \"numberOfComponents\": " << array->GetNumberOfComponents() << ",\n"
     << INDENT << "  \"dataType\": \"" << vtkJSONDataSetWriter::GetShortType(array, needConvert) << "Array\",\n"
     << INDENT << "  \"ref\": {\n"
     << INDENT << "     \"encode\": \"LittleEndian\",\n"
     << INDENT << "     \"basepath\": \"data\",\n"
     << INDENT << "     \"id\": \"" << id.c_str() << "\"\n"
     << INDENT << "  },\n"
     << INDENT << "  \"size\": " << array->GetNumberOfValues() << "\n"
     << INDENT << "}";

  return ss.str();
}

// ----------------------------------------------------------------------------

void vtkJSONDataSetWriter::WriteData()
{
  vtkDataSet* ds = this->GetInput();
  vtkImageData* imageData = vtkImageData::SafeDownCast(ds);
  vtkPolyData* polyData = vtkPolyData::SafeDownCast(ds);
  this->ValidDataSet = false;

  // Get input and check data
  if (ds == nullptr)
  {
    vtkErrorMacro(<< "No data to write!");
    return;
  }

  if (this->FileName == nullptr)
  {
    vtkErrorMacro(<< "Please specify FileName to write");
    return;
  }

  // Capture vtkDataSet definition
  std::stringstream metaJsonFile;
  metaJsonFile << "{\n";
  metaJsonFile << "  \"vtkClass\": \"" << ds->GetClassName() << "\"";

  // ImageData
  if (imageData)
  {
    this->ValidDataSet = true;

    // Spacing
    metaJsonFile << ",\n  \"spacing\": [" << imageData->GetSpacing()[0] << ", "
                 << imageData->GetSpacing()[1] << ", " << imageData->GetSpacing()[2] << "]";

    // Origin
    metaJsonFile << ",\n  \"origin\": [" << imageData->GetOrigin()[0] << ", "
                 << imageData->GetOrigin()[1] << ", " << imageData->GetOrigin()[2] << "]";

    // Extent
    metaJsonFile << ",\n  \"extent\": [" << imageData->GetExtent()[0] << ", "
                 << imageData->GetExtent()[1] << ", " << imageData->GetExtent()[2] << ", "
                 << imageData->GetExtent()[3] << ", " << imageData->GetExtent()[4] << ", "
                 << imageData->GetExtent()[5] << "]";
  }

  // PolyData
  if (polyData && polyData->GetPoints())
  {
    this->ValidDataSet = true;

    vtkPoints* points = polyData->GetPoints();
    metaJsonFile << ",\n  \"points\": "
                 << this->WriteArray(points->GetData(), "vtkPoints", "points").c_str();

    // Verts
    vtkDataArray* cells = polyData->GetVerts()->GetData();
    if (cells->GetNumberOfValues())
    {
      metaJsonFile << ",\n  \"verts\": "
                   << this->WriteArray(cells, "vtkCellArray", "verts").c_str();
    }

    // Lines
    cells = polyData->GetLines()->GetData();
    if (cells->GetNumberOfValues())
    {
      metaJsonFile << ",\n  \"lines\": "
                   << this->WriteArray(cells, "vtkCellArray", "lines").c_str();
    }

    // Strips
    cells = polyData->GetStrips()->GetData();
    if (cells->GetNumberOfValues())
    {
      metaJsonFile << ",\n  \"strips\": "
                   << this->WriteArray(cells, "vtkCellArray", "strips").c_str();
    }

    // Polys
    cells = polyData->GetPolys()->GetData();
    if (cells->GetNumberOfValues())
    {
      metaJsonFile << ",\n  \"polys\": "
                   << this->WriteArray(cells, "vtkCellArray", "polys").c_str();
    }
  }

  // PointData
  std::string fieldJSON = this->WriteDataSetAttributes(ds->GetPointData(), "pointData");
  if (!fieldJSON.empty())
  {
    metaJsonFile << ",\n" << fieldJSON.c_str();
  }

  // CellData
  fieldJSON = this->WriteDataSetAttributes(ds->GetCellData(), "cellData");
  if (!fieldJSON.empty())
  {
    metaJsonFile << ",\n" << fieldJSON.c_str();
  }

  metaJsonFile << "}\n";

  // Write meta-data file
  std::stringstream scenePath;
  scenePath << this->FileName << "/index.json";

  ofstream file;
  file.open(scenePath.str().c_str(), ios::out);
  file << metaJsonFile.str().c_str();
  file.close();
}

// ----------------------------------------------------------------------------

void vtkJSONDataSetWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

// ----------------------------------------------------------------------------

int vtkJSONDataSetWriter::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

// ----------------------------------------------------------------------------
// Static helper functions
// ----------------------------------------------------------------------------

void vtkJSONDataSetWriter::ComputeMD5(const unsigned char* content, int size, std::string& hash)
{
  unsigned char digest[16];
  char md5Hash[33];
  md5Hash[32] = '\0';

  vtksysMD5* md5 = vtksysMD5_New();
  vtksysMD5_Initialize(md5);
  vtksysMD5_Append(md5, content, size);
  vtksysMD5_Finalize(md5, digest);
  vtksysMD5_DigestToHex(digest, md5Hash);
  vtksysMD5_Delete(md5);

  hash = md5Hash;
}

// ----------------------------------------------------------------------------

std::string vtkJSONDataSetWriter::GetShortType(vtkDataArray* input, bool& needConversion)
{
  needConversion = false;
  std::stringstream ss;
  switch (input->GetDataType())
  {
    case VTK_UNSIGNED_CHAR:
    case VTK_UNSIGNED_SHORT:
    case VTK_UNSIGNED_INT:
    case VTK_UNSIGNED_LONG:
    case VTK_UNSIGNED_LONG_LONG:
      ss << "Uint";
      if (input->GetDataTypeSize() <= 4)
      {
        ss << (input->GetDataTypeSize() * 8);
      }
      else
      {
        needConversion = true;
        ss << "32";
      }

      break;

    case VTK_CHAR:
    case VTK_SIGNED_CHAR:
    case VTK_SHORT:
    case VTK_INT:
    case VTK_LONG:
    case VTK_LONG_LONG:
    case VTK_ID_TYPE:
      ss << "Int";
      if (input->GetDataTypeSize() <= 4)
      {
        ss << (input->GetDataTypeSize() * 8);
      }
      else
      {
        needConversion = true;
        ss << "32";
      }
      break;

    case VTK_FLOAT:
    case VTK_DOUBLE:
      ss << "Float";
      ss << (input->GetDataTypeSize() * 8);
      break;

    case VTK_BIT:
    case VTK_STRING:
    case VTK_UNICODE_STRING:
    case VTK_VARIANT:
    default:
      ss << "xxx";
      break;
  }

  return ss.str();
}

// ----------------------------------------------------------------------------

std::string vtkJSONDataSetWriter::GetUID(vtkDataArray* input, bool& needConversion)
{
  const unsigned char* content = (const unsigned char*)input->GetVoidPointer(0);
  int size = input->GetNumberOfValues() * input->GetDataTypeSize();
  std::string hash;
  vtkJSONDataSetWriter::ComputeMD5(content, size, hash);

  std::stringstream ss;
  ss << vtkJSONDataSetWriter::GetShortType(input, needConversion) << "_" << input->GetNumberOfValues()
     << "-" << hash.c_str();

  return ss.str();
}

// ----------------------------------------------------------------------------

std::string vtkJSONDataSetWriter::GetValidString(const char* name)
{
  if (name != nullptr && strlen(name))
  {
    return name;
  }
  std::stringstream ss;
  ss << "invalid_" << this->ValidStringCount++;

  return ss.str();
}

// ----------------------------------------------------------------------------

bool vtkJSONDataSetWriter::WriteArrayAsRAW(vtkDataArray* input, const char* filePath)
{
  if (input->GetDataTypeSize() == 0)
  {
    // Skip BIT arrays
    return false;
  }

  // Make sure parent directory exist
  vtksys::SystemTools::MakeDirectory(vtksys::SystemTools::GetParentDirectory(filePath));

  // Check if we need to convert the (u)int64 to (u)int32
  vtkSmartPointer<vtkDataArray> arrayToWrite = input;
  vtkIdType arraySize = input->GetNumberOfTuples() * input->GetNumberOfComponents();
  switch (input->GetDataType())
  {
    case VTK_UNSIGNED_CHAR:
    case VTK_UNSIGNED_LONG:
    case VTK_UNSIGNED_LONG_LONG:
      if (input->GetDataTypeSize() > 4)
      {
        vtkNew<vtkTypeUInt64Array> srcUInt64;
        srcUInt64->ShallowCopy(input);
        vtkNew<vtkTypeUInt32Array> uint32;
        uint32->SetNumberOfValues(arraySize);
        uint32->SetName(input->GetName());
        for (vtkIdType i = 0; i < arraySize; i++)
        {
          uint32->SetValue(i, srcUInt64->GetValue(i));
        }
        arrayToWrite = uint32;
      }
      break;
    case VTK_LONG:
    case VTK_LONG_LONG:
    case VTK_ID_TYPE:
      if (input->GetDataTypeSize() > 4)
      {
        vtkNew<vtkTypeInt64Array> srcInt64;
        srcInt64->ShallowCopy(input);
        vtkNew<vtkTypeInt32Array> int32;
        int32->SetNumberOfTuples(arraySize);
        int32->SetName(input->GetName());
        for (vtkIdType i = 0; i < arraySize; i++)
        {
          int32->SetValue(i, srcInt64->GetValue(i));
        }
        arrayToWrite = int32;
      }
      break;
  }

  const char* content = (const char*)arrayToWrite->GetVoidPointer(0);
  size_t size = arrayToWrite->GetNumberOfValues() * arrayToWrite->GetDataTypeSize();

  ofstream file;
  file.open(filePath, ios::out | ios::binary);
  file.write(content, size);
  file.close();

  return true;
}
