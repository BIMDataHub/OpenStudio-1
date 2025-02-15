/**********************************************************************
 *  Copyright (c) 2008-2015, Alliance for Sustainable Energy.  
 *  All rights reserved.
 *  
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#ifndef OPENSTUDIO_INSPECTORVIEW_HPP
#define OPENSTUDIO_INSPECTORVIEW_HPP

#include <QDialog>
#include <QWidget>

#include "../model/HVACComponent.hpp"
#include "../model/HVACComponent_Impl.hpp"
#include "../model/ModelObject.hpp"
#include "../model/StraightComponent.hpp"
#include "../model/StraightComponent_Impl.hpp"
#include "../model/WaterToAirComponent.hpp"
#include "../model/WaterToAirComponent_Impl.hpp"

class InspectorGadget;

class QComboBox;
class QPushButton;
class QStackedWidget;
class QTimer;
class QToolButton;
class QVBoxLayout;

namespace openstudio {

namespace model {

class Loop;
class ThermalZone;

}

class BaseInspectorView;
class LibraryTabWidget;
class LoopChooserView;
class ZoneChooserView;
class OSItem;

class InspectorView : public QWidget
{
  Q_OBJECT;

  public:

  InspectorView(QWidget* parent = nullptr);
  virtual ~InspectorView() {}

  void update();

  BaseInspectorView * currentView() { return m_currentView; }

  bool mouseOverInspectorView() { return m_mouseOverInspectorView; };

  signals:

  void addZoneClicked(model::ThermalZone &);
  void removeZoneClicked(model::ThermalZone &);
  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);
  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);
  void toggleUnitsClicked(bool displayIP);
  void itemRemoveClicked(OSItem *);
  void removeButtonClicked(bool);
  void workspaceObjectRemoved();

  void moveBranchForZoneSupplySelected(model::ThermalZone & zone, const Handle & newPlenumHandle);
  void moveBranchForZoneReturnSelected(model::ThermalZone & zone, const Handle & newPlenumHandle);
  void moveBranchForZoneToNewSupplyPlenumSelected(model::ThermalZone & zone, const Handle & newPlenumZoneHandle);
  void moveBranchForZoneToNewReturnPlenumSelected(model::ThermalZone & zone, const Handle & newPlenumZoneHandle);

  public slots:

  void layoutModelObject( model::OptionalModelObject &, bool readOnly, bool displayIP );
  virtual void toggleUnits(bool displayIP);

  protected slots:

  virtual void enterEvent(QEvent * event) override;
  virtual void leaveEvent(QEvent * event) override;

  private:

  QVBoxLayout * m_vLayout;
  BaseInspectorView * m_currentView;
  QStackedWidget * m_stackedWidget;
  boost::optional<model::ModelObject> m_modelObject;
  bool m_mouseOverInspectorView = false;
};

class BaseInspectorView : public QWidget
{
  Q_OBJECT;

  public:

  BaseInspectorView(QWidget * parent = nullptr);

  virtual ~BaseInspectorView() {}

  void addMainView( QWidget * widget );

  virtual void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) = 0;

  virtual void update() {}

  LibraryTabWidget * m_libraryTabWidget;

  signals:

  void toggleUnitsClicked(bool displayIP); 

  void removeButtonClicked(bool);

  void workspaceObjectRemoved();

  private:

  QVBoxLayout * m_vLayout;

  public slots:

  virtual void toggleUnits(bool displayIP);
};

class GenericInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:
  
  GenericInspectorView(QWidget * parent = nullptr);

  virtual ~GenericInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;

  private:

  InspectorGadget * m_inspectorGadget;

  signals:

  void removeButtonClicked(bool);
};

class SplitterMixerInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  SplitterMixerInspectorView(QWidget * parent = nullptr);

  virtual ~SplitterMixerInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;

  signals:

  void addZoneClicked(model::ThermalZone &);

  void removeZoneClicked(model::ThermalZone &);

  private:

  ZoneChooserView * m_zoneChooserView;
};

class NewPlenumDialog : public QDialog
{
  Q_OBJECT;

  public:

  NewPlenumDialog(QWidget * parent = nullptr);

  virtual ~NewPlenumDialog() {}

  QComboBox * zoneChooser;

  private slots:

  void onCancelClicked(); 
  void onApplyClicked(); 
};

class PlenumChooserView : public QWidget
{
  Q_OBJECT;

  public:
  
  PlenumChooserView(QWidget * parent = nullptr);

  virtual ~PlenumChooserView() {}

  QComboBox * supplyPlenumChooser;
  QComboBox * returnPlenumChooser;
  QToolButton * newSupplyPlenumButton;
  QToolButton * newReturnPlenumButton;
};

class RefrigerationWalkinInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  RefrigerationWalkinInspectorView(QWidget * parent = nullptr);

  virtual ~RefrigerationWalkinInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;

  private:

  InspectorGadget * m_walkinInspectorGadget;
  QWidget * m_zoneBoundaryWidget;
  bool m_displayIP;
  boost::optional<model::ModelObject> m_modelObject;
};

class ThermalZoneInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  ThermalZoneInspectorView(QWidget * parent = nullptr);

  virtual ~ThermalZoneInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;

  signals:

  void moveBranchForZoneSupplySelected(model::ThermalZone & zone, const Handle & newPlenumHandle);
  void moveBranchForZoneReturnSelected(model::ThermalZone & zone, const Handle & newPlenumHandle);

  private slots:

  void onSupplyPlenumChooserChanged(int newIndex);
  void onReturnPlenumChooserChanged(int newIndex);

  void onNewSupplyPlenumClicked();
  void onNewReturnPlenumClicked();

  private:

  void update() override;
  InspectorGadget * m_inspectorGadget;
  PlenumChooserView * m_plenumChooser;
  bool m_displayIP;
  boost::optional<model::ModelObject> m_modelObject;
};

class WaterToAirInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  WaterToAirInspectorView(QWidget * parent = nullptr);

  virtual ~WaterToAirInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private slots:

  void onWorkspaceObjectChanged();

  void layoutControllerObject();

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  InspectorGadget * m_coilControllerInspectorGadget;

  LoopChooserView * m_loopChooserView, * m_coolingLoopChooserView;

  bool m_displayIP;
};

class AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView(QWidget * parent = nullptr);

  virtual ~AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_coolingLoopChooserView;
};

class AirTerminalInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  AirTerminalInspectorView(QWidget * parent = nullptr);

  virtual ~AirTerminalInspectorView() {}

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  protected:

  template <class T>
  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP);

  //private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_loopChooserView;
};

class AirTerminalSingleDuctConstantVolumeReheatInspectorView : public AirTerminalInspectorView
{
  Q_OBJECT;

  public:

  AirTerminalSingleDuctConstantVolumeReheatInspectorView(QWidget * parent = nullptr);

  virtual ~AirTerminalSingleDuctConstantVolumeReheatInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;
};

class AirTerminalSingleDuctVAVReheatInspectorView : public AirTerminalInspectorView
{
  Q_OBJECT;

  public:

  AirTerminalSingleDuctVAVReheatInspectorView(QWidget * parent = nullptr);

  virtual ~AirTerminalSingleDuctVAVReheatInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;
};

class AirTerminalSingleDuctParallelPIUReheatInspectorView : public AirTerminalInspectorView
{
  Q_OBJECT;

  public:

  AirTerminalSingleDuctParallelPIUReheatInspectorView(QWidget * parent = nullptr);

  virtual ~AirTerminalSingleDuctParallelPIUReheatInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;
};

class AirTerminalSingleDuctSeriesPIUReheatInspectorView : public AirTerminalInspectorView
{
  Q_OBJECT;

  public:

  AirTerminalSingleDuctSeriesPIUReheatInspectorView(QWidget * parent = nullptr);

  virtual ~AirTerminalSingleDuctSeriesPIUReheatInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;
};

class AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView : public AirTerminalInspectorView
{
  Q_OBJECT;

  public:

  AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView(QWidget * parent = nullptr);

  virtual ~AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;
};

class ZoneHVACPackagedTerminalAirConditionerInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  ZoneHVACPackagedTerminalAirConditionerInspectorView(QWidget * parent = nullptr);

  virtual ~ZoneHVACPackagedTerminalAirConditionerInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_loopChooserView;
};

class ZoneHVACFourPipeFanCoilInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  ZoneHVACFourPipeFanCoilInspectorView(QWidget * parent = nullptr);

  virtual ~ZoneHVACFourPipeFanCoilInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP ) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_heatingLoopChooserView;

  LoopChooserView * m_coolingLoopChooserView;
};

class ZoneHVACLowTempRadiantConstFlowInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  ZoneHVACLowTempRadiantConstFlowInspectorView(QWidget * parent = nullptr);

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP ) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_heatingLoopChooserView;

  LoopChooserView * m_coolingLoopChooserView;
};

class ZoneHVACLowTempRadiantVarFlowInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  ZoneHVACLowTempRadiantVarFlowInspectorView(QWidget * parent = nullptr);

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP ) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_heatingLoopChooserView;

  LoopChooserView * m_coolingLoopChooserView;
};

class ZoneHVACWaterToAirHeatPumpInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  ZoneHVACWaterToAirHeatPumpInspectorView(QWidget * parent = nullptr);

  virtual ~ZoneHVACWaterToAirHeatPumpInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP ) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_heatingLoopChooserView;

  LoopChooserView * m_coolingLoopChooserView;

  LoopChooserView * m_supHeatingLoopChooserView;
};

class ZoneHVACBaseboardConvectiveWaterInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  ZoneHVACBaseboardConvectiveWaterInspectorView(QWidget * parent = nullptr);

  virtual ~ZoneHVACBaseboardConvectiveWaterInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP ) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_heatingLoopChooserView;

};

class ZoneHVACUnitHeaterInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  ZoneHVACUnitHeaterInspectorView(QWidget * parent = nullptr);

  virtual ~ZoneHVACUnitHeaterInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP ) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_heatingLoopChooserView;

};

class AirLoopHVACUnitarySystemInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  AirLoopHVACUnitarySystemInspectorView(QWidget * parent = nullptr);

  virtual ~AirLoopHVACUnitarySystemInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP ) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_heatingLoopChooserView;
  LoopChooserView * m_coolingLoopChooserView;
  LoopChooserView * m_secondaryLoopChooserView;

};

class AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView(QWidget * parent = nullptr);

  virtual ~AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView() {}

  void layoutModelObject( model::ModelObject &, bool readOnly, bool displayIP ) override;

  signals:

  void addToLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  void removeFromLoopClicked(model::Loop &, boost::optional<model::HVACComponent> &);

  private:

  boost::optional<model::ModelObject> m_modelObject;

  InspectorGadget * m_inspectorGadget;

  LoopChooserView * m_heatingLoopChooserView;
  LoopChooserView * m_coolingLoopChooserView;

};

class ScheduleRulesetInspectorView : public BaseInspectorView
{
  Q_OBJECT;

  public:

  ScheduleRulesetInspectorView(QWidget * parent = nullptr);

  virtual ~ScheduleRulesetInspectorView() {}

  virtual void layoutModelObject(model::ModelObject & modelObject, bool readOnly, bool displayIP) override;

};

} // openstudio

#endif // OPENSTUDIO_INSPECTORVIEW_HPP

