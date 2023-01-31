// -*- c++ -*-

#ifndef pqGMVReaderPanel_h
#define pqGMVReaderPanel_h

// ParaView Server Manager includes
#include <vtkSMIntVectorProperty.h>
#include <vtkSMProxy.h>

// Qt Includes.
#include <QCheckBox>
#include <QtDebug>

// ParaView Includes.
#include <pqAutoGeneratedObjectPanel.h>
#include <pqTreeWidget.h>
#include <pqTreeWidgetItemObject.h>
#include <vtkPVConfig.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <winsock.h> // WSADATA, include before sys/types.h
#endif

/// Custom panel for GMVReader source.
/// This panel is only provided to add capabilities to hide/gray out
/// GUI elements in case they do not apply and to link the checkbox
/// ImportTracers to PointArrayStatus.
class pqGMVReaderPanel : public pqAutoGeneratedObjectPanel
{
  Q_OBJECT;
  typedef pqAutoGeneratedObjectPanel Superclass;

public:
  pqGMVReaderPanel(pqProxy* prox, QWidget* par);

private:
  std::string GetHostname();

protected:
  vtkSMIntVectorProperty* timeSteps;
  QCheckBox* IgnoreReaderWidget;

  vtkSMIntVectorProperty* tracers;
  QCheckBox* ImportTracersWidget;

  vtkSMIntVectorProperty* polygons;
  QCheckBox* ImportPolygonsWidget;

  pqTreeWidget* treeWidget;

protected slots:
  void updateTracerDataStatus(int state);
};

#endif // pqGMVReaderPanel_h
