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

#ifndef PATAPP_STARTUPVIEW_HPP
#define PATAPP_STARTUPVIEW_HPP

#include <QWidget>

namespace openstudio{
  
namespace pat {

class StartupView : public QWidget
{
  Q_OBJECT

  public:

  StartupView(QWidget *parent = nullptr);

  virtual ~StartupView() {}

  protected:

  void paintEvent(QPaintEvent *) override;
};

class LoadingProjectView : public QWidget
{
  Q_OBJECT

  public:

  LoadingProjectView(QWidget * parent = nullptr);

  virtual ~LoadingProjectView() {}

  protected:

  void paintEvent(QPaintEvent *) override;
};

} // pat

} // openstudio

#endif // PATAPP_STARTUPVIEW_HPP

