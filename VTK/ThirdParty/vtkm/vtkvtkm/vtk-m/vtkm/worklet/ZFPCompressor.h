//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//============================================================================
#ifndef vtk_m_worklet_zfp_compressor_h
#define vtk_m_worklet_zfp_compressor_h

#include <vtkm/Math.h>
#include <vtkm/cont/Algorithm.h>
#include <vtkm/cont/ArrayHandle.h>
#include <vtkm/cont/ArrayHandleConstant.h>
#include <vtkm/cont/ArrayHandleCounting.h>
#include <vtkm/cont/AtomicArray.h>
#include <vtkm/cont/Timer.h>
#include <vtkm/worklet/DispatcherMapField.h>

#include <vtkm/worklet/zfp/ZFPEncode3.h>
#include <vtkm/worklet/zfp/ZFPTools.h>

using ZFPWord = vtkm::UInt64;

#include <stdio.h>

namespace vtkm
{
namespace worklet
{

class ZFPCompressor
{
public:
  template <typename Scalar, typename Storage>
  vtkm::cont::ArrayHandle<vtkm::Int64> Compress(
    const vtkm::cont::ArrayHandle<Scalar, Storage>& data,
    const vtkm::Float64 requestedRate,
    const vtkm::Id3 dims)
  {
    // DataDump(data, "uncompressed");
    zfp::ZFPStream stream;
    const vtkm::Int32 topoDims = 3;
    stream.SetRate(requestedRate, topoDims, vtkm::Float64());
    //VTKM_ASSERT(

    // Check to see if we need to increase the block sizes
    // in the case where dim[x] is not a multiple of 4

    vtkm::Id3 paddedDims = dims;
    // ensure that we have block sizes
    // that are a multiple of 4
    if (paddedDims[0] % 4 != 0)
      paddedDims[0] += 4 - dims[0] % 4;
    if (paddedDims[1] % 4 != 0)
      paddedDims[1] += 4 - dims[1] % 4;
    if (paddedDims[2] % 4 != 0)
      paddedDims[2] += 4 - dims[2] % 4;
    const vtkm::Id four = 4;
    vtkm::Id totalBlocks =
      (paddedDims[0] / four) * (paddedDims[1] / (four) * (paddedDims[2] / four));


    size_t outbits = zfp::detail::CalcMem3d(paddedDims, stream.minbits);
    vtkm::Id outsize = vtkm::Id(outbits / sizeof(ZFPWord));

    vtkm::cont::ArrayHandle<vtkm::Int64> output;
    // hopefully this inits/allocates the mem only on the device
    vtkm::cont::ArrayHandleConstant<vtkm::Int64> zero(0, outsize);
    vtkm::cont::Algorithm::Copy(zero, output);

    // launch 1 thread per zfp block
    vtkm::cont::ArrayHandleCounting<vtkm::Id> blockCounter(0, 1, totalBlocks);

    //    using Timer = vtkm::cont::Timer<vtkm::cont::DeviceAdapterTagSerial>;
    //    Timer timer;
    vtkm::worklet::DispatcherMapField<zfp::Encode3> compressDispatcher(
      zfp::Encode3(dims, paddedDims, stream.maxbits));
    compressDispatcher.Invoke(blockCounter, data, output);

    //    vtkm::Float64 time = timer.GetElapsedTime();
    //    size_t total_bytes =  data.GetNumberOfValues() * sizeof(vtkm::Float64);
    //    vtkm::Float64 gB = vtkm::Float64(total_bytes) / (1024. * 1024. * 1024.);
    //    vtkm::Float64 rate = gB / time;
    //    std::cout<<"Compress time "<<time<<" sec\n";
    //    std::cout<<"Compress rate "<<rate<<" GB / sec\n";
    //    DataDump(output, "compressed");

    return output;
  }
};
} // namespace worklet
} // namespace vtkm
#endif //  vtk_m_worklet_zfp_compressor_h
