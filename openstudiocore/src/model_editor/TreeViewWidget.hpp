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

#ifndef MODELEDITOR_TREEVIEWWIDGET_HPP
#define MODELEDITOR_TREEVIEWWIDGET_HPP

#include <QModelIndex>

#include "../model/Building.hpp"
#include "ModelEditorAPI.hpp"
#include "ViewWidget.hpp"

namespace modeleditor
{
  
class TreeModel;
class TreeView;

class TreeViewWidget : public ViewWidget
{
  Q_OBJECT

public:
  TreeViewWidget(openstudio::model::Model& model, QWidget *parent = nullptr);
  TreeViewWidget(QWidget *parent = nullptr);
  virtual ~TreeViewWidget();
  virtual void addObjects(openstudio::IddObjectType type = openstudio::IddObjectType("UserCustom")) override;
  virtual void loadModel() override;
  virtual void removeObjects() override;
  virtual void copyObjects() override;
  virtual void pasteObjects() override;
  virtual bool hasSelectedRows() override;
  virtual bool hasRowsToPaste() override;
  std::vector<openstudio::IddObjectType> getAllowableChildTypes();
  void expandAllNodes();
  void collapseAllNodes();
  TreeView* getTreeView();
  TreeModel* getTreeModel();
  virtual void toggleGUIDs() override;

public slots:
  virtual void viewSelection();
  virtual void viewSelection(const QModelIndex& modelIndex) override;
  virtual void on_nameChanged(QString) override;

signals:
  void expandAll();
  void collapseAll();

protected:
  TreeView * mTreeView;
  TreeModel * mTreeModel;

private:
  virtual void connectSignalsAndSlots() override;
  virtual void createLayout() override;
  virtual void createWidgets() override;
  virtual void loadData() override;
  virtual void loadModel(openstudio::model::Model& model) override;
  void expandAppropriateTreeNodes();
  void setExpandedIndexHandles();

  ///! only expanded nodes are here
  std::vector< std::pair<QModelIndex, openstudio::Handle> > mExpandedIndexHandles;

  std::vector<openstudio::model::ModelObject> mModelObjectsToPaste;
};

} // namespace modeleditor

#endif // MODELEDITOR_TREEVIEWWIDGET_HPP
