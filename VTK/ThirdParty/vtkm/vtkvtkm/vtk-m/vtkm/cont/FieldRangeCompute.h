//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//============================================================================
#ifndef vtk_m_cont_FieldRangeCompute_h
#define vtk_m_cont_FieldRangeCompute_h

#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/Field.h>
#include <vtkm/cont/MultiBlock.h>

#include <vtkm/cont/FieldRangeCompute.hxx>

namespace vtkm
{
namespace cont
{
/// \brief Compute ranges for fields in a DataSet or MultiBlock.
///
/// These methods to compute ranges for fields in a dataset or a multiblock.
/// When using VTK-m in a hybrid-parallel environment with distributed processing,
/// this class uses ranges for locally available data alone. Use FieldRangeGlobalCompute
/// to compute ranges globally across all ranks even in distributed mode.

//{@
/// Returns the range for a field from a dataset. If the field is not present, an empty
/// ArrayHandle will be returned.
VTKM_CONT_EXPORT
VTKM_CONT
vtkm::cont::ArrayHandle<vtkm::Range> FieldRangeCompute(
  const vtkm::cont::DataSet& dataset,
  const std::string& name,
  vtkm::cont::Field::Association assoc = vtkm::cont::Field::Association::ANY);

template <typename TypeList>
VTKM_CONT vtkm::cont::ArrayHandle<vtkm::Range> FieldRangeCompute(
  const vtkm::cont::DataSet& dataset,
  const std::string& name,
  vtkm::cont::Field::Association assoc,
  TypeList)
{
  VTKM_IS_LIST_TAG(TypeList);
  return vtkm::cont::detail::FieldRangeComputeImpl(dataset, name, assoc, TypeList());
}

//@}

//{@
/// Returns the range for a field from a multiblock. If the field is not present on any
/// of the blocks, an empty ArrayHandle will be returned. If the field is present on some blocks,
/// but not all, those blocks without the field are skipped.
///
/// The returned array handle will have as many values as the maximum number of components for
/// the selected field across all blocks.
VTKM_CONT_EXPORT
VTKM_CONT
vtkm::cont::ArrayHandle<vtkm::Range> FieldRangeCompute(
  const vtkm::cont::MultiBlock& multiblock,
  const std::string& name,
  vtkm::cont::Field::Association assoc = vtkm::cont::Field::Association::ANY);

template <typename TypeList>
VTKM_CONT vtkm::cont::ArrayHandle<vtkm::Range> FieldRangeCompute(
  const vtkm::cont::MultiBlock& multiblock,
  const std::string& name,
  vtkm::cont::Field::Association assoc,
  TypeList)
{
  VTKM_IS_LIST_TAG(TypeList);
  return vtkm::cont::detail::FieldRangeComputeImpl(multiblock, name, assoc, TypeList());
}

//@}
}
} // namespace vtkm::cont

#endif
