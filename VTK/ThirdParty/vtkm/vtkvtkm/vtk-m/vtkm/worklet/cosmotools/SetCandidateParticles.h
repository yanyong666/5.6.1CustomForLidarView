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

#ifndef vtkm_worklet_cosmotools_set_candidate_particles_h
#define vtkm_worklet_cosmotools_set_candidate_particles_h

#include <vtkm/worklet/WorkletMapField.h>

namespace vtkm
{
namespace worklet
{
namespace cosmotools
{

// Worklet to examine each bin against a cutoff and mark its particles as candidates
template <typename T>
class SetCandidateParticles : public vtkm::worklet::WorkletMapField
{
public:
  struct TagType : vtkm::ListTagBase<T>
  {
  };

  using ControlSignature = void(FieldIn bestPot,          // (input) bin's best potential
                                FieldIn partOffset,       // (input) offset to first particle
                                FieldIn partCount,        // (input) count of particles in bin
                                WholeArrayOut candidate); // (input) z location in bin
  using ExecutionSignature = void(_1, _2, _3, _4);
  using InputDomain = _1;

  T cutoffPotential; // Lowest potential for a candidate bin

  // Constructor
  VTKM_EXEC_CONT
  SetCandidateParticles(T Cutoff)
    : cutoffPotential(Cutoff)
  {
  }

  template <typename OutPortalType>
  VTKM_EXEC void operator()(const T& bestPot,
                            const vtkm::Id& offset,
                            const vtkm::Id& count,
                            const OutPortalType& candidate) const
  {
    if (bestPot <= cutoffPotential)
    {
      for (vtkm::Id part = offset; part < offset + count; part++)
        candidate.Set(part, 1);
    }
  }
}; // SetCandidateParticles
}
}
}

#endif
