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

#include <model/HeaderedPumpsConstantSpeed.hpp>
#include <model/HeaderedPumpsConstantSpeed_Impl.hpp>
#include <model/Schedule.hpp>
#include <model/Schedule_Impl.hpp>
#include <model/ThermalZone.hpp>
#include <model/ThermalZone_Impl.hpp>
#include <model/PlantLoop.hpp>
#include <model/PlantLoop_Impl.hpp>
#include <model/Node.hpp>
#include <model/Node_Impl.hpp>
#include <model/ScheduleTypeLimits.hpp>
#include <model/ScheduleTypeRegistry.hpp>
#include <utilities/idd/IddFactory.hxx>
#include <utilities/idd/OS_HeaderedPumps_ConstantSpeed_FieldEnums.hxx>
#include <utilities/units/Unit.hpp>
#include <utilities/core/Assert.hpp>

namespace openstudio {
namespace model {

namespace detail {

  HeaderedPumpsConstantSpeed_Impl::HeaderedPumpsConstantSpeed_Impl(const IdfObject& idfObject,
                                                                   Model_Impl* model,
                                                                   bool keepHandle)
    : StraightComponent_Impl(idfObject,model,keepHandle)
  {
    OS_ASSERT(idfObject.iddObject().type() == HeaderedPumpsConstantSpeed::iddObjectType());
  }

  HeaderedPumpsConstantSpeed_Impl::HeaderedPumpsConstantSpeed_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                                                                   Model_Impl* model,
                                                                   bool keepHandle)
    : StraightComponent_Impl(other,model,keepHandle)
  {
    OS_ASSERT(other.iddObject().type() == HeaderedPumpsConstantSpeed::iddObjectType());
  }

  HeaderedPumpsConstantSpeed_Impl::HeaderedPumpsConstantSpeed_Impl(const HeaderedPumpsConstantSpeed_Impl& other,
                                                                   Model_Impl* model,
                                                                   bool keepHandle)
    : StraightComponent_Impl(other,model,keepHandle)
  {}

  const std::vector<std::string>& HeaderedPumpsConstantSpeed_Impl::outputVariableNames() const
  {
    static std::vector<std::string> result;
    if (result.empty()){
    }
    return result;
  }

  IddObjectType HeaderedPumpsConstantSpeed_Impl::iddObjectType() const {
    return HeaderedPumpsConstantSpeed::iddObjectType();
  }

  std::vector<ScheduleTypeKey> HeaderedPumpsConstantSpeed_Impl::getScheduleTypeKeys(const Schedule& schedule) const
  {
    std::vector<ScheduleTypeKey> result;
    UnsignedVector fieldIndices = getSourceIndices(schedule.handle());
    UnsignedVector::const_iterator b(fieldIndices.begin()), e(fieldIndices.end());
    if (std::find(b,e,OS_HeaderedPumps_ConstantSpeedFields::PumpFlowRateSchedule) != e)
    {
      result.push_back(ScheduleTypeKey("HeaderedPumpsConstantSpeed","Pump Flow Rate Schedule"));
    }
    return result;
  }

  boost::optional<double> HeaderedPumpsConstantSpeed_Impl::totalRatedFlowRate() const {
    return getDouble(OS_HeaderedPumps_ConstantSpeedFields::TotalRatedFlowRate,true);
  }

  bool HeaderedPumpsConstantSpeed_Impl::isTotalRatedFlowRateAutosized() const {
    bool result = false;
    boost::optional<std::string> value = getString(OS_HeaderedPumps_ConstantSpeedFields::TotalRatedFlowRate, true);
    if (value) {
      result = openstudio::istringEqual(value.get(), "autosize");
    }
    return result;
  }

  int HeaderedPumpsConstantSpeed_Impl::numberofPumpsinBank() const {
    boost::optional<int> value = getInt(OS_HeaderedPumps_ConstantSpeedFields::NumberofPumpsinBank,true);
    OS_ASSERT(value);
    return value.get();
  }

  std::string HeaderedPumpsConstantSpeed_Impl::flowSequencingControlScheme() const {
    boost::optional<std::string> value = getString(OS_HeaderedPumps_ConstantSpeedFields::FlowSequencingControlScheme,true);
    OS_ASSERT(value);
    return value.get();
  }

  double HeaderedPumpsConstantSpeed_Impl::ratedPumpHead() const {
    boost::optional<double> value = getDouble(OS_HeaderedPumps_ConstantSpeedFields::RatedPumpHead,true);
    OS_ASSERT(value);
    return value.get();
  }

