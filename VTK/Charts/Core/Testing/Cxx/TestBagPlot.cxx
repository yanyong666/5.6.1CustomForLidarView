/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestBagPlot.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkChartXY.h"
#include "vtkContextScene.h"
#include "vtkContextView.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkNew.h"
#include "vtkPlotBag.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTable.h"

const double densities[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2.5e-005, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0.010065, 0.0500675, 0.07007, 0.0900725, 0.090075, 0.0900775, 0.07008, 0.0500825, 0.010085, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0.00011, 0.0501125, 0.100115, 0.140118, 0.17012, 0.190123, 0.190125, 0.190128, 0.17013, 0.140133, 0.100135, 0.0501375, 0.00014, 0, 0, 0,
  0, 0, 0, 0.0101575, 0.07016, 0.130163, 0.190165, 0.230168, 0.27017, 0.290173, 0.300175, 0.290177, 0.27018, 0.230183, 0.190185, 0.130188, 0.07019, 0.0101925, 0, 0,
  0, 0, 0.000205, 0.0702075, 0.15021, 0.210213, 0.270215, 0.320217, 0.36022, 0.390223, 0.400225, 0.390228, 0.36023, 0.320233, 0.270235, 0.210237, 0.15024, 0.0702425, 0.000245, 0,
  0, 0, 0.050255, 0.130258, 0.21026, 0.290262, 0.350265, 0.410267, 0.46027, 0.490273, 0.500275, 0.490277, 0.46028, 0.410282, 0.350285, 0.290287, 0.21029, 0.130293, 0.050295, 0,
  0, 0.0103025, 0.100305, 0.190307, 0.27031, 0.350312, 0.430315, 0.500317, 0.55032, 0.580322, 0.600325, 0.580328, 0.55033, 0.500332, 0.430335, 0.350337, 0.27034, 0.190343, 0.100345, 0.0103475,
  0, 0.0503525, 0.140355, 0.230358, 0.32036, 0.410362, 0.500365, 0.570367, 0.63037, 0.680373, 0.700375, 0.680378, 0.63038, 0.570382, 0.500385, 0.410387, 0.32039, 0.230393, 0.140395, 0.0503975,
  0, 0.0704025, 0.170405, 0.270408, 0.36041, 0.460413, 0.550415, 0.630417, 0.71042, 0.770423, 0.800425, 0.770428, 0.71043, 0.630432, 0.550435, 0.460438, 0.36044, 0.270443, 0.170445, 0.0704475,
  0, 0.0904525, 0.190455, 0.290457, 0.39046, 0.490462, 0.580465, 0.680468, 0.77047, 0.850472, 0.900475, 0.850478, 0.77048, 0.680483, 0.580485, 0.490487, 0.39049, 0.290492, 0.190495, 0.0904975,
  0.0005, 0.0905025, 0.190505, 0.300507, 0.40051, 0.500513, 0.600515, 0.700517, 0.80052, 0.900523, 1.00053, 0.900528, 0.80053, 0.700533, 0.600535, 0.500537, 0.40054, 0.300542, 0.190545, 0.0905475,
  0, 0.0905525, 0.190555, 0.290557, 0.39056, 0.490563, 0.580565, 0.680568, 0.77057, 0.850572, 0.900575, 0.850577, 0.77058, 0.680583, 0.580585, 0.490588, 0.39059, 0.290592, 0.190595, 0.0905975,
  0, 0.0706025, 0.170605, 0.270608, 0.36061, 0.460613, 0.550615, 0.630618, 0.71062, 0.770622, 0.800625, 0.770628, 0.71063, 0.630633, 0.550635, 0.460638, 0.36064, 0.270643, 0.170645, 0.0706475,
  0, 0.0506525, 0.140655, 0.230658, 0.32066, 0.410662, 0.500665, 0.570667, 0.63067, 0.680673, 0.700675, 0.680678, 0.63068, 0.570682, 0.500685, 0.410687, 0.32069, 0.230693, 0.140695, 0.0506975,
  0, 0.0107025, 0.100705, 0.190708, 0.27071, 0.350712, 0.430715, 0.500718, 0.55072, 0.580722, 0.600725, 0.580727, 0.55073, 0.500733, 0.430735, 0.350737, 0.27074, 0.190743, 0.100745, 0.0107475,
  0, 0, 0.050755, 0.130758, 0.21076, 0.290762, 0.350765, 0.410767, 0.46077, 0.490773, 0.500775, 0.490777, 0.46078, 0.410782, 0.350785, 0.290787, 0.21079, 0.130793, 0.050795, 0,
  0, 0, 0.000805, 0.0708075, 0.15081, 0.210812, 0.270815, 0.320818, 0.36082, 0.390823, 0.400825, 0.390827, 0.36083, 0.320833, 0.270835, 0.210837, 0.15084, 0.0708425, 0.000845, 0,
  0, 0, 0, 0.0108575, 0.07086, 0.130862, 0.190865, 0.230868, 0.27087, 0.290872, 0.300875, 0.290877, 0.27088, 0.230883, 0.190885, 0.130888, 0.07089, 0.0108925, 0, 0,
  0, 0, 0, 0, 0.00091, 0.0509125, 0.100915, 0.140918, 0.17092, 0.190922, 0.190925, 0.190928, 0.17093, 0.140933, 0.100935, 0.0509375, 0.00094, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0.010965, 0.0509675, 0.07097, 0.0909725, 0.090975, 0.0909775, 0.07098, 0.0509825, 0.010985, 0, 0, 0, 0, 0,
};
//----------------------------------------------------------------------------
int TestBagPlot(int, char * [])
{
  // Set up a 2D scene, add an XY chart to it
  vtkNew<vtkContextView> view;
  view->GetRenderWindow()->SetSize(400, 400);
  view->GetRenderWindow()->SetMultiSamples(0);
  vtkNew<vtkChartXY> chart;
  view->GetScene()->AddItem(chart);
  chart->SetShowLegend(true);

  // Creates a vtkPlotBag input table
  // We construct a 2D grid 20*20.
  int numDataI = 20;
  int numDataJ = 20;

  vtkNew<vtkIntArray> arrX;
  arrX->SetName("X");

  vtkNew<vtkDoubleArray> arrY;
  arrY->SetName("Y");

  vtkNew<vtkDoubleArray> arrDensity;
  arrDensity->SetName("Density");

  vtkNew<vtkTable> table;
  table->AddColumn(arrX);
  table->AddColumn(arrY);
  table->AddColumn(arrDensity);

  table->SetNumberOfRows(numDataI * numDataJ);

  // Fill the table
  for (int j = 0; j < numDataJ; ++j)
  {
    for (int i = 0; i < numDataI; ++i)
    {
      table->SetValue(i + j * numDataI, 0, i); //X
      table->SetValue(i + j * numDataI, 1, j); //Y
      double d = densities[i + j * numDataI];
      table->SetValue(i + j * numDataI, 2, d); // Density
    }
    cout << endl;
  }

  vtkNew<vtkPlotBag> bagPlot;
  chart->AddPlot(bagPlot);
  bagPlot->SetInputData(table, arrX->GetName(),
    arrY->GetName(), arrDensity->GetName());
  bagPlot->SetColor(255, 0, 0, 255);
  bagPlot->SetMarkerSize(4);

  // Render the scene
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
