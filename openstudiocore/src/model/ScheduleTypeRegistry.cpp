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

#include "ScheduleTypeRegistry.hpp"
#include "ScheduleTypeLimits.hpp"
#include "ScheduleTypeLimits_Impl.hpp"
#include "Schedule.hpp"
#include "Schedule_Impl.hpp"
#include "Model.hpp"

#include "../utilities/units/Quantity.hpp"
#include "../utilities/units/OSOptionalQuantity.hpp"

#include "../utilities/core/Containers.hpp"

namespace openstudio {
namespace model {

OSOptionalQuantity ScheduleType::getLowerLimitValue(bool returnIP) const {
  if (OptionalUnit u = ScheduleTypeLimits::units(unitType,returnIP)) {
    if (lowerLimitValue) {
      return OSOptionalQuantity(Quantity(*lowerLimitValue,*u));
    }
    return OSOptionalQuantity(*u);
  }
  LOG_FREE_AND_THROW("openstudio.model.ScheduleTypeRegistry",
                     "Cannot return any units for unitType " << unitType << ".");
  return OSOptionalQuantity(UnitSystem(UnitSystem::SI));
}

OSOptionalQuantity ScheduleType::getUpperLimitValue(bool returnIP) const {
  if (OptionalUnit u = ScheduleTypeLimits::units(unitType,returnIP)) {
    if (upperLimitValue) {
      return OSOptionalQuantity(Quantity(*upperLimitValue,*u));
    }
    return OSOptionalQuantity(*u);
  }
  LOG_FREE_AND_THROW("openstudio.model.ScheduleTypeRegistry",
                     "Cannot return any units for unitType " << unitType << ".");
  return OSOptionalQuantity(UnitSystem(UnitSystem::SI));
}

std::vector<std::string> ScheduleTypeRegistrySingleton::classNames() const {
  StringVector result;
  for (const ClassNameToScheduleTypesMap::value_type& p : m_classNameToScheduleTypesMap) {
    result.push_back(p.first);
  }
  return result;
}

std::vector<ScheduleType> ScheduleTypeRegistrySingleton::getScheduleTypesByClassName(
    const std::string& className) const
{
  ScheduleTypeVector result;
  auto it = m_classNameToScheduleTypesMap.find(className);
  if (it != m_classNameToScheduleTypesMap.end()) {
    result = it->second;
  }
  return result;
}

ScheduleType ScheduleTypeRegistrySingleton::getScheduleType(const std::string &className,
                                                            const std::string &scheduleDisplayName) const
{
  ScheduleTypeVector scheduleTypes = getScheduleTypesByClassName(className);
  for (const ScheduleType& scheduleType : scheduleTypes) {
    if (scheduleType.scheduleDisplayName == scheduleDisplayName) {
      return scheduleType;
    }
  }
  LOG_AND_THROW("No '" << scheduleDisplayName << "' Schedule is registered for class "
                << className << ".");
  return ScheduleType();
}

ScheduleTypeLimits ScheduleTypeRegistrySingleton::getOrCreateScheduleTypeLimits(const ScheduleType& scheduleType,
                                                                                Model& model) const
{
  std::string defaultName = getDefaultName(scheduleType);

  // DLM: I do not understand why both upper and lower limit have to be set to reuse this?
  //// if fully specified, try to retrieve
  //if (scheduleType.lowerLimitValue && scheduleType.upperLimitValue) {
  ScheduleTypeLimitsVector candidates = model.getConcreteModelObjectsByName<ScheduleTypeLimits>(defaultName);
    for (const ScheduleTypeLimits& candidate : candidates) {
      if (isCompatible(scheduleType,candidate)) {
        return candidate;
      }
    }
  //}

  // otherwise, or if not there, create and return
  ScheduleTypeLimits scheduleTypeLimits(model);

  scheduleTypeLimits.setName(defaultName);

  if (scheduleType.lowerLimitValue) {
    scheduleTypeLimits.setLowerLimitValue(scheduleType.lowerLimitValue.get());
  }
  if (scheduleType.upperLimitValue) {
    scheduleTypeLimits.setUpperLimitValue(scheduleType.upperLimitValue.get());
  }
  if (scheduleType.isContinuous) {
    scheduleTypeLimits.setNumericType("Continuous");
  }
  else {
    scheduleTypeLimits.setNumericType("Discrete");
  }
  if (!scheduleType.unitType.empty()) {
    scheduleTypeLimits.setUnitType(scheduleType.unitType);
  }

  return scheduleTypeLimits;
}

ScheduleTypeRegistrySingleton::ScheduleTypeRegistrySingleton()
{
  // className, scheduleDisplayName, scheduleRelationshipName, isContinuous, unitType, lowerLimitValue, upperLimitValue;

  const ScheduleType scheduleTypes[] =
  {
    {"AirConditionerVariableRefrigerantFlow","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirConditionerVariableRefrigerantFlow","Thermostat Priority Schedule","thermostatPrioritySchedule",false,"ControlMode",0.0,1.0},
    {"AirConditionerVariableRefrigerantFlow","Basin Heater Operating Schedule","basinHeaterOperatingSchedule",false,"Availability",0.0,1.0},
    {"AirLoopHVACUnitaryHeatCoolVAVChangeoverBypass","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirLoopHVACUnitaryHeatCoolVAVChangeoverBypass","Outdoor Air Flow Rate Multiplier Schedule","outdoorAirFlowRateMultiplierSchedule",true,"",0.0,1.0},
    {"AirLoopHVACUnitaryHeatCoolVAVChangeoverBypass","Supply Air Fan Operating Mode Schedule","supplyAirFanOperatingModeSchedule",false,"ControlMode",0.0,1.0},
    {"AirLoopHVACUnitaryHeatPumpAirToAir","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirLoopHVACUnitaryHeatPumpAirToAir","Supply Air Fan Operating Mode","supplyAirFanOperatingModeSchedule",false,"ControlMode",0.0,1.0},
    {"AirLoopHVACUnitaryHeatPumpAirToAirMultiSpeed","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirLoopHVACUnitaryHeatPumpAirToAirMultiSpeed","Supply Air Fan Operating Mode Schedule","supplyAirFanOperatingModeSchedule",false,"ControlMode",0.0,1.0},
    {"AirLoopHVACUnitarySystem","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirLoopHVACUnitarySystem","Supply Air Fan Operating Mode","supplyAirFanOperatingModeSchedule",false,"ControlMode",0.0,1.0},
    {"AirTerminalSingleDuctConstantVolumeCooledBeam","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirTerminalSingleDuctConstantVolumeFourPipeInduction","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirTerminalSingleDuctConstantVolumeReheat","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirTerminalSingleDuctParallelPIUReheat","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirTerminalSingleDuctSeriesPIUReheat","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirTerminalSingleDuctUncontrolled","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirTerminalSingleDuctVAVReheat","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirTerminalSingleDuctVAVReheat","Minimum Air Flow Fraction","minimumAirFlowFractionSchedule",true,"",0.0,1.0},
    {"AirTerminalSingleDuctVAVNoReheat","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirTerminalSingleDuctVAVNoReheat","Minimum Air Flow Fraction","minimumAirFlowFractionSchedule",true,"",0.0,1.0},
    {"AirTerminalSingleDuctVAVHeatAndCoolNoReheat","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AirTerminalSingleDuctVAVHeatAndCoolReheat","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"AvailabilityManagerScheduled","Availability","schedule",false,"Availability",0.0,1.0},
    {"ChillerElectricEIR","Basin Heater","basinHeaterSchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingCooledBeam","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingDXSingleSpeed","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingDXSingleSpeed","Basin Heater Operation","basinHeaterOperatingSchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingDXTwoSpeed","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingDXTwoSpeed","Basin Heater Operation","basinHeaterOperatingSchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingDXTwoStageWithHumidityControlMode","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingDXTwoStageWithHumidityControlMode","Basin Heater Operating Schedule","basinHeaterOperatingSchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingDXMultiSpeed","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingDXMultiSpeed","Basin Heater Operating Schedule","basinHeaterOperatingSchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingDXVariableRefrigerantFlow","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilCoolingLowTempRadiantConstFlow","Cooling High Water Temperature","coolingHighWaterTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"CoilCoolingLowTempRadiantConstFlow","Cooling Low Water Temperature","coolingLowWaterTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"CoilCoolingLowTempRadiantConstFlow","Cooling High Control Temperature","coolingHighControlTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"CoilCoolingLowTempRadiantConstFlow","Cooling Low Control Temperature","coolingLowControlTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"CoilCoolingLowTempRadiantVarFlow","Cooling Control Temperature Schedule","coolingControlTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"CoilCoolingWater","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilHeatingDesuperheater","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilHeatingDXSingleSpeed","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilHeatingDXVariableRefrigerantFlow","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilHeatingElectric","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilHeatingGas","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilHeatingGasMultiStage","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilHeatingLowTempRadiantConstFlow","Heating High Water Temperature","heatingHighWaterTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"CoilHeatingLowTempRadiantConstFlow","Heating Low Water Temperature","heatingLowWaterTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"CoilHeatingLowTempRadiantConstFlow","Heating High Control Temperature","heatingHighControlTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"CoilHeatingLowTempRadiantConstFlow","Heating Low Control Temperature","heatingLowControlTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"CoilHeatingLowTempRadiantVarFlow","Heating Control Temperature","heatingControlTemperature",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"CoilHeatingWater","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilHeatingWaterBaseboard","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilWaterHeatingDesuperheater","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoilWaterHeatingDesuperheater","Setpoint Temperature","setpointTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"ControllerOutdoorAir","Minimum Outdoor Air","minimumOutdoorAirSchedule",true,"",0.0,1.0},
    {"ControllerOutdoorAir","Minimum Fraction of Outdoor Air","minimumFractionofOutdoorAirSchedule",true,"",0.0,1.0},
    {"ControllerOutdoorAir","Maximum Fraction of Outdoor Air","maximumFractionofOutdoorAirSchedule",true,"",0.0,1.0},
    {"ControllerOutdoorAir","Time of Day Economizer Control","timeofDayEconomizerControlSchedule",false,"Availability",0.0,1.0},
    {"ControllerMechanicalVentilation","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"CoolingTowerSingleSpeed","Basin Heater Operating","basinHeaterOperatingSchedule",false,"Availability",0.0,1.0},
    {"CoolingTowerSingleSpeed","Blowdown Makeup Water Usage","blowdownMakeupWaterUsageSchedule",true,"VolumetricFlowRate",0.0,OptionalDouble()},
    {"CoolingTowerTwoSpeed","Basin Heater Operating","basinHeaterOperatingSchedule",false,"Availability",0.0,1.0},
    {"CoolingTowerTwoSpeed","Blowdown Makeup Water Usage","blowdownMakeupWaterUsageSchedule",true,"VolumetricFlowRate",0.0,OptionalDouble()},
    {"CoolingTowerVariableSpeed","Basin Heater Operating Schedule","basinHeaterOperatingSchedule",false,"Availability",0.0,1.0},
    {"CoolingTowerVariableSpeed","Blowdown Makeup Water Usage Schedule","blowdownMakeupWaterUsageSchedule",true,"VolumetricFlowRate",0.0,OptionalDouble()},
    {"DefaultScheduleSet","Hours of Operation","hoursofOperationSchedule",false,"Availability",0.0,1.0},
    {"DefaultScheduleSet","Number of People","numberofPeopleSchedule",true,"",0.0,1.0},
    {"DefaultScheduleSet","People Activity Level","peopleActivityLevelSchedule",true,"ActivityLevel",0.0,OptionalDouble()},
    {"DefaultScheduleSet","Lighting","lightingSchedule",true,"",0.0,1.0},
    {"DefaultScheduleSet","Electric Equipment","electricEquipmentSchedule",true,"",0.0,1.0},
    {"DefaultScheduleSet","Gas Equipment","gasEquipmentSchedule",true,"",0.0,1.0},
    {"DefaultScheduleSet","Hot Water Equipment","hotWaterEquipmentSchedule",true,"",0.0,1.0},
    {"DefaultScheduleSet","Infiltration","infiltrationSchedule",true,"",0.0,1.0},
    {"DefaultScheduleSet","Steam Equipment","steamEquipmentSchedule",true,"",0.0,1.0},
    {"DefaultScheduleSet","Other Equipment","otherEquipmentSchedule",true,"",0.0,1.0},
    {"DesignSpecificationOutdoorAir","Outdoor Air Flow Rate","outdoorAirFlowRateFractionSchedule",true,"",0.0,1.0},
    {"ElectricEquipment","Electric Equipment","schedule",true,"",0.0,1.0},
    {"EvaporativeCoolerDirectResearchSpecial","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"EvaporativeCoolerIndirectResearchSpecial","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"EvaporativeFluidCoolerSingleSpeed","Blowdown Makeup Water Usage","blowdownMakeupWaterUsageSchedule",true,"VolumetricFlowRate",0.0,OptionalDouble()},
    {"ExteriorLights","Exterior Lights","schedule",true,"",0.0,1.0},
    {"FanConstantVolume","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"FanOnOff","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"FanVariableVolume","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"FanZoneExhaust","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"FanZoneExhaust","Flow Fraction","flowFractionSchedule",true,"Dimensionless",0.0,1.0},
    {"FanZoneExhaust","Minimum Zone Temperature Limit","minimumZoneTemperatureLimitSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"FanZoneExhaust","Balanced Exhaust Fraction","balancedExhaustFractionSchedule",true,"Dimensionless",0.0,1.0},    
    {"GasEquipment","Gas Equipment","schedule",true,"",0.0,1.0},
    {"HeatExchangerAirToAirSensibleAndLatent","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"HeatExchangerFluidToFluid","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"HumidifierSteamElectric","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"HotWaterEquipment","Hot Water Equipment","schedule",true,"",0.0,1.0},
    {"Lights","Lighting","schedule",true,"",0.0,1.0},
    {"Luminaire","Luminaire","schedule",true,"",0.0,1.0},
    {"OtherEquipment","Other Equipment","schedule",true,"",0.0,1.0},
    {"OutputVariable","Active Hours","schedule",false,"Availability",0.0,1.0},
    {"People","Number of People","numberofPeopleSchedule",true,"",0.0,1.0},
    {"People","Activity Level","activityLevelSchedule",true,"ActivityLevel",0.0,OptionalDouble()},
    {"People","Work Efficiency","workEfficiencySchedule",true,"",0.0,1.0},
    {"People","Clothing Insulation","clothingInsulationSchedule",true,"ClothingInsulation",0.0,OptionalDouble()},
    {"People","Air Velocity","airVelocitySchedule",true,"Velocity",0.0,OptionalDouble()},
    {"PumpConstantSpeed","Pump Flow Rate","pumpFlowRateSchedule",true,"",0.0,1.0},
    {"PumpVariableSpeed","Pump Flow Rate","pumpFlowRateSchedule",true,"",0.0,1.0},
    {"PumpVariableSpeed","Pump RPM","pumpRPMSchedule",true,"RotationsPerMinute",OptionalDouble(),OptionalDouble()},
    {"PumpVariableSpeed","Minimum Pressure","minimumPressureSchedule",true,"Pressure",OptionalDouble(),OptionalDouble()},
    {"PumpVariableSpeed","Maximum Pressure","maximumPressureSchedule",true,"Pressure",OptionalDouble(),OptionalDouble()},
    {"PumpVariableSpeed","Minimum RPM","minimumRPMSchedule",true,"RotationsPerMinute",OptionalDouble(),OptionalDouble()},
    {"PumpVariableSpeed","Maximum RPM","maximumRPMSchedule",true,"RotationsPerMinute",OptionalDouble(),OptionalDouble()},
    {"HeaderedPumpsConstantSpeed","Pump Flow Rate Schedule","pumpFlowRateSchedule",true,"",0.0,1.0},
    {"HeaderedPumpsVariableSpeed","Pump Flow Rate Schedule","pumpFlowRateSchedule",true,"",0.0,1.0},
    {"RefrigerationCase","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"RefrigerationCase","Case Lighting","caseLightingSchedule",true,"",0.0,1.0},
    {"RefrigerationCase","Case Defrost","caseDefrostSchedule",true,"",0.0,1.0},
    {"RefrigerationCase","Case Defrost Drip-Down","caseDefrostDripDownSchedule",true,"",0.0,1.0},
    {"RefrigerationCase","Refrigerated Case Restocking","refrigeratedCaseRestockingSchedule",true,"LinearPowerDensity",0.0,OptionalDouble()},
    {"RefrigerationCase","Case Credit Fraction","caseCreditFractionSchedule",true,"",0.0,1.0,},
    {"RefrigerationCondenserEvaporativeCooled","Evaporative Condenser Availability","evaporativeCondenserAvailabilitySchedule",false,"Availability",0.0,1.0},
    {"RefrigerationCondenserWaterCooled","Water Outlet Temperature","waterOutletTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"RefrigerationWalkIn","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"RefrigerationWalkIn","Heating Power","heatingPowerSchedule",true,"",0.0,1.0},
    {"RefrigerationWalkIn","Lighting","lightingSchedule",true,"",0.0,1.0},
    {"RefrigerationWalkIn","Defrost","defrostSchedule",true,"",0.0,1.0},
    {"RefrigerationWalkIn","Defrost Drip-Down","defrostDripDownSchedule",true,"",0.0,1.0},
    {"RefrigerationWalkIn","Restocking","restockingSchedule",true,"Capacity",0.0,OptionalDouble()},
    {"RefrigerationWalkInZoneBoundary","Glass Reach In Door Opening  Facing Zone","glassReachInDoorOpeningScheduleFacingZone",true,"",0.0,1.0},
    {"RefrigerationWalkInZoneBoundary","Stocking Door Opening  Facing Zone","stockingDoorOpeningScheduleFacingZone",true,"",0.0,1.0},
    {"SetpointManagerScheduled","(Exact, Min, Max) Temperature","schedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"SetpointManagerScheduled","(Exact, Min, Max) Humidity Ratio","schedule",true,"",0.0,OptionalDouble()},
    {"SetpointManagerScheduled","(Exact, Min, Max) Mass Flow Rate","schedule",true,"MassFlowRate",OptionalDouble(),OptionalDouble()},
    {"SetpointManagerScheduledDualSetpoint","High Setpoint","highSetpointSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"SetpointManagerScheduledDualSetpoint","Low Setpoint","lowSetpointSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"SetpointManagerOutdoorAirReset","Setpoint Manager Outdoor Air Reset","schedule",true,"",0.0,OptionalDouble()},
    {"ShadingSurface","Transmittance","transmittanceSchedule",true,"",0.0,1.0},
    {"SiteWaterMainsTemperature","Temperature","temperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"SpaceInfiltrationDesignFlowRate","Infiltration","schedule",true,"",0.0,1.0},
    {"SpaceInfiltrationEffectiveLeakageArea","Infiltration","schedule",true,"",0.0,1.0},
    {"SteamEquipment","Steam Equipment","schedule",true,"",0.0,1.0},
    {"ThermalStorageIceDetailed","Availability Schedule","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ThermostatSetpointDualSetpoint","Heating Setpoint Temperature","heatingSetpointTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"ThermostatSetpointDualSetpoint","Cooling Setpoint Temperature","coolingSetpointTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"ZoneControlThermostatStagedDualSetpoint","Heating Temperature Setpoint Schedule","heatingTemperatureSetpointSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"ZoneControlThermostatStagedDualSetpoint","Cooling Temperature Setpoint Base Schedule","coolingTemperatureSetpointBaseSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"WaterHeaterMixed","Setpoint Temperature","setpointTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"WaterHeaterMixed","Ambient Temperature","ambientTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"WaterHeaterMixed","Use Flow Rate Fraction","useFlowRateFractionSchedule",true,"",0.0,1.0},
    {"WaterHeaterMixed","Cold Water Supply Temperature","coldWaterSupplyTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"WaterUseConnections","Hot Water Supply Temperature","hotWaterSupplyTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"WaterUseConnections","Cold Water Supply Temperature","coldWaterSupplyTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"WaterUseEquipment","Flow Rate Fraction","flowRateFractionSchedule",true,"",0.0,1.0},
    {"WaterUseEquipment","Target Temperature","targetTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"WaterUseEquipment","Hot Water Supply Temperature","hotWaterSupplyTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"WaterUseEquipment","Cold Water Supply Temperature","coldWaterSupplyTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"WaterUseEquipment","Sensible Fraction","sensibleFractionSchedule",true,"",0.0,1.0},
    {"WaterUseEquipment","Latent Fraction","latentFractionSchedule",true,"",0.0,1.0},
    {"ZoneControlHumidistat","Humidifying Relative Humidity Setpoint","humidifyingRelativeHumiditySetpointSchedule",true,"Percent",0.0,100.0},
    {"ZoneControlHumidistat","Dehumidifying Relative Humidity Setpoint","dehumidifyingRelativeHumiditySetpointSchedule",true,"Percent",0.0,100.0},
    {"ZoneHVACBaseboardConvectiveElectric","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACBaseboardConvectiveWater","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACIdealLoadsAirSystem","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACIdealLoadsAirSystem","Heating Availability","heatingAvailabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACIdealLoadsAirSystem","Cooling Availability","coolingAvailabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACFourPipeFanCoil","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACFourPipeFanCoil","Outdoor Air","outdoorAirSchedule",true,"",0.0,1.0},
    {"ZoneHVACLowTemperatureRadiantElectric","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACLowTemperatureRadiantElectric","Heating Setpoint Temperature","heatingSetpointTemperature",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"ZoneHVACLowTempRadiantConstFlow","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACLowTempRadiantConstFlow","Pump Flow Rate","pumpFlowRateSchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACLowTempRadiantVarFlow","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACPackagedTerminalAirConditioner","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACPackagedTerminalAirConditioner","Supply Air Fan Operating Mode","supplyAirFanOperatingModeSchedule",false,"ControlMode",0.0,1.0},
    {"ZoneHVACPackagedTerminalHeatPump","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACPackagedTerminalHeatPump","Supply Air Fan Operating Mode","supplyAirFanOperatingModeSchedule",false,"ControlMode",0.0,1.0},
    {"RefrigerationAirChiller","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"RefrigerationAirChiller","Heating Power","heatingPowerSchedule",true,"",0.0,1.0},
    {"RefrigerationAirChiller","Defrost","defrostSchedule",true,"",0.0,1.0},
    {"RefrigerationAirChiller","Defrost Drip-Down","defrostDripDownSchedule",true,"",0.0,1.0},
    {"ZoneHVACTerminalUnitVariableRefrigerantFlow","Terminal Unit Availability schedule","terminalUnitAvailabilityschedule",false,"Availability",0.0,1.0},
    {"ZoneHVACTerminalUnitVariableRefrigerantFlow","Supply Air Fan Operating Mode Schedule","supplyAirFanOperatingModeSchedule",false,"ControlMode",0.0,1.0},
    {"ZoneHVACHighTemperatureRadiant","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACHighTemperatureRadiant","Heating Setpoint Temperature","heatingSetpointTemperatureSchedule",true,"Temperature",OptionalDouble(),OptionalDouble()},
    {"ZoneHVACWaterToAirHeatPump","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"ZoneHVACWaterToAirHeatPump","Supply Air Fan Operating Mode","supplyAirFanOperatingModeSchedule",false,"ControlMode",0.0,1.0},
    {"ZoneHVACUnitHeater","Availability","availabilitySchedule",false,"Availability",0.0,1.0},
    {"","","",true,"",OptionalDouble(),OptionalDouble()}
  };

  int i(0);
  while (!scheduleTypes[i].className.empty()) {
    auto it = m_classNameToScheduleTypesMap.find(scheduleTypes[i].className);
    if (it == m_classNameToScheduleTypesMap.end()) {
      m_classNameToScheduleTypesMap.insert(ClassNameToScheduleTypesMap::value_type(scheduleTypes[i].className,ScheduleTypeVector(1u,scheduleTypes[i])));
    }
    else {
      it->second.push_back(scheduleTypes[i]);
    }
    ++i;
  }
}

std::string ScheduleTypeRegistrySingleton::getDefaultName(const ScheduleType& scheduleType) const
{
  std::string result = scheduleType.unitType;
  if (result.empty()) {
    if (scheduleType.isContinuous) {
      if (scheduleType.lowerLimitValue && (scheduleType.lowerLimitValue == 0.0) &&
          scheduleType.upperLimitValue && (scheduleType.upperLimitValue == 1.0))
      {
        result = "Fractional";
      }
      else {
        result = "Dimensionless";
      }
    }
    else {
      if (scheduleType.lowerLimitValue && (scheduleType.lowerLimitValue == 0.0) &&
          scheduleType.upperLimitValue && (scheduleType.upperLimitValue == 1.0))
      {
        result = "Binary";
      }
      else {
        result = "Integer";
      }
    }
  }
  else if (result == "Availability") {
    result = "OnOff";
  }
  return result;
}

bool isCompatible(const std::string& className,
                  const std::string& scheduleDisplayName,
                  const ScheduleTypeLimits& candidate)
{
  ScheduleType scheduleType = ScheduleTypeRegistry::instance().getScheduleType(className,scheduleDisplayName);
  return isCompatible(scheduleType,candidate);
}

bool isCompatible(const ScheduleType& scheduleType,
                  const ScheduleTypeLimits& candidate)
{
  // do not check continuous/discrete

  // check unit type
  std::string unitType = candidate.unitType();
  if (ScheduleTypeLimits::units(scheduleType.unitType,false) != ScheduleTypeLimits::units(unitType,false)) {
    return false;
  }

  // check lower limit
  if (scheduleType.lowerLimitValue) {
    if (!candidate.lowerLimitValue() ||
        (candidate.lowerLimitValue().get() < scheduleType.lowerLimitValue.get()))
    {
      return false;
    }
  }

  // check upper limit
  if (scheduleType.upperLimitValue) {
    if (!candidate.upperLimitValue() ||
        (candidate.upperLimitValue().get() > scheduleType.upperLimitValue.get()))
    {
      return false;
    }
  }

  // passed all checks
  return true;
}

bool checkOrAssignScheduleTypeLimits(const std::string& className,
                                     const std::string& scheduleDisplayName,
                                     Schedule& schedule)
{
  ScheduleType scheduleType = ScheduleTypeRegistry::instance().getScheduleType(
      className,
      scheduleDisplayName);
  bool result(true);
  if (OptionalScheduleTypeLimits scheduleTypeLimits = schedule.scheduleTypeLimits()) {
    if (!isCompatible(scheduleType,*scheduleTypeLimits)) {
      result = false;
    }
  }
  else {
    Model model = schedule.model();
    ScheduleTypeLimits candidate = ScheduleTypeRegistry::instance().getOrCreateScheduleTypeLimits(
        scheduleType,
        model);
    result = schedule.setScheduleTypeLimits(candidate);
    if (!result && (candidate.directUseCount(false) == 0)) {
      candidate.remove();
    }
  }
  return result;
}

std::vector<ScheduleTypeLimits> getCompatibleScheduleTypeLimits(const Model& model,
                                                                const std::string& className,
                                                                const std::string& scheduleDisplayName)
{
  ScheduleTypeLimitsVector result;
  ScheduleTypeLimitsVector candidates = model.getConcreteModelObjects<ScheduleTypeLimits>();
  ScheduleType scheduleType = ScheduleTypeRegistry::instance().getScheduleType(className,scheduleDisplayName);
  for (const ScheduleTypeLimits& candidate : candidates) {
    if (isCompatible(scheduleType,candidate)) {
      result.push_back(candidate);
    }
  }
  return result;
}

std::vector<Schedule> getCompatibleSchedules(const Model& model,
                                             const std::string& className,
                                             const std::string& scheduleDisplayName)
{
  ScheduleVector result;
  ScheduleVector candidates = model.getModelObjects<Schedule>();
  ScheduleTypeLimitsVector okTypes = getCompatibleScheduleTypeLimits(model,className,scheduleDisplayName);
  for (const Schedule& candidate : candidates) {
    if (OptionalScheduleTypeLimits candidateType = candidate.scheduleTypeLimits()) {
      if (std::find(okTypes.begin(),okTypes.end(),*candidateType) != okTypes.end()) {
        result.push_back(candidate);
      }
    }
    else {
      // by default, keep all non-typed schedules
      result.push_back(candidate);
    }
  }
  return result;
}

} // model
} // openstudio