  boost::optional<double> HeaderedPumpsConstantSpeed_Impl::ratedPowerConsumption() const {
    return getDouble(OS_HeaderedPumps_ConstantSpeedFields::RatedPowerConsumption,true);
  }

  bool HeaderedPumpsConstantSpeed_Impl::isRatedPowerConsumptionAutosized() const {
    bool result = false;
    boost::optional<std::string> value = getString(OS_HeaderedPumps_ConstantSpeedFields::RatedPowerConsumption, true);
    if (value) {
      result = openstudio::istringEqual(value.get(), "autosize");
    }
    return result;
  }

  double HeaderedPumpsConstantSpeed_Impl::motorEfficiency() const {
    boost::optional<double> value = getDouble(OS_HeaderedPumps_ConstantSpeedFields::MotorEfficiency,true);
    OS_ASSERT(value);
    return value.get();
  }

  double HeaderedPumpsConstantSpeed_Impl::fractionofMotorInefficienciestoFluidStream() const {
    boost::optional<double> value = getDouble(OS_HeaderedPumps_ConstantSpeedFields::FractionofMotorInefficienciestoFluidStream,true);
    OS_ASSERT(value);
    return value.get();
  }

  std::string HeaderedPumpsConstantSpeed_Impl::pumpControlType() const {
    boost::optional<std::string> value = getString(OS_HeaderedPumps_ConstantSpeedFields::PumpControlType,true);
    OS_ASSERT(value);
    return value.get();
  }

  boost::optional<Schedule> HeaderedPumpsConstantSpeed_Impl::pumpFlowRateSchedule() const {
    return getObject<ModelObject>().getModelObjectTarget<Schedule>(OS_HeaderedPumps_ConstantSpeedFields::PumpFlowRateSchedule);
  }

  boost::optional<ThermalZone> HeaderedPumpsConstantSpeed_Impl::thermalZone() const {
    return getObject<ModelObject>().getModelObjectTarget<ThermalZone>(OS_HeaderedPumps_ConstantSpeedFields::ThermalZone);
  }

  double HeaderedPumpsConstantSpeed_Impl::skinLossRadiativeFraction() const {
    boost::optional<double> value = getDouble(OS_HeaderedPumps_ConstantSpeedFields::SkinLossRadiativeFraction,true);
    OS_ASSERT(value);
    return value.get();
  }

  bool HeaderedPumpsConstantSpeed_Impl::setTotalRatedFlowRate(boost::optional<double> totalRatedFlowRate) {
    bool result(false);
    if (totalRatedFlowRate) {
      result = setDouble(OS_HeaderedPumps_ConstantSpeedFields::TotalRatedFlowRate, totalRatedFlowRate.get());
    }
    return result;
  }

  void HeaderedPumpsConstantSpeed_Impl::autosizeTotalRatedFlowRate() {
    bool result = setString(OS_HeaderedPumps_ConstantSpeedFields::TotalRatedFlowRate, "autosize");
    OS_ASSERT(result);
  }

  void HeaderedPumpsConstantSpeed_Impl::setNumberofPumpsinBank(int numberofPumpsinBank) {
    bool result = setInt(OS_HeaderedPumps_ConstantSpeedFields::NumberofPumpsinBank, numberofPumpsinBank);
    OS_ASSERT(result);
  }

  bool HeaderedPumpsConstantSpeed_Impl::setFlowSequencingControlScheme(std::string flowSequencingControlScheme) {
    bool result = setString(OS_HeaderedPumps_ConstantSpeedFields::FlowSequencingControlScheme, flowSequencingControlScheme);
    return result;
  }

  void HeaderedPumpsConstantSpeed_Impl::setRatedPumpHead(double ratedPumpHead) {
    bool result = setDouble(OS_HeaderedPumps_ConstantSpeedFields::RatedPumpHead, ratedPumpHead);
    OS_ASSERT(result);
  }

  void HeaderedPumpsConstantSpeed_Impl::setRatedPowerConsumption(boost::optional<double> ratedPowerConsumption) {
    bool result(false);
    if (ratedPowerConsumption) {
      result = setDouble(OS_HeaderedPumps_ConstantSpeedFields::RatedPowerConsumption, ratedPowerConsumption.get());
    }
    OS_ASSERT(result);
  }

  void HeaderedPumpsConstantSpeed_Impl::autosizeRatedPowerConsumption() {
    bool result = setString(OS_HeaderedPumps_ConstantSpeedFields::RatedPowerConsumption, "autosize");
    OS_ASSERT(result);
  }

