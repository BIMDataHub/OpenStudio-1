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

#ifndef MODEL_SIZINGPARAMETERS_IMPL_HPP
#define MODEL_SIZINGPARAMETERS_IMPL_HPP

#include "ModelAPI.hpp"
#include "ModelObject_Impl.hpp"

#include "../utilities/units/Quantity.hpp"
#include "../utilities/units/OSOptionalQuantity.hpp"

namespace openstudio {
namespace model {

namespace detail {

  /** SizingParameters_Impl is a ModelObject_Impl that is the implementation class for SizingParameters.*/
  class MODEL_API SizingParameters_Impl : public ModelObject_Impl {
    Q_OBJECT;

    Q_PROPERTY(double heatingSizingFactor READ heatingSizingFactor WRITE setHeatingSizingFactor RESET resetHeatingSizingFactor);
    Q_PROPERTY(openstudio::Quantity heatingSizingFactor_SI READ heatingSizingFactor_SI WRITE setHeatingSizingFactor RESET resetHeatingSizingFactor);
    Q_PROPERTY(openstudio::Quantity heatingSizingFactor_IP READ heatingSizingFactor_IP WRITE setHeatingSizingFactor RESET resetHeatingSizingFactor);
    Q_PROPERTY(bool isHeatingSizingFactorDefaulted READ isHeatingSizingFactorDefaulted);

    Q_PROPERTY(double coolingSizingFactor READ coolingSizingFactor WRITE setCoolingSizingFactor RESET resetCoolingSizingFactor);
    Q_PROPERTY(openstudio::Quantity coolingSizingFactor_SI READ coolingSizingFactor_SI WRITE setCoolingSizingFactor RESET resetCoolingSizingFactor);
    Q_PROPERTY(openstudio::Quantity coolingSizingFactor_IP READ coolingSizingFactor_IP WRITE setCoolingSizingFactor RESET resetCoolingSizingFactor);
    Q_PROPERTY(bool isCoolingSizingFactorDefaulted READ isCoolingSizingFactorDefaulted);

    Q_PROPERTY(boost::optional<int> timestepsinAveragingWindow READ timestepsinAveragingWindow WRITE setTimestepsinAveragingWindow RESET resetTimestepsinAveragingWindow);

    // TODO: Add relationships for objects related to this one, but not pointed to by the underlying data.
    //       Such relationships can be generated by the GenerateRelationships.rb script.
   public:

    /** @name Constructors and Destructors */
    //@{

    SizingParameters_Impl(const IdfObject& idfObject,
                          Model_Impl* model,
                          bool keepHandle);

    SizingParameters_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                          Model_Impl* model,
                          bool keepHandle);

    SizingParameters_Impl(const SizingParameters_Impl& other,
                          Model_Impl* model,
                          bool keepHandle);

    virtual ~SizingParameters_Impl() {}

    //@}

    /** @name Virtual Methods */
    //@{

    virtual boost::optional<ParentObject> parent() const override;

    virtual const std::vector<std::string>& outputVariableNames() const override;

    virtual IddObjectType iddObjectType() const override;

    //@}
    /** @name Getters */
    //@{

    double heatingSizingFactor() const;

    Quantity getHeatingSizingFactor(bool returnIP=false) const;

    bool isHeatingSizingFactorDefaulted() const;

    double coolingSizingFactor() const;

    Quantity getCoolingSizingFactor(bool returnIP=false) const;

    bool isCoolingSizingFactorDefaulted() const;

    boost::optional<int> timestepsinAveragingWindow() const;

    //@}
    /** @name Setters */
    //@{

    bool setHeatingSizingFactor(double heatingSizingFactor);

    bool setHeatingSizingFactor(const Quantity& heatingSizingFactor);

    void resetHeatingSizingFactor();

    bool setCoolingSizingFactor(double coolingSizingFactor);

    bool setCoolingSizingFactor(const Quantity& coolingSizingFactor);

    void resetCoolingSizingFactor();

    bool setTimestepsinAveragingWindow(boost::optional<int> timestepsinAveragingWindow);

    void resetTimestepsinAveragingWindow();

    //@}
    /** @name Other */
    //@{

    //@}
   protected:
   private:
    REGISTER_LOGGER("openstudio.model.SizingParameters");

    openstudio::Quantity heatingSizingFactor_SI() const;
    openstudio::Quantity heatingSizingFactor_IP() const;
    openstudio::Quantity coolingSizingFactor_SI() const;
    openstudio::Quantity coolingSizingFactor_IP() const;
  };

} // detail

} // model
} // openstudio

#endif // MODEL_SIZINGPARAMETERS_IMPL_HPP

