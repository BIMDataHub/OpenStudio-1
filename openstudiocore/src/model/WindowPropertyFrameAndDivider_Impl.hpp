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

#ifndef MODEL_WINDOWPROPERTYFRAMEANDDIVIDER_IMPL_HPP
#define MODEL_WINDOWPROPERTYFRAMEANDDIVIDER_IMPL_HPP

#include "ModelAPI.hpp"
#include "ResourceObject_Impl.hpp"

namespace openstudio {
namespace model {

namespace detail {

  /** WindowPropertyFrameAndDivider_Impl is a ResourceObject_Impl that is the implementation class for WindowPropertyFrameAndDivider.*/
  class MODEL_API WindowPropertyFrameAndDivider_Impl : public ResourceObject_Impl {
   public:
    /** @name Constructors and Destructors */
    //@{

    WindowPropertyFrameAndDivider_Impl(const IdfObject& idfObject,
                                       Model_Impl* model,
                                       bool keepHandle);

    WindowPropertyFrameAndDivider_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                                       Model_Impl* model,
                                       bool keepHandle);

    WindowPropertyFrameAndDivider_Impl(const WindowPropertyFrameAndDivider_Impl& other,
                                       Model_Impl* model,
                                       bool keepHandle);

    virtual ~WindowPropertyFrameAndDivider_Impl() {}

    //@}
    /** @name Virtual Methods */
    //@{

    virtual const std::vector<std::string>& outputVariableNames() const override;

    virtual IddObjectType iddObjectType() const override;

    //@}
    /** @name Getters */
    //@{

    double frameWidth() const;

    bool isFrameWidthDefaulted() const;

    double frameOutsideProjection() const;

    bool isFrameOutsideProjectionDefaulted() const;

    double frameInsideProjection() const;

    bool isFrameInsideProjectionDefaulted() const;

    boost::optional<double> frameConductance() const;

    double ratioOfFrameEdgeGlassConductanceToCenterOfGlassConductance() const;

    bool isRatioOfFrameEdgeGlassConductanceToCenterOfGlassConductanceDefaulted() const;

    double frameSolarAbsorptance() const;

    bool isFrameSolarAbsorptanceDefaulted() const;

    double frameVisibleAbsorptance() const;

    bool isFrameVisibleAbsorptanceDefaulted() const;

    double frameThermalHemisphericalEmissivity() const;

    bool isFrameThermalHemisphericalEmissivityDefaulted() const;

    std::string dividerType() const;

    bool isDividerTypeDefaulted() const;

    double dividerWidth() const;

    bool isDividerWidthDefaulted() const;

    double numberOfHorizontalDividers() const;

    bool isNumberOfHorizontalDividersDefaulted() const;

    double numberOfVerticalDividers() const;

    bool isNumberOfVerticalDividersDefaulted() const;

    double dividerOutsideProjection() const;

    bool isDividerOutsideProjectionDefaulted() const;

    double dividerInsideProjection() const;

    bool isDividerInsideProjectionDefaulted() const;

    double dividerConductance() const;

    bool isDividerConductanceDefaulted() const;

    double ratioOfDividerEdgeGlassConductanceToCenterOfGlassConductance() const;

    bool isRatioOfDividerEdgeGlassConductanceToCenterOfGlassConductanceDefaulted() const;

    double dividerSolarAbsorptance() const;

    bool isDividerSolarAbsorptanceDefaulted() const;

    double dividerVisibleAbsorptance() const;

    bool isDividerVisibleAbsorptanceDefaulted() const;

    double dividerThermalHemisphericalEmissivity() const;

    bool isDividerThermalHemisphericalEmissivityDefaulted() const;

    double outsideRevealDepth() const;

    bool isOutsideRevealDepthDefaulted() const;

    double outsideRevealSolarAbsorptance() const;

    bool isOutsideRevealSolarAbsorptanceDefaulted() const;

    double insideSillDepth() const;

    bool isInsideSillDepthDefaulted() const;

