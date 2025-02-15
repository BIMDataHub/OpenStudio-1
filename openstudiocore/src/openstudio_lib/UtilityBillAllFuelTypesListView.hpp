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

#ifndef OPENSTUDIO_UTILITYBILLALLFUELTYPESLISTVIEW_HPP
#define OPENSTUDIO_UTILITYBILLALLFUELTYPESLISTVIEW_HPP

#include "OSCollapsibleItemList.hpp"

#include "../model/Model.hpp"
#include "../model/ModelObject.hpp"

#include "../utilities/data/DataEnums.hpp"

#include <boost/optional.hpp>

namespace openstudio {

class FuelType;

class UtilityBillAllFuelTypesListView : public OSCollapsibleItemList
{
  Q_OBJECT

  public:
    UtilityBillAllFuelTypesListView(const model::Model& model, 
                            bool addScrollArea, 
                            OSItemType headerType,
                            QWidget * parent = nullptr);

    UtilityBillAllFuelTypesListView(const std::vector<std::pair<FuelType, std::string> >& utilityBillFuelTypesAndNames,
                            const model::Model& model,
                            bool addScrollArea,
                            OSItemType headerType,
                            QWidget * parent = nullptr);

    virtual ~UtilityBillAllFuelTypesListView() {}

    void addUtilityBillFuelType(const FuelType & fuelType, const std::string& name);

    void addModelObjectType(const IddObjectType& iddObjectType, const std::string& name);

    FuelType currentFuelType() const;

    boost::optional<openstudio::FuelType> selectedFuelType() const;

    virtual boost::optional<openstudio::model::ModelObject> selectedModelObject() const;

  private:

    std::vector<std::pair<FuelType, std::string> > m_utilityBillFuelTypesAndNames;

    model::Model m_model;
    OSItemType m_headerType;
    bool m_showLocalBCL;
};



} // openstudio

#endif // OPENSTUDIO_UTILITYBILLALLFUELTYPESLISTVIEW_HPP

