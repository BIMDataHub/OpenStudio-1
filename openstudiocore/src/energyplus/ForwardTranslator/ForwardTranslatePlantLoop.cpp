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

#include "../ForwardTranslator.hpp"

#include "../../model/Model.hpp"
#include "../../model/PlantLoop.hpp"
#include "../../model/PlantLoop_Impl.hpp"
#include "../../model/SizingPlant.hpp"
#include "../../model/SizingPlant_Impl.hpp"
#include "../../model/Node.hpp"
#include "../../model/Node_Impl.hpp"
#include "../../model/Splitter.hpp"
#include "../../model/Splitter_Impl.hpp"
#include "../../model/Mixer.hpp"
#include "../../model/Mixer_Impl.hpp"
#include "../../model/PumpVariableSpeed.hpp"
#include "../../model/Schedule.hpp"
#include "../../model/Schedule_Impl.hpp"
#include "../../model/BoilerHotWater.hpp"
#include "../../model/BoilerHotWater_Impl.hpp"
#include "../../model/ChillerElectricEIR.hpp"
#include "../../model/ChillerElectricEIR_Impl.hpp"
#include "../../model/ChillerAbsorption.hpp"
#include "../../model/ChillerAbsorption_Impl.hpp"
#include "../../model/ChillerAbsorptionIndirect.hpp"
#include "../../model/ChillerAbsorptionIndirect_Impl.hpp"
#include "../../model/WaterHeaterMixed.hpp"
#include "../../model/WaterHeaterMixed_Impl.hpp"
#include "../../model/CoolingTowerVariableSpeed.hpp"
#include "../../model/CoolingTowerVariableSpeed_Impl.hpp"
#include "../../model/CoolingTowerSingleSpeed.hpp"
#include "../../model/CoolingTowerSingleSpeed_Impl.hpp"
#include "../../model/CoolingTowerTwoSpeed.hpp"
#include "../../model/CoolingTowerTwoSpeed_Impl.hpp"
#include "../../model/GroundHeatExchangerVertical.hpp"
#include "../../model/GroundHeatExchangerVertical_Impl.hpp"
#include "../../model/GroundHeatExchangerHorizontalTrench.hpp"
#include "../../model/GroundHeatExchangerHorizontalTrench_Impl.hpp"
#include "../../model/HeatExchangerFluidToFluid.hpp"
#include "../../model/HeatExchangerFluidToFluid_Impl.hpp"
#include "../../model/WaterToAirComponent.hpp"
#include "../../model/WaterToAirComponent_Impl.hpp"
#include "../../model/WaterToWaterComponent.hpp"
#include "../../model/WaterToWaterComponent_Impl.hpp"
#include "../../model/CoilHeatingWaterBaseboard.hpp"
#include "../../model/CoilHeatingWaterBaseboard_Impl.hpp"
#include "../../model/CoilCoolingCooledBeam.hpp"
#include "../../model/CoilCoolingCooledBeam_Impl.hpp"
#include "../../model/StraightComponent.hpp"
#include "../../model/StraightComponent_Impl.hpp"
#include "../../model/CoilHeatingLowTempRadiantConstFlow.hpp"
#include "../../model/CoilHeatingLowTempRadiantConstFlow_Impl.hpp"
#include "../../model/CoilCoolingLowTempRadiantConstFlow.hpp"
#include "../../model/CoilCoolingLowTempRadiantConstFlow_Impl.hpp"
#include "../../model/CoilHeatingLowTempRadiantVarFlow.hpp"
#include "../../model/CoilHeatingLowTempRadiantVarFlow_Impl.hpp"
#include "../../model/CoilCoolingLowTempRadiantVarFlow.hpp"
#include "../../model/CoilCoolingLowTempRadiantVarFlow_Impl.hpp"
#include "../../model/ZoneHVACComponent.hpp"
#include "../../model/ZoneHVACComponent_Impl.hpp"
#include "../../model/SetpointManager.hpp"
#include "../../model/LifeCycleCost.hpp"
#include "../../utilities/idf/IdfExtensibleGroup.hpp"
#include <utilities/idd/IddEnums.hxx>
#include <utilities/idd/IddFactory.hxx>
#include <utilities/idd/PlantLoop_FieldEnums.hxx>
#include <utilities/idd/BranchList_FieldEnums.hxx>
#include <utilities/idd/Branch_FieldEnums.hxx>
#include <utilities/idd/ConnectorList_FieldEnums.hxx>
#include <utilities/idd/Connector_Splitter_FieldEnums.hxx>
#include <utilities/idd/Connector_Mixer_FieldEnums.hxx>
#include <utilities/idd/Pipe_Adiabatic_FieldEnums.hxx>
#include <utilities/idd/PlantEquipmentOperationSchemes_FieldEnums.hxx>
#include <utilities/idd/PlantEquipmentOperation_HeatingLoad_FieldEnums.hxx>
#include <utilities/idd/PlantEquipmentOperation_CoolingLoad_FieldEnums.hxx>
#include <utilities/idd/PlantEquipmentOperation_ComponentSetpoint_FieldEnums.hxx>
#include <utilities/idd/PlantEquipmentOperation_Uncontrolled_FieldEnums.hxx>
#include <utilities/idd/PlantEquipmentList_FieldEnums.hxx>
#include <utilities/idd/Sizing_Plant_FieldEnums.hxx>
#include <utilities/idd/AirTerminal_SingleDuct_ConstantVolume_CooledBeam_FieldEnums.hxx>
#include <utilities/idd/ZoneHVAC_AirDistributionUnit_FieldEnums.hxx>
#include "../../utilities/core/Assert.hpp"

using namespace openstudio::model;

using namespace std;

namespace openstudio {

namespace energyplus {

enum SetpointComponentType {HEATING, COOLING, BOTH};

struct SetpointComponentInfo
{
  ModelObject modelObject;
  Node outletNode;
  double flowRate;
  bool isFlowRateAutosized;
  SetpointComponentType type;