  bool HeaderedPumpsConstantSpeed_Impl::setMotorEfficiency(double motorEfficiency) {
    bool result = setDouble(OS_HeaderedPumps_ConstantSpeedFields::MotorEfficiency, motorEfficiency);
    return result;
  }

  bool HeaderedPumpsConstantSpeed_Impl::setFractionofMotorInefficienciestoFluidStream(double fractionofMotorInefficienciestoFluidStream) {
    bool result = setDouble(OS_HeaderedPumps_ConstantSpeedFields::FractionofMotorInefficienciestoFluidStream, fractionofMotorInefficienciestoFluidStream);
    return result;
  }

  bool HeaderedPumpsConstantSpeed_Impl::setPumpControlType(std::string pumpControlType) {
    bool result = setString(OS_HeaderedPumps_ConstantSpeedFields::PumpControlType, pumpControlType);
    return result;
  }

  bool HeaderedPumpsConstantSpeed_Impl::setPumpFlowRateSchedule(Schedule& schedule) {
    bool result = setSchedule(OS_HeaderedPumps_ConstantSpeedFields::PumpFlowRateSchedule,
                              "HeaderedPumpsConstantSpeed",
                              "Pump Flow Rate Schedule",
                              schedule);
    return result;
  }

  void HeaderedPumpsConstantSpeed_Impl::resetPumpFlowRateSchedule() {
    bool result = setString(OS_HeaderedPumps_ConstantSpeedFields::PumpFlowRateSchedule, "");
    OS_ASSERT(result);
  }

  bool HeaderedPumpsConstantSpeed_Impl::setThermalZone(const boost::optional<ThermalZone>& thermalZone) {
    bool result(false);
    if (thermalZone) {
      result = setPointer(OS_HeaderedPumps_ConstantSpeedFields::ThermalZone, thermalZone.get().handle());
    }
    else {
      resetThermalZone();
      result = true;
    }
    return result;
  }

  void HeaderedPumpsConstantSpeed_Impl::resetThermalZone() {
    bool result = setString(OS_HeaderedPumps_ConstantSpeedFields::ThermalZone, "");
    OS_ASSERT(result);
  }

  bool HeaderedPumpsConstantSpeed_Impl::setSkinLossRadiativeFraction(double skinLossRadiativeFraction) {
    bool result = setDouble(OS_HeaderedPumps_ConstantSpeedFields::SkinLossRadiativeFraction, skinLossRadiativeFraction);
    return result;
  }

  unsigned HeaderedPumpsConstantSpeed_Impl::inletPort() {
    return OS_HeaderedPumps_ConstantSpeedFields::InletNodeName;
  }

  unsigned HeaderedPumpsConstantSpeed_Impl::outletPort() {
    return OS_HeaderedPumps_ConstantSpeedFields::OutletNodeName;
  }

  bool HeaderedPumpsConstantSpeed_Impl::addToNode(Node & node)
  {
    if( boost::optional<PlantLoop> plant = node.plantLoop() ) {
      return StraightComponent_Impl::addToNode(node);
    }

    return false;
  }

} // detail

HeaderedPumpsConstantSpeed::HeaderedPumpsConstantSpeed(const Model& model)
  : StraightComponent(HeaderedPumpsConstantSpeed::iddObjectType(),model)
{
  OS_ASSERT(getImpl<detail::HeaderedPumpsConstantSpeed_Impl>());

  autosizeTotalRatedFlowRate();
  setNumberofPumpsinBank(2);
  setFlowSequencingControlScheme("Sequential");
  setRatedPumpHead(179352);
  autosizeRatedPowerConsumption();
  setMotorEfficiency(0.9); 
  setFractionofMotorInefficienciestoFluidStream(0.0);
  setPumpControlType("Continuous");
  setSkinLossRadiativeFraction(0.1);
}

IddObjectType HeaderedPumpsConstantSpeed::iddObjectType() {
  return IddObjectType(IddObjectType::OS_HeaderedPumps_ConstantSpeed);
}

std::vector<std::string> HeaderedPumpsConstantSpeed::flowSequencingControlSchemeValues() {
  return getIddKeyNames(IddFactory::instance().getObject(iddObjectType()).get(),
                        OS_HeaderedPumps_ConstantSpeedFields::FlowSequencingControlScheme);
}

std::vector<std::string> HeaderedPumpsConstantSpeed::pumpControlTypeValues() {
  return getIddKeyNames(IddFactory::instance().getObject(iddObjectType()).get(),
                        OS_HeaderedPumps_ConstantSpeedFields::PumpControlType);
}

boost::optional<double> HeaderedPumpsConstantSpeed::totalRatedFlowRate() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->totalRatedFlowRate();
}

