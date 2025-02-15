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

#ifndef MODEL_TEMPERINGVALVE_IMPL_HPP
#define MODEL_TEMPERINGVALVE_IMPL_HPP

#include <model/ModelAPI.hpp>
#include <model/StraightComponent_Impl.hpp>

namespace openstudio {
namespace model {

class Node;

namespace detail {

  /** TemperingValve_Impl is a StraightComponent_Impl that is the implementation class for TemperingValve.*/
  class MODEL_API TemperingValve_Impl : public StraightComponent_Impl {
   public:
    /** @name Constructors and Destructors */
    //@{

    TemperingValve_Impl(const IdfObject& idfObject,
                        Model_Impl* model,
                        bool keepHandle);

    TemperingValve_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                        Model_Impl* model,
                        bool keepHandle);

    TemperingValve_Impl(const TemperingValve_Impl& other,
                        Model_Impl* model,
                        bool keepHandle);

    virtual ~TemperingValve_Impl() {}

    //@}
    /** @name Virtual Methods */
    //@{

    virtual const std::vector<std::string>& outputVariableNames() const;

    virtual IddObjectType iddObjectType() const;

    //@}
    /** @name Getters */
    //@{

    boost::optional<Node> stream2SourceNode() const;

    boost::optional<Node> temperatureSetpointNode() const;

    boost::optional<Node> pumpOutletNode() const;

    //@}
    /** @name Setters */
    //@{

    bool setStream2SourceNode(const boost::optional<Node> & stream2SourceNode);

    void resetStream2SourceNode();

    bool setTemperatureSetpointNode(const boost::optional<Node> & temperatureSetpointNode);

    void resetTemperatureSetpointNode();

    bool setPumpOutletNode(const boost::optional<Node> & pumpOutletNode);

    void resetPumpOutletNode();

    //@}
    /** @name Other */
    //@{

    unsigned inletPort();

    unsigned outletPort();

    bool addToNode(Node & node);

    // Set pumpOutletNode, temperatureSetpointNode, and stream2SourceNode to reasonable values if they are not set
    void setControlNodes();

    //@}
   protected:
   private:
    REGISTER_LOGGER("openstudio.model.TemperingValve");

    // TODO: Check the return types of these methods.
    // Optional getters for use by methods like children() so can remove() if the constructor fails.
    // There are other ways for the public versions of these getters to fail--perhaps all required
    // objects should be returned as boost::optionals
    boost::optional<Connection> optionalInletNode() const;
    boost::optional<Connection> optionalOutletNode() const;
  };

} // detail

} // model
} // openstudio

#endif // MODEL_TEMPERINGVALVE_IMPL_HPP