  SetpointComponentInfo(const ModelObject & t_mo, const Node & t_outletNode, double t_flowRate, bool t_isFlowRateAutosized, SetpointComponentType t_type )
    : modelObject(t_mo),
      outletNode(t_outletNode),
      flowRate(t_flowRate),
      isFlowRateAutosized(t_isFlowRateAutosized),
      type(t_type)
  {
  }
};

boost::optional<Node> isSetpointComponent(PlantLoop & plantLoop,const ModelObject & comp)
{
  boost::optional<Node> result;

  Node supplyOutletNode = plantLoop.supplyOutletNode();
  boost::optional<HVACComponent> hvacComp = comp.optionalCast<HVACComponent>();
  OS_ASSERT(hvacComp);
  std::vector<Node> nodes = subsetCastVector<Node>(plantLoop.supplyComponents(hvacComp.get(),supplyOutletNode));
  OS_ASSERT(! nodes.empty());
  Node componentOutletNode = nodes.front();
  if( componentOutletNode != supplyOutletNode )
  {
    std::vector<SetpointManager> _setpointManagers = componentOutletNode.setpointManagers();
    if( ! _setpointManagers.empty() )
    {
      result = componentOutletNode;
    }
  }

  return result;
}

IdfObject ForwardTranslator::populateBranch( IdfObject & branchIdfObject, 
                          std::vector<ModelObject> & modelObjects,
                          PlantLoop & plantLoop)
{
  if(modelObjects.size() > 0)
  {
    int i = 0;

    for( auto & modelObject : modelObjects )
    {
      boost::optional<Node> inletNode;
      boost::optional<Node> outletNode;
      std::string objectName;
      std::string iddType;
    
      //translate and map each model object
      //in most cases, the name and idd object type come directly from the resulting idfObject
      if ( boost::optional<IdfObject> idfObject = this->translateAndMapModelObject(modelObject) )
      {
        objectName = idfObject->name().get();
        iddType = idfObject->iddObject().name();
      }

      if( boost::optional<StraightComponent> straightComponent = modelObject.optionalCast<StraightComponent>() )
      {
        inletNode = straightComponent->inletModelObject()->optionalCast<Node>();
        outletNode = straightComponent->outletModelObject()->optionalCast<Node>();
        //special case for ZoneHVAC:Baseeboard:Convective:Water.  In E+, this object appears on both the 
        //zonehvac:equipmentlist and the branch.  In OpenStudio, this object was broken into 2 objects:
        //ZoneHVACBaseboardConvectiveWater and CoilHeatingWaterBaseboard.  The ZoneHVAC goes onto the zone and
        //has a child coil that goes onto the plantloop.  In order to get the correct translation to E+, we need
        //to put the name of the containing ZoneHVACBaseboardConvectiveWater onto the branch.
        if (boost::optional<CoilHeatingWaterBaseboard> coilBB = modelObject.optionalCast<CoilHeatingWaterBaseboard>() )
        {
          if (boost::optional<ZoneHVACComponent> contZnBB = coilBB->containingZoneHVACComponent())
          {
            //translate and map containingZoneHVACBBConvWater
            if ( boost::optional<IdfObject> idfContZnBB = this->translateAndMapModelObject(*contZnBB) )
            {
              //Get the name and the idd object from the idf object version of this
              objectName = idfContZnBB->name().get();
              iddType = idfContZnBB->iddObject().name();
            }
          }
        }
        //special case for AirTerminalSingleDuctConstantVolumeChilledBeam
        if (boost::optional<CoilCoolingCooledBeam> coilCB = modelObject.optionalCast<CoilCoolingCooledBeam>() )
        {
          if (boost::optional<StraightComponent> airTerm = coilCB->containingStraightComponent())
          {  
            boost::optional<IdfObject> idfAirDistUnit = this->translateAndMapModelObject(*airTerm);
            //translate and map containingStraightComponent
            if (idfAirDistUnit)
            {
              //Get the name and idd type of the air terminal inside the air distribution unit
              objectName = idfAirDistUnit->getString(ZoneHVAC_AirDistributionUnitFields::AirTerminalName).get();
              iddType = idfAirDistUnit->getString(ZoneHVAC_AirDistributionUnitFields::AirTerminalObjectType).get();
            }
          }
        }
        //special case for ZoneHVAC:LowTemperatureRadiant:ConstantFlow and ZoneHVAC:LowTemperatureRadiant:VariableFlow.  In E+, this object appears on both the 
        //zonehvac:equipmentlist and the branch.  In OpenStudio, this object was broken into 2 objects:
        //ZoneHVACBaseboardConvectiveWater and CoilHeatingWaterBaseboard.  The ZoneHVAC goes onto the zone and
        //has a child coil that goes onto the plantloop.  In order to get the correct translation to E+, we need
        //to put the name of the containing ZoneHVACBaseboardConvectiveWater onto the branch.
        if (boost::optional<CoilHeatingLowTempRadiantConstFlow> coilHLRC = modelObject.optionalCast<CoilHeatingLowTempRadiantConstFlow>() )
        {
          if (boost::optional<ZoneHVACComponent> znLowTempRadConst = coilHLRC->containingZoneHVACComponent())
          {
            //translate and map containingZoneHVACBBConvWater
            if ( boost::optional<IdfObject> idfZnLowTempRadConst = this->translateAndMapModelObject(*znLowTempRadConst) )
            {
              //Get the name and the idd object from the idf object version of this
              objectName = idfZnLowTempRadConst->name().get();
              iddType = idfZnLowTempRadConst->iddObject().name();
            }
          }
        }
        if (boost::optional<CoilCoolingLowTempRadiantConstFlow> coilCLRC = modelObject.optionalCast<CoilCoolingLowTempRadiantConstFlow>() )
        {
          if (boost::optional<ZoneHVACComponent> znLowTempRadConst = coilCLRC->containingZoneHVACComponent())
          {
            //translate and map containingZoneHVACBBConvWater
            if ( boost::optional<IdfObject> idfZnLowTempRadConst = this->translateAndMapModelObject(*znLowTempRadConst) )
            {
              //Get the name and the idd object from the idf object version of this
              objectName = idfZnLowTempRadConst->name().get();
              iddType = idfZnLowTempRadConst->iddObject().name();
            }
          }
        }
        if (boost::optional<CoilHeatingLowTempRadiantVarFlow> coilHLRC = modelObject.optionalCast<CoilHeatingLowTempRadiantVarFlow>() )
        {
          if (boost::optional<ZoneHVACComponent> znLowTempRadVar = coilHLRC->containingZoneHVACComponent())
          {
            //translate and map containingZoneHVACBBConvWater
            if ( boost::optional<IdfObject> idfZnLowTempRadVar = this->translateAndMapModelObject(*znLowTempRadVar) )
            {
              //Get the name and the idd object from the idf object version of this
              objectName = idfZnLowTempRadVar->name().get();
              iddType = idfZnLowTempRadVar->iddObject().name();
            }
          }
        }
        if (boost::optional<CoilCoolingLowTempRadiantVarFlow> coilCLRC = modelObject.optionalCast<CoilCoolingLowTempRadiantVarFlow>() )
        {
          if (boost::optional<ZoneHVACComponent> znLowTempRadVar = coilCLRC->containingZoneHVACComponent())
          {
            //translate and map containingZoneHVACBBConvWater
            if ( boost::optional<IdfObject> idfZnLowTempRadVar = this->translateAndMapModelObject(*znLowTempRadVar) )
            {
              //Get the name and the idd object from the idf object version of this
              objectName = idfZnLowTempRadVar->name().get();
              iddType = idfZnLowTempRadVar->iddObject().name();
            }
          }
        }
      }
      else if( boost::optional<WaterToAirComponent> waterToAirComponent = modelObject.optionalCast<WaterToAirComponent>() )
      {
        inletNode = waterToAirComponent->waterInletModelObject()->optionalCast<Node>();
        outletNode = waterToAirComponent->waterOutletModelObject()->optionalCast<Node>();
      }
      else if( boost::optional<WaterToWaterComponent> waterToWaterComponent = modelObject.optionalCast<WaterToWaterComponent>() )
      {
        if( plantLoop.supplyComponent(waterToWaterComponent->handle()) )
        {
          inletNode = waterToWaterComponent->supplyInletModelObject()->optionalCast<Node>();
          outletNode = waterToWaterComponent->supplyOutletModelObject()->optionalCast<Node>();
        }
        else if( plantLoop.demandComponent(waterToWaterComponent->handle()) )
        {
          inletNode = waterToWaterComponent->demandInletModelObject()->optionalCast<Node>();
          outletNode = waterToWaterComponent->demandOutletModelObject()->optionalCast<Node>();
        }
      }

      IdfExtensibleGroup eg = branchIdfObject.pushExtensibleGroup();
      eg.setString(BranchExtensibleFields::ComponentObjectType,iddType);
      eg.setString(BranchExtensibleFields::ComponentName,objectName);
      eg.setString(BranchExtensibleFields::ComponentInletNodeName,inletNode->name().get());
      eg.setString(BranchExtensibleFields::ComponentOutletNodeName,outletNode->name().get());
      eg.setString(BranchExtensibleFields::ComponentBranchControlType,"Passive");

      //




      i++;
    }
  }
  
  return branchIdfObject; 
}

boost::optional<IdfObject> ForwardTranslator::translatePlantLoop( PlantLoop & plantLoop )
{
  // Create a new IddObjectType::PlantLoop
  IdfObject idfObject(IddObjectType::PlantLoop);
  m_idfObjects.push_back(idfObject);

  for (LifeCycleCost lifeCycleCost : plantLoop.lifeCycleCosts()){
    translateAndMapModelObject(lifeCycleCost);
  }

  OptionalModelObject temp;
  boost::optional<std::string> s;
  boost::optional<double> value;

  // Name

  if( (s = plantLoop.name()) )
  {
    idfObject.setName(s.get());
  }

  // Fluid Type

  if( (s = plantLoop.fluidType()) )
  {
    idfObject.setString(PlantLoopFields::FluidType,s.get());
  }

  // Loop Temperature Setpoint Node Name

  if( boost::optional<Node> node = plantLoop.loopTemperatureSetpointNode() )
  {
    idfObject.setString(PlantLoopFields::LoopTemperatureSetpointNodeName,node->name().get());
  }

  // Maximum Loop Temperature

  if( (value = plantLoop.maximumLoopTemperature()) )
  {
    idfObject.setDouble(PlantLoopFields::MaximumLoopTemperature,value.get());
  } 

  // Minimum Loop Temperature

  if( (value = plantLoop.minimumLoopTemperature()) )
  {
    idfObject.setDouble(PlantLoopFields::MinimumLoopTemperature,value.get());
  } 
  
  // Maximum Loop Flow Rate

  if( plantLoop.isMaximumLoopFlowRateAutosized() )
  {
    idfObject.setString(PlantLoopFields::MaximumLoopFlowRate,"Autosize");
  }
  else if( (value = plantLoop.maximumLoopFlowRate()) )
  {
    idfObject.setDouble(PlantLoopFields::MaximumLoopFlowRate,value.get());
  }
  
  // Minimum Loop Flow Rate

  if( plantLoop.isMinimumLoopFlowRateAutosized() )
  {
    idfObject.setString(PlantLoopFields::MinimumLoopFlowRate,"Autosize");
  }
  else if( (value = plantLoop.minimumLoopFlowRate()) )
  {
    idfObject.setDouble(PlantLoopFields::MinimumLoopFlowRate,value.get());
  }

  // LoadDistributionScheme

  idfObject.setString(PlantLoopFields::LoadDistributionScheme,"Optimal");

  // Plant Loop Volume

  if( plantLoop.isPlantLoopVolumeAutocalculated() )
  {
    idfObject.setString(PlantLoopFields::PlantLoopVolume,"Autocalculate");
  }
  else if( (value = plantLoop.plantLoopVolume()) )
  {
    idfObject.setDouble(PlantLoopFields::PlantLoopVolume,value.get());
  }

  // Common Pipe Simulation

  if( (s = plantLoop.commonPipeSimulation()) )
  {
    idfObject.setString(PlantLoopFields::CommonPipeSimulation,s.get());
  } 

  // Inlet/Outlet Nodes
  idfObject.setString(PlantLoopFields::PlantSideInletNodeName,plantLoop.supplyInletNode().name().get());
  idfObject.setString(PlantLoopFields::PlantSideOutletNodeName,plantLoop.supplyOutletNode().name().get());
  idfObject.setString(PlantLoopFields::DemandSideInletNodeName,plantLoop.demandInletNode().name().get());
  idfObject.setString(PlantLoopFields::DemandSideOutletNodeName,plantLoop.demandOutletNode().name().get());

  // Operation Scheme

  IdfObject _operationScheme(IddObjectType::PlantEquipmentOperationSchemes);
  m_idfObjects.push_back(_operationScheme);
  _operationScheme.setName(plantLoop.name().get() + " Operation Scheme List");
  _operationScheme.clearExtensibleGroups();
  
  idfObject.setString(PlantLoopFields::PlantEquipmentOperationSchemeName,_operationScheme.name().get());

  std::vector<ModelObject> supplyComponents = plantLoop.supplyComponents();
  std::vector<ModelObject> heatingComponents;
  std::vector<ModelObject> coolingComponents;
  std::vector<ModelObject> uncontrolledComponents;
  std::vector<SetpointComponentInfo> setpointComponents;

  // These will be used later, but only if reasonable sizing values have not already been provided.
  bool sizeAsCondenserSystem = false;
  bool sizeAsHotWaterSystem = false;
  bool sizeAsChilledWaterSystem = false;

  for( const auto & supplyComponent : supplyComponents )
  {
    bool autosize = false;
    double flowRate = 0.0;

    switch(supplyComponent.iddObject().type().value())
    {
      case openstudio::IddObjectType::OS_Boiler_HotWater :
      {
        sizeAsHotWaterSystem = true;
        if( boost::optional<Node> outletNode = isSetpointComponent(plantLoop,supplyComponent) ) 
        {
          BoilerHotWater boiler = supplyComponent.cast<BoilerHotWater>();
          if( boiler.isDesignWaterFlowRateAutosized() )
          {
            autosize = true;
          }
          else if(boost::optional<double> optionalFlowRate = boiler.designWaterFlowRate())
          {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,HEATING));
        }
        else
        {
          heatingComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_WaterHeater_Mixed :
      {
        sizeAsHotWaterSystem = true;
        if( boost::optional<Node> outletNode = isSetpointComponent(plantLoop,supplyComponent) ) 
        {
          WaterHeaterMixed waterHeater = supplyComponent.cast<WaterHeaterMixed>();
          if( waterHeater.isUseSideDesignFlowRateAutosized() )
          {
            autosize = true;
          }
          else if(boost::optional<double> optionalFlowRate = waterHeater.useSideDesignFlowRate())
          {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,HEATING));
        }
        else
        {
          heatingComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_DistrictHeating :
      {
        sizeAsHotWaterSystem = true;
        if( boost::optional<Node> outletNode = isSetpointComponent(plantLoop,supplyComponent) ) 
        {
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,0.0,true,HEATING));
        }
        else
        {
          heatingComponents.push_back(supplyComponent);
        }
        break;
      }      
      case openstudio::IddObjectType::OS_Chiller_Electric_EIR :
      {
        sizeAsChilledWaterSystem = true;
        if( boost::optional<Node> outletNode = isSetpointComponent(plantLoop,supplyComponent) ) 
        {
          ChillerElectricEIR chiller = supplyComponent.cast<ChillerElectricEIR>();
          if( chiller.isReferenceChilledWaterFlowRateAutosized() )
          {
            autosize = true;
          }
          else if(boost::optional<double> optionalFlowRate = chiller.referenceChilledWaterFlowRate())
          {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,COOLING));
        }
        else
        {
          coolingComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_Chiller_Absorption_Indirect :
      {
        sizeAsChilledWaterSystem = true;
        if( auto outletNode = isSetpointComponent(plantLoop,supplyComponent) ) {
          auto chiller = supplyComponent.cast<ChillerAbsorptionIndirect>();
          if( chiller.isDesignChilledWaterFlowRateAutosized() ) {
            autosize = true;
          } else if(auto optionalFlowRate = chiller.designChilledWaterFlowRate()) {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,COOLING));
        } else {
          coolingComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_Chiller_Absorption :
      {
        sizeAsChilledWaterSystem = true;
        if( auto outletNode = isSetpointComponent(plantLoop,supplyComponent) ) {
          auto chiller = supplyComponent.cast<ChillerAbsorption>();
          if( chiller.isDesignChilledWaterFlowRateAutosized() ) {
            autosize = true;
          } else if(auto optionalFlowRate = chiller.designChilledWaterFlowRate()) {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,COOLING));
        } else {
          coolingComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_ThermalStorage_Ice_Detailed :
      {
        sizeAsChilledWaterSystem = true;
        if( auto outletNode = isSetpointComponent(plantLoop,supplyComponent) ) {
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,0.0,true,BOTH));
        }
        else
        {
          coolingComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_DistrictCooling :
      {
        sizeAsChilledWaterSystem = true;
        if( boost::optional<Node> outletNode = isSetpointComponent(plantLoop,supplyComponent) ) 
        {
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,0.0,true,COOLING));
        }
        else
        {
          coolingComponents.push_back(supplyComponent);
        }
        break;
      }      
      case openstudio::IddObjectType::OS_CoolingTower_SingleSpeed :
      {
        sizeAsCondenserSystem = true;
        if( boost::optional<Node> outletNode = isSetpointComponent(plantLoop,supplyComponent) ) 
        {
          CoolingTowerSingleSpeed tower = supplyComponent.cast<CoolingTowerSingleSpeed>();
          if( tower.isDesignWaterFlowRateAutosized() )
          {
            autosize = true;
          }
          else if(boost::optional<double> optionalFlowRate = tower.designWaterFlowRate())
          {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,COOLING));
        }
        else
        {
          coolingComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_CoolingTower_VariableSpeed :
      {
        sizeAsCondenserSystem = true;
        if( boost::optional<Node> outletNode = isSetpointComponent(plantLoop,supplyComponent) ) 
        {
          CoolingTowerVariableSpeed tower = supplyComponent.cast<CoolingTowerVariableSpeed>();
          if( tower.isDesignWaterFlowRateAutosized() )
          {
            autosize = true;
          }
          else if(boost::optional<double> optionalFlowRate = tower.designWaterFlowRate())
          {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,COOLING));
        }
        else
        {
          coolingComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_CoolingTower_TwoSpeed:
      {
        sizeAsCondenserSystem = true;
        if (boost::optional<Node> outletNode = isSetpointComponent(plantLoop, supplyComponent))
        {
          CoolingTowerTwoSpeed tower = supplyComponent.cast<CoolingTowerTwoSpeed>();
          if (tower.isDesignWaterFlowRateAutosized())
          {
            autosize = true;
          }
          else if (boost::optional<double> optionalFlowRate = tower.designWaterFlowRate())
          {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,COOLING));
        }
        else
        {
          coolingComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_GroundHeatExchanger_Vertical :
      {
        sizeAsCondenserSystem = true;
        if (boost::optional<Node> outletNode = isSetpointComponent(plantLoop, supplyComponent))
        {
          GroundHeatExchangerVertical hx = supplyComponent.cast<GroundHeatExchangerVertical>();
          if (boost::optional<double> optionalFlowRate = hx.maximumFlowRate())
          {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,BOTH));
        }
        else
        {
          uncontrolledComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_GroundHeatExchanger_HorizontalTrench :
      {
        sizeAsCondenserSystem = true;
        if (boost::optional<Node> outletNode = isSetpointComponent(plantLoop, supplyComponent))
        {
          auto hx = supplyComponent.cast<GroundHeatExchangerHorizontalTrench>();
          if (boost::optional<double> optionalFlowRate = hx.designFlowRate())
          {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,BOTH));
        }
        else
        {
          uncontrolledComponents.push_back(supplyComponent);
        }
        break;
      }
      case openstudio::IddObjectType::OS_HeatExchanger_FluidToFluid :
      {
        sizeAsCondenserSystem = true;
        if (boost::optional<Node> outletNode = isSetpointComponent(plantLoop, supplyComponent))
        {
          auto hx = supplyComponent.cast<HeatExchangerFluidToFluid>();
          if (hx.isLoopSupplySideDesignFlowRateAutosized())
          {
            autosize = true;
          }
          else if (boost::optional<double> optionalFlowRate = hx.loopSupplySideDesignFlowRate())
          {
            flowRate = optionalFlowRate.get();
          }
          setpointComponents.push_back(SetpointComponentInfo(supplyComponent,*outletNode,flowRate,autosize,BOTH));
        }
        else
        {
          uncontrolledComponents.push_back(supplyComponent);
        }
        break;
      }
      default:
      {
        break;
      }
    }
  }

  boost::optional<IdfObject> _optionalHeatingPlantEquipmentList;
  boost::optional<IdfObject> _optionalCoolingPlantEquipmentList;
  boost::optional<IdfObject> _optionalUncontrolledPlantEquipmentList;
  boost::optional<IdfObject> _optionalSetpointOperation;

  if( ! setpointComponents.empty() )
  {
    Schedule alwaysOn = plantLoop.model().alwaysOnDiscreteSchedule();
    IdfObject scheduleCompact = translateAndMapModelObject(alwaysOn).get();

    IdfObject _setpointOperation(IddObjectType::PlantEquipmentOperation_ComponentSetpoint);
    _setpointOperation.setName(plantLoop.name().get() + " Setpoint Operation Scheme");
    m_idfObjects.push_back(_setpointOperation);
    _setpointOperation.clearExtensibleGroups();
    _optionalSetpointOperation = _setpointOperation;

    IdfExtensibleGroup eg = _operationScheme.pushExtensibleGroup();
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeObjectType,_setpointOperation.iddObject().name());
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeName,_setpointOperation.name().get());
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeScheduleName,scheduleCompact.name().get());
  }

  if( ! heatingComponents.empty() )
  {
    Schedule alwaysOn = plantLoop.model().alwaysOnDiscreteSchedule();
    IdfObject scheduleCompact = translateAndMapModelObject(alwaysOn).get();

    IdfObject _heatingOperation(IddObjectType::PlantEquipmentOperation_HeatingLoad);
    _heatingOperation.setName(plantLoop.name().get() + " Heating Operation Scheme");
    m_idfObjects.push_back(_heatingOperation);
    _heatingOperation.clearExtensibleGroups();

    IdfObject _plantEquipmentList(IddObjectType::PlantEquipmentList);
    _plantEquipmentList.setName(plantLoop.name().get() + " Heating Equipment List");
    _plantEquipmentList.clearExtensibleGroups();
    m_idfObjects.push_back(_plantEquipmentList);
    _optionalHeatingPlantEquipmentList = _plantEquipmentList;

    IdfExtensibleGroup eg = _heatingOperation.pushExtensibleGroup();
    eg.setDouble(PlantEquipmentOperation_HeatingLoadExtensibleFields::LoadRangeLowerLimit,0.0);
    eg.setDouble(PlantEquipmentOperation_HeatingLoadExtensibleFields::LoadRangeUpperLimit,1E9);
    eg.setString(PlantEquipmentOperation_HeatingLoadExtensibleFields::RangeEquipmentListName,_plantEquipmentList.name().get());

    eg = _operationScheme.pushExtensibleGroup();
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeObjectType,_heatingOperation.iddObject().name());
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeName,_heatingOperation.name().get());
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeScheduleName,scheduleCompact.name().get());
  }

