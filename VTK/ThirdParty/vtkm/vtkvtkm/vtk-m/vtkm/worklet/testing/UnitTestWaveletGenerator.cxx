//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//============================================================================

#include <vtkm/worklet/WaveletGenerator.h>

#include <vtkm/cont/Timer.h>
#include <vtkm/cont/testing/Testing.h>

namespace detail
{

struct WaveletGeneratorFunctor
{
  vtkm::cont::DataSet Ds;
  template <typename DeviceAdapter>
  bool operator()(DeviceAdapter)
  {
    vtkm::worklet::WaveletGenerator gen;
    Ds = gen.GenerateDataSet<DeviceAdapter>();
    return true;
  }
};
}

void WaveletGeneratorTest()
{
  vtkm::cont::Timer timer;
  timer.Start();
  detail::WaveletGeneratorFunctor wgFunctor;
  vtkm::cont::TryExecute(wgFunctor);


  double time = timer.GetElapsedTime();

  std::cout << "Default wavelet took " << time << "s.\n";

  {
    auto coords = wgFunctor.Ds.GetCoordinateSystem("coords");
    auto data = coords.GetData();
    VTKM_TEST_ASSERT(test_equal(data.GetNumberOfValues(), 9261), "Incorrect number of points.");
  }

  {
    auto cells = wgFunctor.Ds.GetCellSet(wgFunctor.Ds.GetCellSetIndex("cells"));
    VTKM_TEST_ASSERT(test_equal(cells.GetNumberOfCells(), 8000), "Incorrect number of cells.");
  }

  // Spot check some scalars
  {
    using ScalarHandleType = vtkm::cont::ArrayHandle<vtkm::FloatDefault>;

    auto field = wgFunctor.Ds.GetField("scalars", vtkm::cont::Field::Association::POINTS);
    auto dynData = field.GetData();
    VTKM_TEST_ASSERT(dynData.IsType<ScalarHandleType>(), "Invalid scalar handle type.");
    ScalarHandleType handle = dynData.Cast<ScalarHandleType>();
    auto data = handle.GetPortalConstControl();

    VTKM_TEST_ASSERT(test_equal(data.GetNumberOfValues(), 9261), "Incorrect number of scalars.");

    VTKM_TEST_ASSERT(test_equal(data.Get(0), 60.7635), "Incorrect scalar value.");
    VTKM_TEST_ASSERT(test_equal(data.Get(16), 99.6115), "Incorrect scalar value.");
    VTKM_TEST_ASSERT(test_equal(data.Get(21), 69.1968), "Incorrect scalar value.");
    VTKM_TEST_ASSERT(test_equal(data.Get(256), 118.620), "Incorrect scalar value.");
    VTKM_TEST_ASSERT(test_equal(data.Get(1024), 140.466), "Incorrect scalar value.");
    VTKM_TEST_ASSERT(test_equal(data.Get(1987), 203.720), "Incorrect scalar value.");
    VTKM_TEST_ASSERT(test_equal(data.Get(2048), 223.010), "Incorrect scalar value.");
    VTKM_TEST_ASSERT(test_equal(data.Get(3110), 128.282), "Incorrect scalar value.");
    VTKM_TEST_ASSERT(test_equal(data.Get(4097), 153.913), "Incorrect scalar value.");
    VTKM_TEST_ASSERT(test_equal(data.Get(6599), 120.068), "Incorrect scalar value.");
    VTKM_TEST_ASSERT(test_equal(data.Get(7999), 65.6710), "Incorrect scalar value.");
  }
}

int UnitTestWaveletGenerator(int argc, char* argv[])
{
  return vtkm::cont::testing::Testing::Run(WaveletGeneratorTest, argc, argv);
}
