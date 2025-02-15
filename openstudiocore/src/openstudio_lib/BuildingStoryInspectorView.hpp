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

#ifndef OPENSTUDIO_BUILDINGSTORYINSPECTORVIEW_HPP
#define OPENSTUDIO_BUILDINGSTORYINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectVectorController.hpp"

#include "../model/BuildingStory.hpp"

class QLabel;
class QPushButton;
class QColor;

namespace openstudio {

class OSDropZone;
class OSLineEdit;
class OSQuantityEdit;
class OSQuantityEdit2;
class RenderingColorWidget;

class BuildingStoryDefaultConstructionSetVectorController : public ModelObjectVectorController
{
  Q_OBJECT

public:
  virtual ~BuildingStoryDefaultConstructionSetVectorController() {}

protected:

  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;

  virtual std::vector<OSItemId> makeVector() override;

  virtual void onRemoveItem(OSItem* item) override;

  virtual void onReplaceItem(OSItem * currentItem, const OSItemId& replacementItemId) override;

  virtual void onDrop(const OSItemId& itemId) override;
};

class BuildingStoryDefaultScheduleSetVectorController : public ModelObjectVectorController
{
  Q_OBJECT

public:

  virtual ~BuildingStoryDefaultScheduleSetVectorController() {}

protected:

  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;

  virtual std::vector<OSItemId> makeVector() override;

  virtual void onRemoveItem(OSItem* item) override;

  virtual void onReplaceItem(OSItem * currentItem, const OSItemId& replacementItemId) override;

  virtual void onDrop(const OSItemId& itemId) override;
};

class BuildingStorySpacesVectorController : public ModelObjectVectorController
{
  Q_OBJECT

public:

  virtual ~BuildingStorySpacesVectorController() {}
  
  virtual void attach(const model::ModelObject& modelObject) override;

protected:

  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;

  virtual std::vector<OSItemId> makeVector() override;

  virtual void onObjectAdded(const openstudio::model::ModelObject& modelObject, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle) override;
  
  virtual void onObjectRemoved(const openstudio::model::ModelObject& modelObject, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle) override;
  
  virtual void onRemoveItem(OSItem* item) override;

  virtual void onDrop(const OSItemId& itemId) override;
};

class BuildingStoryUnassignedSpacesVectorController : public ModelObjectVectorController
{
  Q_OBJECT

public:

  virtual ~BuildingStoryUnassignedSpacesVectorController() {}

  virtual void attachModel(const model::Model& model) override;

protected:

  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;

  virtual std::vector<OSItemId> makeVector() override;

  virtual void onObjectAdded(const openstudio::model::ModelObject& modelObject, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle) override;

  virtual void onObjectRemoved(const openstudio::model::ModelObject& modelObject, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle) override;
};


class BuildingStoryInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

  public:

    BuildingStoryInspectorView(bool isIP, const openstudio::model::Model& model, QWidget * parent = nullptr);
    
    virtual ~BuildingStoryInspectorView() {}

  protected:

    virtual void onClearSelection() override;

    virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

    virtual void onUpdate() override;

  private:

    void attach(openstudio::model::BuildingStory& buildingStory);

    void detach();

    OSLineEdit* m_nameEdit;
    OSQuantityEdit2 * m_zCoordinate = nullptr;
    OSQuantityEdit2 * m_floorToFloorHeight = nullptr;
    OSQuantityEdit2 * m_floorToCeilingHeight = nullptr;
    BuildingStoryDefaultConstructionSetVectorController* m_defaultConstructionSetVectorController;
    OSDropZone* m_defaultConstructionSetDropZone;
    BuildingStoryDefaultScheduleSetVectorController* m_defaultScheduleSetVectorController;
    OSDropZone* m_defaultScheduleSetDropZone;
    RenderingColorWidget* m_renderingColorWidget;
    bool m_isIP;
    //BuildingStorySpacesVectorController* m_spacesVectorController;
    //OSDropZone* m_spacesDropZone;

  public slots:

    void toggleUnits(bool displayIP) override;
};


} // openstudio

#endif // OPENSTUDIO_BUILDINGSTORYINSPECTORVIEW_HPP

