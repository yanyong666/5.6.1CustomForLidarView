//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//============================================================================

#ifndef vtk_m_filter_CellAverage_h
#define vtk_m_filter_CellAverage_h

#include <vtkm/filter/FilterCell.h>
#include <vtkm/worklet/CellAverage.h>

namespace vtkm
{
namespace filter
{
/// \brief  Point to cell interpolation filter.
///
/// CellAverage is a filter that transforms point data (i.e., data
/// specified at cell points) into cell data (i.e., data specified per cell).
/// The method of transformation is based on averaging the data
/// values of all points used by particular cell.
///
class CellAverage : public vtkm::filter::FilterCell<CellAverage>
{
public:
  VTKM_CONT
  CellAverage();

  template <typename T, typename StorageType, typename DerivedPolicy>
  VTKM_CONT vtkm::cont::DataSet DoExecute(const vtkm::cont::DataSet& input,
                                          const vtkm::cont::ArrayHandle<T, StorageType>& field,
                                          const vtkm::filter::FieldMetadata& fieldMeta,
                                          const vtkm::filter::PolicyBase<DerivedPolicy>& policy);

private:
  vtkm::worklet::CellAverage Worklet;
};
}
} // namespace vtkm::filter

#include <vtkm/filter/CellAverage.hxx>

#endif // vtk_m_filter_CellAverage_h