bool HeaderedPumpsConstantSpeed::isTotalRatedFlowRateAutosized() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->isTotalRatedFlowRateAutosized();
}

int HeaderedPumpsConstantSpeed::numberofPumpsinBank() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->numberofPumpsinBank();
}

std::string HeaderedPumpsConstantSpeed::flowSequencingControlScheme() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->flowSequencingControlScheme();
}

double HeaderedPumpsConstantSpeed::ratedPumpHead() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->ratedPumpHead();
}

boost::optional<double> HeaderedPumpsConstantSpeed::ratedPowerConsumption() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->ratedPowerConsumption();
}

bool HeaderedPumpsConstantSpeed::isRatedPowerConsumptionAutosized() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->isRatedPowerConsumptionAutosized();
}

double HeaderedPumpsConstantSpeed::motorEfficiency() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->motorEfficiency();
}

double HeaderedPumpsConstantSpeed::fractionofMotorInefficienciestoFluidStream() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->fractionofMotorInefficienciestoFluidStream();
}

std::string HeaderedPumpsConstantSpeed::pumpControlType() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->pumpControlType();
}

boost::optional<Schedule> HeaderedPumpsConstantSpeed::pumpFlowRateSchedule() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->pumpFlowRateSchedule();
}

boost::optional<ThermalZone> HeaderedPumpsConstantSpeed::thermalZone() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->thermalZone();
}

double HeaderedPumpsConstantSpeed::skinLossRadiativeFraction() const {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->skinLossRadiativeFraction();
}

bool HeaderedPumpsConstantSpeed::setTotalRatedFlowRate(double totalRatedFlowRate) {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setTotalRatedFlowRate(totalRatedFlowRate);
}

void HeaderedPumpsConstantSpeed::autosizeTotalRatedFlowRate() {
  getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->autosizeTotalRatedFlowRate();
}

void HeaderedPumpsConstantSpeed::setNumberofPumpsinBank(int numberofPumpsinBank) {
  getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setNumberofPumpsinBank(numberofPumpsinBank);
}

bool HeaderedPumpsConstantSpeed::setFlowSequencingControlScheme(std::string flowSequencingControlScheme) {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setFlowSequencingControlScheme(flowSequencingControlScheme);
}

void HeaderedPumpsConstantSpeed::setRatedPumpHead(double ratedPumpHead) {
  getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setRatedPumpHead(ratedPumpHead);
}

void HeaderedPumpsConstantSpeed::setRatedPowerConsumption(double ratedPowerConsumption) {
  getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setRatedPowerConsumption(ratedPowerConsumption);
}

void HeaderedPumpsConstantSpeed::autosizeRatedPowerConsumption() {
  getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->autosizeRatedPowerConsumption();
}

bool HeaderedPumpsConstantSpeed::setMotorEfficiency(double motorEfficiency) {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setMotorEfficiency(motorEfficiency);
}

bool HeaderedPumpsConstantSpeed::setFractionofMotorInefficienciestoFluidStream(double fractionofMotorInefficienciestoFluidStream) {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setFractionofMotorInefficienciestoFluidStream(fractionofMotorInefficienciestoFluidStream);
}

bool HeaderedPumpsConstantSpeed::setPumpControlType(std::string pumpControlType) {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setPumpControlType(pumpControlType);
}

bool HeaderedPumpsConstantSpeed::setPumpFlowRateSchedule(Schedule& schedule) {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setPumpFlowRateSchedule(schedule);
}

void HeaderedPumpsConstantSpeed::resetPumpFlowRateSchedule() {
  getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->resetPumpFlowRateSchedule();
}

bool HeaderedPumpsConstantSpeed::setThermalZone(const ThermalZone& thermalZone) {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setThermalZone(thermalZone);
}

void HeaderedPumpsConstantSpeed::resetThermalZone() {
  getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->resetThermalZone();
}

bool HeaderedPumpsConstantSpeed::setSkinLossRadiativeFraction(double skinLossRadiativeFraction) {
  return getImpl<detail::HeaderedPumpsConstantSpeed_Impl>()->setSkinLossRadiativeFraction(skinLossRadiativeFraction);
}

/// @cond
HeaderedPumpsConstantSpeed::HeaderedPumpsConstantSpeed(std::shared_ptr<detail::HeaderedPumpsConstantSpeed_Impl> impl)
  : StraightComponent(impl)
{}
/// @endcond

} // model
} // openstudio