  if( ! coolingComponents.empty() )
  {
    Schedule alwaysOn2 = plantLoop.model().alwaysOnDiscreteSchedule();
    IdfObject scheduleCompact2 = translateAndMapModelObject(alwaysOn2).get();
    
    IdfObject _coolingOperation(IddObjectType::PlantEquipmentOperation_CoolingLoad);
    _coolingOperation.setName(plantLoop.name().get() + " Cooling Operation Scheme");
    m_idfObjects.push_back(_coolingOperation);
    _coolingOperation.clearExtensibleGroups();

    IdfObject _plantEquipmentList(IddObjectType::PlantEquipmentList);
    _plantEquipmentList.setName(plantLoop.name().get() + " Cooling Equipment List");
    _plantEquipmentList.clearExtensibleGroups();
    m_idfObjects.push_back(_plantEquipmentList);
    _optionalCoolingPlantEquipmentList = _plantEquipmentList;

    IdfExtensibleGroup eg = _coolingOperation.pushExtensibleGroup();
    eg.setDouble(PlantEquipmentOperation_CoolingLoadExtensibleFields::LoadRangeLowerLimit,0.0);
    eg.setDouble(PlantEquipmentOperation_CoolingLoadExtensibleFields::LoadRangeUpperLimit,1E9);
    eg.setString(PlantEquipmentOperation_CoolingLoadExtensibleFields::RangeEquipmentListName,_plantEquipmentList.name().get());

    eg = _operationScheme.pushExtensibleGroup();
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeObjectType,_coolingOperation.iddObject().name());
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeName,_coolingOperation.name().get());
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeScheduleName,scheduleCompact2.name().get());
  }

  if( ! uncontrolledComponents.empty() )
  {
    Schedule alwaysOn = plantLoop.model().alwaysOnDiscreteSchedule();
    IdfObject _alwaysOn = translateAndMapModelObject(alwaysOn).get();
    
    IdfObject _uncontrolledOperation(IddObjectType::PlantEquipmentOperation_Uncontrolled);
    _uncontrolledOperation.setName(plantLoop.name().get() + " Uncontrolled Operation Scheme");
    m_idfObjects.push_back(_uncontrolledOperation);
    _uncontrolledOperation.clearExtensibleGroups();

    IdfObject _plantEquipmentList(IddObjectType::PlantEquipmentList);
    _plantEquipmentList.setName(plantLoop.name().get() + " Uncontrolled Equipment List");
    _plantEquipmentList.clearExtensibleGroups();
    m_idfObjects.push_back(_plantEquipmentList);
    _optionalUncontrolledPlantEquipmentList = _plantEquipmentList;

    _uncontrolledOperation.setString(PlantEquipmentOperation_UncontrolledFields::EquipmentListName,_plantEquipmentList.name().get());

    IdfExtensibleGroup eg = _operationScheme.pushExtensibleGroup();
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeObjectType,_uncontrolledOperation.iddObject().name());
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeName,_uncontrolledOperation.name().get());
    eg.setString(PlantEquipmentOperationSchemesExtensibleFields::ControlSchemeScheduleName,_alwaysOn.name().get());
  }

  SizingPlant sizingPlant = plantLoop.sizingPlant();

  if( (sizingPlant.designLoopExitTemperature() < 0.01) && (sizingPlant.loopDesignTemperatureDifference() < 0.01) )
  {
    if( sizeAsCondenserSystem )
    {
      sizingPlant.setLoopType("Condenser");
      sizingPlant.setDesignLoopExitTemperature(29.4);
      sizingPlant.setLoopDesignTemperatureDifference(5.6);
    }
    else if( sizeAsChilledWaterSystem )
    {
      sizingPlant.setLoopType("Cooling");
      sizingPlant.setDesignLoopExitTemperature(7.22);
      sizingPlant.setLoopDesignTemperatureDifference(6.67);
    }
    else 
    {
      sizingPlant.setLoopType("Heating");
      sizingPlant.setDesignLoopExitTemperature(82.0);
      sizingPlant.setLoopDesignTemperatureDifference(11.0);
    }
  }

  translateAndMapModelObject(sizingPlant);

  // Supply Side

  IdfObject _supplyBranchList(IddObjectType::BranchList);
  _supplyBranchList.setName(plantLoop.name().get() + " Supply Branches");
  _supplyBranchList.clearExtensibleGroups();
  m_idfObjects.push_back(_supplyBranchList);
  idfObject.setString(PlantLoopFields::PlantSideBranchListName,_supplyBranchList.name().get());

  IdfObject _supplyConnectorList(IddObjectType::ConnectorList);
  _supplyConnectorList.setName(plantLoop.name().get() + " Supply Connector List");
  m_idfObjects.push_back(_supplyConnectorList);
  idfObject.setString(PlantLoopFields::PlantSideConnectorListName,_supplyConnectorList.name().get());

  Splitter supplySplitter = plantLoop.supplySplitter();
  Mixer supplyMixer = plantLoop.supplyMixer();

  Node supplyInletNode = plantLoop.supplyInletNode();
  Node supplyOutletNode = plantLoop.supplyOutletNode();

  IdfObject _supplySplitter(IddObjectType::Connector_Splitter);
  _supplySplitter.clearExtensibleGroups();
  _supplySplitter.setName(plantLoop.name().get() + " Supply Splitter");
  m_idfObjects.push_back(_supplySplitter);
  ExtensibleIndex ei2(0,0);
  ei2.group = 0;
  ei2.field = ConnectorListExtensibleFields::ConnectorObjectType;
  _supplyConnectorList.setString(_supplyConnectorList.iddObject().index(ei2),
                                 _supplySplitter.iddObject().name());
  ei2.field = ConnectorListExtensibleFields::ConnectorName;
  _supplyConnectorList.setString(_supplyConnectorList.iddObject().index(ei2),
                                 _supplySplitter.name().get());

  IdfObject _supplyMixer(IddObjectType::Connector_Mixer);
  _supplyMixer.clearExtensibleGroups();
  _supplyMixer.setName(plantLoop.name().get() + " Supply Mixer");
  m_idfObjects.push_back(_supplyMixer);
  ei2.group = 1;
  ei2.field = ConnectorListExtensibleFields::ConnectorObjectType;
  _supplyConnectorList.setString(_supplyConnectorList.iddObject().index(ei2),
                                 _supplyMixer.iddObject().name());
  ei2.field = ConnectorListExtensibleFields::ConnectorName;
  _supplyConnectorList.setString(_supplyConnectorList.iddObject().index(ei2),
                                 _supplyMixer.name().get());

  // Supply inlet branch

  IdfObject _supplyInletBranch(IddObjectType::Branch);
  _supplyInletBranch.setName(plantLoop.name().get() + " Supply Inlet Branch");
  _supplyInletBranch.clearExtensibleGroups();
  IdfExtensibleGroup eg = _supplyBranchList.pushExtensibleGroup();
  eg.setString(BranchListExtensibleFields::BranchName,_supplyInletBranch.name().get());

  _supplySplitter.setString(Connector_SplitterFields::InletBranchName,_supplyInletBranch.name().get());

  m_idfObjects.push_back(_supplyInletBranch);

  std::vector<ModelObject> supplyInletModelObjects;
  std::vector<ModelObject> supplyInletBranchModelObjects;
  supplyInletModelObjects = plantLoop.supplyComponents(supplyInletNode,supplySplitter);

  OS_ASSERT( supplyInletModelObjects.size() >= 2 );

  supplyInletModelObjects.erase(supplyInletModelObjects.begin());
  supplyInletModelObjects.erase(supplyInletModelObjects.end() - 1);

  for( auto & supplyInletModelObject : supplyInletModelObjects )
  {
    //nodes don't go onto branches, but still need to be translated and mapped
    //because doing so translates their setpoint managers
    if( boost::optional<Node> node = supplyInletModelObject.optionalCast<Node>() )
    {
      boost::optional<IdfObject> idfObject = this->translateAndMapModelObject(supplyInletModelObject);
    }
    //all other types of objects go onto the branch
    else
    {
      supplyInletBranchModelObjects.push_back(supplyInletModelObject);
    }
  }

  if( supplyInletBranchModelObjects.size() > 0 )
  {
    populateBranch( _supplyInletBranch,
                    supplyInletBranchModelObjects,
                    plantLoop );
  }
  else
  {
    IdfObject pipe(IddObjectType::Pipe_Adiabatic);
    pipe.setName(plantLoop.name().get() + " Supply Inlet Pipe");
    m_idfObjects.push_back(pipe);

    std::string inletNodeName = plantLoop.supplyInletNode().name().get();
    std::string outletNodeName = plantLoop.name().get() + " Supply Inlet Pipe Node";

    pipe.setString(Pipe_AdiabaticFields::InletNodeName,inletNodeName);
    pipe.setString(Pipe_AdiabaticFields::OutletNodeName,outletNodeName);

    IdfExtensibleGroup eg = _supplyInletBranch.pushExtensibleGroup();
    eg.setString(BranchExtensibleFields::ComponentObjectType,pipe.iddObject().name());
    eg.setString(BranchExtensibleFields::ComponentName,pipe.name().get());
    eg.setString(BranchExtensibleFields::ComponentInletNodeName,inletNodeName);
    eg.setString(BranchExtensibleFields::ComponentOutletNodeName,outletNodeName);
    eg.setString(BranchExtensibleFields::ComponentBranchControlType,"Passive");
  }

  // Populate supply equipment branches

  std::vector<model::ModelObject> splitterOutletObjects = supplySplitter.outletModelObjects();
  std::vector<model::ModelObject> mixerInletObjects = supplyMixer.inletModelObjects();

  auto it2 = mixerInletObjects.begin();
  unsigned i = 0;
  for( auto & splitterOutletObject : splitterOutletObjects )
  {
    i++;

    std::stringstream ss;
    ss << i;
    std::string istring = ss.str();

    model::HVACComponent comp1 = splitterOutletObject.optionalCast<model::HVACComponent>().get();
    model::HVACComponent comp2 = it2->optionalCast<model::HVACComponent>().get();

    std::vector<model::ModelObject> allComponents = plantLoop.supplyComponents(comp1,comp2);
    std::vector<model::ModelObject> branchComponents;

    IdfObject _equipmentBranch(IddObjectType::Branch); 
    _equipmentBranch.clearExtensibleGroups();
    _equipmentBranch.setName( plantLoop.name().get() + " Supply Branch " + istring );
    m_idfObjects.push_back(_equipmentBranch);

    eg = _supplySplitter.pushExtensibleGroup();
    eg.setString(Connector_SplitterExtensibleFields::OutletBranchName,_equipmentBranch.name().get());

    eg = _supplyMixer.pushExtensibleGroup();
    eg.setString(Connector_MixerExtensibleFields::InletBranchName,_equipmentBranch.name().get());

    eg = _supplyBranchList.pushExtensibleGroup();
    eg.setString(BranchListExtensibleFields::BranchName,_equipmentBranch.name().get());

    for( auto & component : allComponents )
    {
      //nodes don't go onto branches, but still need to be translated and mapped
      //because doing so translates their setpoint managers
      if( boost::optional<Node> node = component.optionalCast<Node>() )
      {
        boost::optional<IdfObject> idfObject = this->translateAndMapModelObject(component);
      }
      //all other types of objects go onto the branch
      else
      {
        branchComponents.push_back(component);
      }
    }

    if( branchComponents.size() > 0 )
    {
      populateBranch( _equipmentBranch,
                      branchComponents,
                      plantLoop );
    }
    else
    {
      IdfObject pipe(IddObjectType::Pipe_Adiabatic);
      pipe.setName(plantLoop.name().get() + " Supply Branch " + istring + " Pipe");
      m_idfObjects.push_back(pipe);

      std::string inletNodeName = pipe.name().get() + " Inlet Node";
      std::string outletNodeName = pipe.name().get() + " Outlet Node";
      
      pipe.setString(Pipe_AdiabaticFields::InletNodeName,inletNodeName);
      pipe.setString(Pipe_AdiabaticFields::OutletNodeName,outletNodeName);

      IdfExtensibleGroup eg = _equipmentBranch.pushExtensibleGroup();
      eg.setString(BranchExtensibleFields::ComponentObjectType,pipe.iddObject().name());
      eg.setString(BranchExtensibleFields::ComponentName,pipe.name().get());
      eg.setString(BranchExtensibleFields::ComponentInletNodeName,inletNodeName);
      eg.setString(BranchExtensibleFields::ComponentOutletNodeName,outletNodeName);
      eg.setString(BranchExtensibleFields::ComponentBranchControlType,"Passive");
    }

    ++it2;
  }

  // Populate supply outlet branch

  IdfObject _supplyOutletBranch(IddObjectType::Branch);
  _supplyOutletBranch.setName(plantLoop.name().get() + " Supply Outlet Branch");
  _supplyOutletBranch.clearExtensibleGroups();
  eg = _supplyBranchList.pushExtensibleGroup();
  eg.setString(BranchListExtensibleFields::BranchName,_supplyOutletBranch.name().get());

  _supplyMixer.setString(Connector_MixerFields::OutletBranchName,_supplyOutletBranch.name().get());

  m_idfObjects.push_back(_supplyOutletBranch);

  std::vector<ModelObject> supplyOutletModelObjects;
  std::vector<ModelObject> supplyOutletBranchModelObjects;
  supplyOutletModelObjects = plantLoop.supplyComponents(supplyMixer,supplyOutletNode);  

  OS_ASSERT( supplyOutletModelObjects.size() >= 2 );

  supplyOutletModelObjects.erase(supplyOutletModelObjects.begin());
  supplyOutletModelObjects.erase(supplyOutletModelObjects.end() - 1);

  for( auto & supplyOutletModelObject : supplyOutletModelObjects )
  {
    //nodes don't go onto branches, but still need to be translated and mapped
    //because doing so translates their setpoint managers
    if( boost::optional<Node> node = supplyOutletModelObject.optionalCast<Node>() )
    {
      boost::optional<IdfObject> idfObject = this->translateAndMapModelObject(supplyOutletModelObject);
    }
    //all other types of objects go onto the branch
    else
    {
      supplyOutletBranchModelObjects.push_back(supplyOutletModelObject);
    }
  }

  if( supplyOutletBranchModelObjects.size() > 0 )
  {
    populateBranch( _supplyOutletBranch,
                    supplyOutletBranchModelObjects,
                    plantLoop );
  }
  else
  {
    IdfObject pipe(IddObjectType::Pipe_Adiabatic);
    pipe.setName(plantLoop.name().get() + " Supply Outlet Pipe");
    m_idfObjects.push_back(pipe);

    std::string inletNodeName = plantLoop.name().get() + " Supply Outlet Pipe Node";
    std::string outletNodeName = plantLoop.supplyOutletNode().name().get();
    
    pipe.setString(Pipe_AdiabaticFields::InletNodeName,inletNodeName);
    pipe.setString(Pipe_AdiabaticFields::OutletNodeName,outletNodeName);

    IdfExtensibleGroup eg = _supplyOutletBranch.pushExtensibleGroup();
    eg.setString(BranchExtensibleFields::ComponentObjectType,pipe.iddObject().name());
    eg.setString(BranchExtensibleFields::ComponentName,pipe.name().get());
    eg.setString(BranchExtensibleFields::ComponentInletNodeName,inletNodeName);
    eg.setString(BranchExtensibleFields::ComponentOutletNodeName,outletNodeName);
    eg.setString(BranchExtensibleFields::ComponentBranchControlType,"Passive");
  }

  // Demand Side

  IdfObject _demandBranchList(IddObjectType::BranchList);
  _demandBranchList.setName(plantLoop.name().get() + " Demand Branches");
  _demandBranchList.clearExtensibleGroups();
  m_idfObjects.push_back(_demandBranchList);
  idfObject.setString(PlantLoopFields::DemandSideBranchListName,_demandBranchList.name().get());

  IdfObject _demandConnectorList(IddObjectType::ConnectorList);
  _demandConnectorList.setName(plantLoop.name().get() + " Demand Connector List");
  m_idfObjects.push_back(_demandConnectorList);
  idfObject.setString(PlantLoopFields::DemandSideConnectorListName,_demandConnectorList.name().get());

  Splitter demandSplitter = plantLoop.demandSplitter();
  Mixer demandMixer = plantLoop.demandMixer();

  Node demandInletNode = plantLoop.demandInletNode();
  Node demandOutletNode = plantLoop.demandOutletNode();

  IdfObject _demandSplitter(IddObjectType::Connector_Splitter);
  _demandSplitter.clearExtensibleGroups();
  _demandSplitter.setName(plantLoop.name().get() + " Demand Splitter");
  m_idfObjects.push_back(_demandSplitter);
  ExtensibleIndex ei(0,0);
  ei.group = 0;
  ei.field = ConnectorListExtensibleFields::ConnectorObjectType;
  _demandConnectorList.setString(_demandConnectorList.iddObject().index(ei),
                                 _demandSplitter.iddObject().name());
  ei.field = ConnectorListExtensibleFields::ConnectorName;
  _demandConnectorList.setString(_demandConnectorList.iddObject().index(ei),
                                 _demandSplitter.name().get());

  IdfObject _demandMixer(IddObjectType::Connector_Mixer);
  _demandMixer.clearExtensibleGroups();
  _demandMixer.setName(plantLoop.name().get() + " Demand Mixer");
  m_idfObjects.push_back(_demandMixer);
  ei.group = 1;
  ei.field = ConnectorListExtensibleFields::ConnectorObjectType;
  _demandConnectorList.setString(_demandConnectorList.iddObject().index(ei),
                                 _demandMixer.iddObject().name());
  ei.field = ConnectorListExtensibleFields::ConnectorName;
  _demandConnectorList.setString(_demandConnectorList.iddObject().index(ei),
                                 _demandMixer.name().get());

  // Demand inlet branch

  IdfObject _demandInletBranch(IddObjectType::Branch);
  _demandInletBranch.setName(plantLoop.name().get() + " Demand Inlet Branch");
  _demandInletBranch.clearExtensibleGroups();
  eg = _demandBranchList.pushExtensibleGroup();
  eg.setString(BranchListExtensibleFields::BranchName,_demandInletBranch.name().get());

  _demandSplitter.setString(Connector_SplitterFields::InletBranchName,_demandInletBranch.name().get());

  m_idfObjects.push_back(_demandInletBranch);

  std::vector<ModelObject> demandInletModelObjects;
  std::vector<ModelObject> demandInletBranchModelObjects;
  demandInletModelObjects = plantLoop.demandComponents(demandInletNode,demandSplitter);

  OS_ASSERT( demandInletModelObjects.size() >= 2 );  

  demandInletModelObjects.erase(demandInletModelObjects.begin());
  demandInletModelObjects.erase(demandInletModelObjects.end() - 1);

  for( auto & demandInletModelObject : demandInletModelObjects )
  {
    //nodes don't go onto branches, but still need to be translated and mapped
    //because doing so translates their setpoint managers
    if( boost::optional<Node> node = demandInletModelObject.optionalCast<Node>() )
    {
      boost::optional<IdfObject> idfObject = this->translateAndMapModelObject(demandInletModelObject);
    }
    //all other types of objects go onto the branch
    else
    {
      demandInletBranchModelObjects.push_back(demandInletModelObject);
    }
  }

  if( demandInletBranchModelObjects.size() > 0 )
  {
    populateBranch( _demandInletBranch,
                    demandInletBranchModelObjects,
                    plantLoop );
  }
  else
  {
    IdfObject pipe(IddObjectType::Pipe_Adiabatic);
    pipe.setName(plantLoop.name().get() + " Demand Inlet Pipe");
    m_idfObjects.push_back(pipe);

    std::string inletNodeName = plantLoop.demandInletNode().name().get();
    std::string outletNodeName = plantLoop.name().get() + " Demand Inlet Pipe Node";
    
    pipe.setString(Pipe_AdiabaticFields::InletNodeName,inletNodeName);
    pipe.setString(Pipe_AdiabaticFields::OutletNodeName,outletNodeName);

    IdfExtensibleGroup eg = _demandInletBranch.pushExtensibleGroup();
    eg.setString(BranchExtensibleFields::ComponentObjectType,pipe.iddObject().name());
    eg.setString(BranchExtensibleFields::ComponentName,pipe.name().get());
    eg.setString(BranchExtensibleFields::ComponentInletNodeName,inletNodeName);
    eg.setString(BranchExtensibleFields::ComponentOutletNodeName,outletNodeName);
    eg.setString(BranchExtensibleFields::ComponentBranchControlType,"Passive");
  }

  // Populate equipment branches

  splitterOutletObjects = demandSplitter.outletModelObjects();
  mixerInletObjects = demandMixer.inletModelObjects();

  it2 = mixerInletObjects.begin();
  i = 0;
  for( auto & splitterOutletObject : splitterOutletObjects )
  {
    i++;

    std::stringstream ss;
    ss << i;
    std::string istring = ss.str();

    model::HVACComponent comp1 = splitterOutletObject.optionalCast<model::HVACComponent>().get();
    model::HVACComponent comp2 = it2->optionalCast<model::HVACComponent>().get();

    std::vector<model::ModelObject> allComponents = plantLoop.demandComponents(comp1,comp2);
    std::vector<model::ModelObject> branchComponents;

    IdfObject _equipmentBranch(IddObjectType::Branch); 
    _equipmentBranch.clearExtensibleGroups();
    _equipmentBranch.setName( plantLoop.name().get() + " Demand Branch " + istring );
    m_idfObjects.push_back(_equipmentBranch);

    eg = _demandSplitter.pushExtensibleGroup();
    eg.setString(Connector_SplitterExtensibleFields::OutletBranchName,_equipmentBranch.name().get());

    eg = _demandMixer.pushExtensibleGroup();
    eg.setString(Connector_MixerExtensibleFields::InletBranchName,_equipmentBranch.name().get());

    eg = _demandBranchList.pushExtensibleGroup();
    eg.setString(BranchListExtensibleFields::BranchName,_equipmentBranch.name().get());

    for( auto & component : allComponents )
    {
      //nodes don't go onto branches, but still need to be translated and mapped
      //because doing so translates their setpoint managers
      if( boost::optional<Node> node = component.optionalCast<Node>() )
      {
        boost::optional<IdfObject> idfObject = this->translateAndMapModelObject(component);
      }
      //all other types of objects go onto the branch
      else
      {
        branchComponents.push_back(component);
      }
    }

    if( branchComponents.size() > 0 )
    {
      populateBranch( _equipmentBranch,
                      branchComponents,
                      plantLoop );
    }
    else
    {
      IdfObject pipe(IddObjectType::Pipe_Adiabatic);
      pipe.setName(plantLoop.name().get() + " Demand Branch " + istring + " Pipe");
      m_idfObjects.push_back(pipe);

      std::string inletNodeName = pipe.name().get() + " Inlet Node";
      std::string outletNodeName = pipe.name().get() + " Outlet Node";
      
      pipe.setString(Pipe_AdiabaticFields::InletNodeName,inletNodeName);
      pipe.setString(Pipe_AdiabaticFields::OutletNodeName,outletNodeName);

      IdfExtensibleGroup eg = _equipmentBranch.pushExtensibleGroup();
      eg.setString(BranchExtensibleFields::ComponentObjectType,pipe.iddObject().name());
      eg.setString(BranchExtensibleFields::ComponentName,pipe.name().get());
      eg.setString(BranchExtensibleFields::ComponentInletNodeName,inletNodeName);
      eg.setString(BranchExtensibleFields::ComponentOutletNodeName,outletNodeName);
      eg.setString(BranchExtensibleFields::ComponentBranchControlType,"Passive");
    }

    ++it2;
  }

  // Install a bypass branch with a pipe

  if( splitterOutletObjects.size() > 0 )
  {
    IdfObject _equipmentBranch(IddObjectType::Branch); 
    _equipmentBranch.clearExtensibleGroups();
    _equipmentBranch.setName( plantLoop.name().get() + " Demand Bypass Branch" );
    m_idfObjects.push_back(_equipmentBranch);

    eg = _demandSplitter.pushExtensibleGroup();
    eg.setString(Connector_SplitterExtensibleFields::OutletBranchName,_equipmentBranch.name().get());

    eg = _demandMixer.pushExtensibleGroup();
    eg.setString(Connector_MixerExtensibleFields::InletBranchName,_equipmentBranch.name().get());

    eg = _demandBranchList.pushExtensibleGroup();
    eg.setString(BranchListExtensibleFields::BranchName,_equipmentBranch.name().get());

    IdfObject pipe(IddObjectType::Pipe_Adiabatic);
    pipe.setName(plantLoop.name().get() + " Demand Bypass Pipe");
    m_idfObjects.push_back(pipe);

    std::string inletNodeName = pipe.name().get() + " Inlet Node";
    std::string outletNodeName = pipe.name().get() + " Outlet Node";
    
    pipe.setString(Pipe_AdiabaticFields::InletNodeName,inletNodeName);
    pipe.setString(Pipe_AdiabaticFields::OutletNodeName,outletNodeName);

    IdfExtensibleGroup eg = _equipmentBranch.pushExtensibleGroup();
    eg.setString(BranchExtensibleFields::ComponentObjectType,pipe.iddObject().name());
    eg.setString(BranchExtensibleFields::ComponentName,pipe.name().get());
    eg.setString(BranchExtensibleFields::ComponentInletNodeName,inletNodeName);
    eg.setString(BranchExtensibleFields::ComponentOutletNodeName,outletNodeName);
    eg.setString(BranchExtensibleFields::ComponentBranchControlType,"Bypass");
  }

  // Populate demand outlet branch

  IdfObject _demandOutletBranch(IddObjectType::Branch);
  _demandOutletBranch.setName(plantLoop.name().get() + " Demand Outlet Branch");
  _demandOutletBranch.clearExtensibleGroups();
  eg = _demandBranchList.pushExtensibleGroup();
  eg.setString(BranchListExtensibleFields::BranchName,_demandOutletBranch.name().get());

  _demandMixer.setString(Connector_MixerFields::OutletBranchName,_demandOutletBranch.name().get());

  m_idfObjects.push_back(_demandOutletBranch);

  std::vector<ModelObject> demandOutletModelObjects;
  std::vector<ModelObject> demandOutletBranchModelObjects;
  demandOutletModelObjects = plantLoop.demandComponents(demandMixer,demandOutletNode);  

  OS_ASSERT( demandOutletModelObjects.size() >= 2 );

  demandOutletModelObjects.erase(demandOutletModelObjects.begin());
  demandOutletModelObjects.erase(demandOutletModelObjects.end() - 1);

  for( auto & demandOutletModelObject : demandOutletModelObjects )
  {
    //nodes don't go onto branches, but still need to be translated and mapped
    //because doing so translates their setpoint managers
    if( boost::optional<Node> node = demandOutletModelObject.optionalCast<Node>() )
    {
      boost::optional<IdfObject> idfObject = this->translateAndMapModelObject(demandOutletModelObject);
    }
    //all other types of objects go onto the branch
    else
    {
      demandOutletBranchModelObjects.push_back(demandOutletModelObject);
    }
  }

  if( demandOutletBranchModelObjects.size() > 0 )
  {
    populateBranch( _demandOutletBranch,
                    demandOutletBranchModelObjects,
                    plantLoop );
  }
  else
  {
    IdfObject pipe(IddObjectType::Pipe_Adiabatic);
    pipe.setName(plantLoop.name().get() + " Demand Outlet Pipe");
    m_idfObjects.push_back(pipe);

    std::string inletNodeName = plantLoop.name().get() + " Demand Outlet Pipe Node";
    std::string outletNodeName = plantLoop.demandOutletNode().name().get();
    
    pipe.setString(Pipe_AdiabaticFields::InletNodeName,inletNodeName);
    pipe.setString(Pipe_AdiabaticFields::OutletNodeName,outletNodeName);

    IdfExtensibleGroup eg = _demandOutletBranch.pushExtensibleGroup();
    eg.setString(BranchExtensibleFields::ComponentObjectType,pipe.iddObject().name());
    eg.setString(BranchExtensibleFields::ComponentName,pipe.name().get());
    eg.setString(BranchExtensibleFields::ComponentInletNodeName,inletNodeName);
    eg.setString(BranchExtensibleFields::ComponentOutletNodeName,outletNodeName);
    eg.setString(BranchExtensibleFields::ComponentBranchControlType,"Passive");
  }

  // Populate PlantEquipmentList

  if( _optionalHeatingPlantEquipmentList )
  {
    for( auto & heatingComponent : heatingComponents )
    {
      boost::optional<IdfObject> _idfObject = translateAndMapModelObject(heatingComponent);

      if( _idfObject )
      {
        IdfExtensibleGroup eg = _optionalHeatingPlantEquipmentList->pushExtensibleGroup();
        eg.setString(PlantEquipmentListExtensibleFields::EquipmentObjectType,_idfObject->iddObject().name());
        eg.setString(PlantEquipmentListExtensibleFields::EquipmentName,_idfObject->name().get());
      }
    }
  }

  if( _optionalCoolingPlantEquipmentList )
  {
    for( auto & coolingComponent : coolingComponents )
    {
      boost::optional<IdfObject> _idfObject = translateAndMapModelObject(coolingComponent);

      if( _idfObject )
      {
        IdfExtensibleGroup eg = _optionalCoolingPlantEquipmentList->pushExtensibleGroup();
        eg.setString(PlantEquipmentListExtensibleFields::EquipmentObjectType,_idfObject->iddObject().name());
        eg.setString(PlantEquipmentListExtensibleFields::EquipmentName,_idfObject->name().get());
      }
    }
  }

  if( _optionalUncontrolledPlantEquipmentList )
  {
    for( auto & uncontrolledComponent : uncontrolledComponents )
    {
      boost::optional<IdfObject> _idfObject = translateAndMapModelObject(uncontrolledComponent);

      if( _idfObject )
      {
        IdfExtensibleGroup eg = _optionalUncontrolledPlantEquipmentList->pushExtensibleGroup();
        eg.setString(PlantEquipmentListExtensibleFields::EquipmentObjectType,_idfObject->iddObject().name());
        eg.setString(PlantEquipmentListExtensibleFields::EquipmentName,_idfObject->name().get());
      }
    }
  }

  if( _optionalSetpointOperation )
  {
    for( auto & setpointComponent : setpointComponents )
    {
      boost::optional<IdfObject> _idfObject = translateAndMapModelObject(setpointComponent.modelObject);

      OS_ASSERT(_idfObject);

      IdfExtensibleGroup eg = _optionalSetpointOperation->pushExtensibleGroup();
      eg.setString(PlantEquipmentOperation_ComponentSetpointExtensibleFields::EquipmentObjectType,_idfObject->iddObject().name());
      eg.setString(PlantEquipmentOperation_ComponentSetpointExtensibleFields::EquipmentName,_idfObject->name().get());
      eg.setString(PlantEquipmentOperation_ComponentSetpointExtensibleFields::DemandCalculationNodeName,setpointComponent.outletNode.name().get());
      eg.setString(PlantEquipmentOperation_ComponentSetpointExtensibleFields::SetpointNodeName,setpointComponent.outletNode.name().get());
      if( setpointComponent.isFlowRateAutosized )
      {
        eg.setString(PlantEquipmentOperation_ComponentSetpointExtensibleFields::ComponentFlowRate,"Autosize");
      }
      else
      {
        eg.setDouble(PlantEquipmentOperation_ComponentSetpointExtensibleFields::ComponentFlowRate,setpointComponent.flowRate);
      }
      switch(setpointComponent.type)
      {
        case HEATING :
          eg.setString(PlantEquipmentOperation_ComponentSetpointExtensibleFields::OperationType,"Heating");
          break;
        case COOLING :
          eg.setString(PlantEquipmentOperation_ComponentSetpointExtensibleFields::OperationType,"Cooling");
          break;
        default :
          eg.setString(PlantEquipmentOperation_ComponentSetpointExtensibleFields::OperationType,"Dual");
          break;
      }
    }
  }

  return boost::optional<IdfObject>(idfObject);
}

} // energyplus

} // openstudio

