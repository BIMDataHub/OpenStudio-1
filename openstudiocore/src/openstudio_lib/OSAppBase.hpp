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

#ifndef OPENSTUDIO_OSAPPBASE_HPP
#define OPENSTUDIO_OSAPPBASE_HPP

#include "../shared_gui_components/BaseApp.hpp"

#include "../ruleset/RubyUserScriptInfoGetter.hpp"

#include "OpenStudioAPI.hpp"
#include "../utilities/core/Logger.hpp"

#include <QApplication>

#include <boost/smart_ptr.hpp>

class QEvent;

namespace openstudio {

class OSDocument;

class WaitDialog;

class OPENSTUDIO_API OSAppBase : public QApplication, public BaseApp
{

  Q_OBJECT

  public:

  OSAppBase( int & argc, char ** argv, const QSharedPointer<MeasureManager> &t_measureManager);

  virtual ~OSAppBase();

  virtual std::shared_ptr<OSDocument> currentDocument() const = 0;

  static OSAppBase * instance();

  virtual boost::optional<openstudio::analysisdriver::SimpleProject> project() override;
  virtual QWidget *mainWidget() override;
  virtual MeasureManager &measureManager() override;
  virtual boost::optional<openstudio::model::Model> currentModel() override;
  virtual boost::optional<openstudio::Workspace> currentWorkspace() override;
  virtual void updateSelectedMeasureState() override;
  virtual void addMeasure() override;
  virtual void duplicateSelectedMeasure() override;
  virtual void updateMyMeasures() override;
  virtual void updateBCLMeasures() override;
  virtual void downloadUpdatedBCLMeasures() override;
  virtual void openBclDlg() override;
  virtual void chooseHorizontalEditTab() override;
  virtual QSharedPointer<openstudio::EditController> editController() override;
  boost::shared_ptr<WaitDialog> waitDialog() {return m_waitDialog;}
  virtual bool notify(QObject * receiver, QEvent * e) override;

  protected:

  virtual bool event(QEvent * e) override;

  virtual void childEvent(QChildEvent * e) override;

  private:

  REGISTER_LOGGER("openstudio.OSAppBase");

  QSharedPointer<openstudio::MeasureManager> m_measureManager;

  boost::shared_ptr<WaitDialog> m_waitDialog;

  public slots:

  virtual void reloadFile(const QString& fileToLoad, bool modified, bool saveCurrentTabs) = 0;

  void showMeasureUpdateDlg();
};

} // openstudio

#endif // OPENSTUDIO_OSAPPBASE_HPP