    double insideSillSolarAbsorptance() const;

    bool isInsideSillSolarAbsorptanceDefaulted() const;

    double insideRevealDepth() const;

    bool isInsideRevealDepthDefaulted() const;

    double insideRevealSolarAbsorptance() const;

    bool isInsideRevealSolarAbsorptanceDefaulted() const;

    //@}
    /** @name Setters */
    //@{

    bool setFrameWidth(double frameWidth);

    void resetFrameWidth();

    bool setFrameOutsideProjection(double frameOutsideProjection);

    void resetFrameOutsideProjection();

    bool setFrameInsideProjection(double frameInsideProjection);

    void resetFrameInsideProjection();

    bool setFrameConductance(boost::optional<double> frameConductance);

    void resetFrameConductance();

    bool setRatioOfFrameEdgeGlassConductanceToCenterOfGlassConductance(double ratioOfFrameEdgeGlassConductanceToCenterOfGlassConductance);

    void resetRatioOfFrameEdgeGlassConductanceToCenterOfGlassConductance();

    bool setFrameSolarAbsorptance(double frameSolarAbsorptance);

    void resetFrameSolarAbsorptance();

    bool setFrameVisibleAbsorptance(double frameVisibleAbsorptance);

    void resetFrameVisibleAbsorptance();

    bool setFrameThermalHemisphericalEmissivity(double frameThermalHemisphericalEmissivity);

    void resetFrameThermalHemisphericalEmissivity();

    bool setDividerType(std::string dividerType);

    void resetDividerType();

    bool setDividerWidth(double dividerWidth);

    void resetDividerWidth();

    bool setNumberOfHorizontalDividers(double numberOfHorizontalDividers);

    void resetNumberOfHorizontalDividers();

    bool setNumberOfVerticalDividers(double numberOfVerticalDividers);

    void resetNumberOfVerticalDividers();

    bool setDividerOutsideProjection(double dividerOutsideProjection);

    void resetDividerOutsideProjection();

    bool setDividerInsideProjection(double dividerInsideProjection);

    void resetDividerInsideProjection();

    bool setDividerConductance(double dividerConductance);

    void resetDividerConductance();

    bool setRatioOfDividerEdgeGlassConductanceToCenterOfGlassConductance(double ratioOfDividerEdgeGlassConductanceToCenterOfGlassConductance);

    void resetRatioOfDividerEdgeGlassConductanceToCenterOfGlassConductance();

    bool setDividerSolarAbsorptance(double dividerSolarAbsorptance);

    void resetDividerSolarAbsorptance();

    bool setDividerVisibleAbsorptance(double dividerVisibleAbsorptance);

    void resetDividerVisibleAbsorptance();

    bool setDividerThermalHemisphericalEmissivity(double dividerThermalHemisphericalEmissivity);

    void resetDividerThermalHemisphericalEmissivity();

    bool setOutsideRevealDepth(double outsideRevealDepth);

    void resetOutsideRevealDepth();

    bool setOutsideRevealSolarAbsorptance(double outsideRevealSolarAbsorptance);

    void resetOutsideRevealSolarAbsorptance();

    bool setInsideSillDepth(double insideSillDepth);

    void resetInsideSillDepth();

    bool setInsideSillSolarAbsorptance(double insideSillSolarAbsorptance);

    void resetInsideSillSolarAbsorptance();

    bool setInsideRevealDepth(double insideRevealDepth);

    void resetInsideRevealDepth();

    bool setInsideRevealSolarAbsorptance(double insideRevealSolarAbsorptance);

    void resetInsideRevealSolarAbsorptance();

    //@}
    /** @name Other */
    //@{

    //@}
   protected:
   private:
    REGISTER_LOGGER("openstudio.model.WindowPropertyFrameAndDivider");
  };

} // detail

} // model
} // openstudio

#endif // MODEL_WINDOWPROPERTYFRAMEANDDIVIDER_IMPL_HPP

