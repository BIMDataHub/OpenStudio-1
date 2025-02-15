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

#ifndef OPENSTUDIO_DESIGNDAYGRIDVIEW_HPP
#define OPENSTUDIO_DESIGNDAYGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "OSItem.hpp"

#include "../model/Model.hpp"

#include <QWidget>

namespace openstudio{

class DesignDayGridController;

class DesignDayGridView : public QWidget
{
  Q_OBJECT

public:

  DesignDayGridView(bool isIP, const model::Model & model, QWidget * parent = 0);

  virtual ~DesignDayGridView() {}

  DesignDayGridController * m_gridController = nullptr;

  std::vector<model::ModelObject> selectedObjects() const;

private:

  void addObject(const model::ModelObject& modelObject);

  void copyObject(const openstudio::model::ModelObject& modelObject);

  void removeObject(openstudio::model::ModelObject modelObject);

  void purgeObjects(const IddObjectType& iddObjectType);

  bool m_isIP;

signals:

  void toggleUnitsClicked(bool displayIP);

  void dropZoneItemClicked(OSItem* item);

public slots:

  void onAddClicked();

  void onCopyClicked();

  void onRemoveClicked();

  void onPurgeClicked();

};

class DesignDayGridController : public OSGridController
{

  Q_OBJECT

public:

  DesignDayGridController(bool isIP,
    const QString & headerText,
    IddObjectType iddObjectType,
    model::Model model,
    std::vector<model::ModelObject> modelObjects);

  virtual ~DesignDayGridController() {}

  virtual void refreshModelObjects();

protected:

  virtual void setCategoriesAndFields();

  virtual void addColumns(const QString &t_category, std::vector<QString> & fields);

  virtual void checkSelectedFields();

  virtual QString getColor(const model::ModelObject & modelObject);

public slots:

  virtual void onItemDropped(const OSItemId& itemId);

  virtual void onComboBoxIndexChanged(int index);

};

} // openstudio

#endif // OPENSTUDIO_DESIGNDAYGRIDVIEW_HPP

