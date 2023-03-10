/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractExodusGlobalTemporalVariables.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkExtractExodusGlobalTemporalVariables
 * @brief extract global temporal arrays generated by vtkExodusIIReader
 *
 * vtkExtractExodusGlobalTemporalVariables is intended to extract global
 * temporal arrays generated by vtkExodusIIReader.
 *
 * Global temporal variables are special arrays that have values per timestep.
 * vtkExodusIIReader can read these values for all timestep and provide them
 * easily, thus avoiding the need to read all timesteps to extract these values
 * over timesteps. A slight complication is with ParaView, where ParaView
 * supports what are referred to as restarts i.e. instead of a single file
 * having all timesteps, the timesteps are arbitrarily split among multiple
 * files. For such cases, this filter needs to iterate over all the files to
 * accumulate the data. Since there's no information about files or ability to
 * request the upstream reader to read a particular file, this filter request
 * specific timesteps using some smarts to determine which timestep to request
 * and not simply loop over all timesteps.
 *
 * @sa vtkExodusIIReader, vtkExodusIIReader::GLOBAL_TEMPORAL_VARIABLE.
 */
#ifndef vtkExtractExodusGlobalTemporalVariables_h
#define vtkExtractExodusGlobalTemporalVariables_h

#include "vtkFiltersExtractionModule.h" // For export macro
#include "vtkTableAlgorithm.h"

#include <memory> // for std::unique_ptr

class VTKFILTERSEXTRACTION_EXPORT vtkExtractExodusGlobalTemporalVariables : public vtkTableAlgorithm
{
public:
  static vtkExtractExodusGlobalTemporalVariables* New();
  vtkTypeMacro(vtkExtractExodusGlobalTemporalVariables, vtkTableAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkExtractExodusGlobalTemporalVariables();
  ~vtkExtractExodusGlobalTemporalVariables() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;
  int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestUpdateExtent(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

  //@{
  /**
   * These methods are used by vtkPExtractExodusGlobalTemporalVariables to
   * synchronize internal state between ranks.
   */
  void GetContinuationState(bool& continue_executing_flag, size_t& offset) const;
  void SetContinuationState(bool continue_executing_flag, size_t offset);
  //@}

private:
  vtkExtractExodusGlobalTemporalVariables(const vtkExtractExodusGlobalTemporalVariables&) = delete;
  void operator=(const vtkExtractExodusGlobalTemporalVariables&) = delete;

  class vtkInternals;
  std::unique_ptr<vtkInternals> Internals;
};

#endif
