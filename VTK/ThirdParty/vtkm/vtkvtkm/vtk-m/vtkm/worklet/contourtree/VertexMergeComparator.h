//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//============================================================================
//  Copyright (c) 2016, Los Alamos National Security, LLC
//  All rights reserved.
//
//  Copyright 2016. Los Alamos National Security, LLC.
//  This software was produced under U.S. Government contract DE-AC52-06NA25396
//  for Los Alamos National Laboratory (LANL), which is operated by
//  Los Alamos National Security, LLC for the U.S. Department of Energy.
//  The U.S. Government has rights to use, reproduce, and distribute this
//  software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC
//  MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE
//  USE OF THIS SOFTWARE.  If software is modified to produce derivative works,
//  such modified software should be clearly marked, so as not to confuse it
//  with the version available from LANL.
//
//  Additionally, redistribution and use in source and binary forms, with or
//  without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//  3. Neither the name of Los Alamos National Security, LLC, Los Alamos
//     National Laboratory, LANL, the U.S. Government, nor the names of its
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND
//  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
//  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
//  NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
//  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//============================================================================

//  This code is based on the algorithm presented in the paper:
//  “Parallel Peak Pruning for Scalable SMP Contour Tree Computation.”
//  Hamish Carr, Gunther Weber, Christopher Sewell, and James Ahrens.
//  Proceedings of the IEEE Symposium on Large Data Analysis and Visualization
//  (LDAV), October 2016, Baltimore, Maryland.

//=======================================================================================
//
// COMMENTS:
//
// Basically, we have a single functor (so far), whose job is to work out the downwards
// join neighbour of each vertex.
//
// Any vector needed by the functor for lookup purposes will be passed as a parameter to
// the constructor and saved, with the actual function call being the operator ()
//
//=======================================================================================

#ifndef vtkm_worklet_contourtree_vertex_merge_comparator_h
#define vtkm_worklet_contourtree_vertex_merge_comparator_h

#include <vtkm/cont/ArrayHandle.h>
#include <vtkm/cont/ExecutionObjectBase.h>

namespace vtkm
{
namespace worklet
{
namespace contourtree
{

//=======================================================================================
//
//	VertexMergeComparator
//
// A comparator that sorts the vertices on the join maximum (assuming already sorted on
// indexed value)
//
//=======================================================================================

template <typename T, typename StorageType>
class VertexMergeComparator : public vtkm::cont::ExecutionObjectBase
{
public:
  using ValueArrayType = vtkm::cont::ArrayHandle<T, StorageType>;
  using IdArrayType = vtkm::cont::ArrayHandle<vtkm::Id>;

  ValueArrayType Values;
  IdArrayType Extrema;
  bool IsJoinTree;

  VTKM_CONT
  VertexMergeComparator(ValueArrayType values, IdArrayType extrema, bool isJoinTree)
    : Values(values)
    , Extrema(extrema)
    , IsJoinTree(isJoinTree)
  {
  }

  template <typename DeviceAdapter>
  class ExecObject
  {
  public:
    using ValuePortalType =
      typename ValueArrayType::template ExecutionTypes<DeviceAdapter>::PortalConst;
    using IdPortalType = typename IdArrayType::template ExecutionTypes<DeviceAdapter>::PortalConst;

    ValuePortalType Values;
    IdPortalType Extrema;
    bool IsJoinTree;

    VTKM_CONT
    ExecObject(ValuePortalType values, IdPortalType extrema, bool isJoinTree)
      : Values(values)
      , Extrema(extrema)
      , IsJoinTree(isJoinTree)
    {
    }

    VTKM_EXEC
    bool operator()(const vtkm::Id& i, const vtkm::Id& j) const
    {
      // retrieve the pseudo-extremum the vertex belongs to
      vtkm::Id pseudoExtI = this->Extrema.Get(i);
      vtkm::Id pseudoExtJ = this->Extrema.Get(j);

      if (pseudoExtI < pseudoExtJ)
        return false ^ this->IsJoinTree;
      if (pseudoExtJ < pseudoExtI)
        return true ^ this->IsJoinTree;

      T valueI = this->Values.Get(i);
      T valueJ = this->Values.Get(j);

      if (valueI < valueJ)
      {
        return false ^ this->IsJoinTree;
      }
      if (valueI > valueJ)
      {
        return true ^ this->IsJoinTree;
      }
      if (i < j)
      {
        return false ^ this->IsJoinTree;
      }
      if (j < i)
      {
        return true ^ this->IsJoinTree;
      }
      return false; // true ^ isJoinTree;
    }
  };

  template <typename DeviceAdapter>
  VTKM_CONT ExecObject<DeviceAdapter> PrepareForExecution(DeviceAdapter) const
  {
    return ExecObject<DeviceAdapter>(this->Values.PrepareForInput(DeviceAdapter()),
                                     this->Extrema.PrepareForInput(DeviceAdapter()),
                                     this->IsJoinTree);
  }
}; // VertexMergeComparator
}
}
}

#endif
