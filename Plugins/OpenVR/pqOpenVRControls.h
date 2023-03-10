
/*=========================================================================

  Program:   ParaView

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   pqOpenVRControls
 * @brief   ParaView GUI for use within virtual reality
 *
 * This class brings elements of the ParaView GUI into VR where they
 * can be used. Instantiated by the pqOpenVRDockPanel.
 */

#include <QWidget>

class pqPipelineSource;
class pqView;
class vtkPVOpenVRHelper;

class pqOpenVRControls : public QWidget
{
  Q_OBJECT
  typedef QWidget Superclass;

public:
  pqOpenVRControls(vtkPVOpenVRHelper* val, QWidget* p = 0)
    : Superclass(p)
  {
    this->constructor(val);
  }
  ~pqOpenVRControls();

  pqPipelineSource* GetSelectedPipelineSource();

  // set the right trigger menu to the passed value
  // used when the helper programmatically adjusts
  // the mode
  void SetRightTriggerMode(std::string const& text);

protected:
  vtkPVOpenVRHelper* Helper;

protected slots:
  void rightTriggerChanged(QString const& text);
  void controllerLabelsChanged(int);
  void navigationPanelChanged(int);
  void exit();

private:
  void constructor(vtkPVOpenVRHelper* val);

  class pqInternals;
  pqInternals* Internals;
};
