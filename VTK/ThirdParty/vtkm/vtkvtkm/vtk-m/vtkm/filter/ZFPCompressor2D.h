//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//============================================================================

#ifndef vtk_m_filter_ZFPCompressor2D_h
#define vtk_m_filter_ZFPCompressor2D_h

#include <vtkm/filter/FilterField.h>
#include <vtkm/worklet/ZFP2DCompressor.h>

namespace vtkm
{
namespace filter
{
/// \brief Compress a scalar field using ZFP

/// Takes as input a 1D array and generates on
/// output of compressed data.
/// @warning
/// This filter is currently only supports 1D volumes.
class ZFPCompressor2D : public vtkm::filter::FilterField<ZFPCompressor2D>
{
public:
  VTKM_CONT
  ZFPCompressor2D();

  void SetRate(vtkm::Float64 _rate) { rate = _rate; }
  vtkm::Float64 GetRate() { return rate; }

  template <typename T, typename StorageType, typename DerivedPolicy>
  VTKM_CONT vtkm::cont::DataSet DoExecute(const vtkm::cont::DataSet& input,
                                          const vtkm::cont::ArrayHandle<T, StorageType>& field,
                                          const vtkm::filter::FieldMetadata& fieldMeta,
                                          const vtkm::filter::PolicyBase<DerivedPolicy>& policy);

  //Map a new field onto the resulting dataset after running the filter
  //this call is only valid
  template <typename T, typename StorageType, typename DerivedPolicy>
  VTKM_CONT bool DoMapField(vtkm::cont::DataSet& result,
                            const vtkm::cont::ArrayHandle<T, StorageType>& input,
                            const vtkm::filter::FieldMetadata& fieldMeta,
                            const vtkm::filter::PolicyBase<DerivedPolicy>& policy);

private:
  vtkm::Float64 rate;
  vtkm::worklet::ZFP2DCompressor compressor;
};

template <>
class FilterTraits<ZFPCompressor2D>
{
public:
  struct TypeListTagMCScalars : vtkm::ListTagBase<vtkm::Int32, vtkm::Float32, vtkm::Float64>
  {
  };
  using InputFieldTypeList = TypeListTagMCScalars;
};
}
} // namespace vtkm::filter

#include <vtkm/filter/ZFPCompressor2D.hxx>

#endif // vtk_m_filter_ZFPCompressor2D_h
