//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//============================================================================
#ifndef vtk_m_internal_ArrayPortalHelpers_h
#define vtk_m_internal_ArrayPortalHelpers_h


#include <vtkm/VecTraits.h>
#include <vtkm/internal/ExportMacros.h>

namespace vtkm
{
namespace internal
{

template <typename PortalType>
struct PortalSupportsGets
{
  template <typename U, typename S = decltype(std::declval<U>().Get(vtkm::Id{}))>
  static std::true_type has(int);
  template <typename U>
  static std::false_type has(...);
  using type = decltype(has<PortalType>(0));
};

template <typename PortalType>
struct PortalSupportsSets
{
  template <typename U,
            typename S = decltype(std::declval<U>().Set(vtkm::Id{},
                                                        std::declval<typename U::ValueType>()))>
  static std::true_type has(int);
  template <typename U>
  static std::false_type has(...);
  using type = decltype(has<PortalType>(0));
};
}
} // namespace vtkm::internal

#endif //vtk_m_internal_ArrayPortalHelpers_h
