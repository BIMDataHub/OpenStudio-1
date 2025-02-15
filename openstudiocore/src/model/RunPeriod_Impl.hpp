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

#ifndef MODEL_RUNPERIOD_IMPL_HPP
#define MODEL_RUNPERIOD_IMPL_HPP


#include "ParentObject_Impl.hpp"

namespace openstudio {
namespace model {

namespace detail {

  class MODEL_API RunPeriod_Impl : public ParentObject_Impl {
   public:

    // constructor
    RunPeriod_Impl(const IdfObject& idfObject, Model_Impl* model, bool keepHandle);

    // construct from workspace
    RunPeriod_Impl(const openstudio::detail::WorkspaceObject_Impl& other,
                   Model_Impl* model,
                   bool keepHandle);

    // clone copy constructor
    RunPeriod_Impl(const RunPeriod_Impl& other,Model_Impl* model,bool keepHandle);

    // virtual destructor
    virtual ~RunPeriod_Impl(){}

    int getBeginMonth() const;
    int getBeginDayOfMonth() const;
    int getEndMonth() const;
    int getEndDayOfMonth() const;
    bool getUseWeatherFileHolidays() const;
    bool getUseWeatherFileDaylightSavings() const;
    bool getApplyWeekendHolidayRule() const;
    bool getUseWeatherFileRainInd() const;
    bool getUseWeatherFileSnowInd() const;
    int getNumTimePeriodRepeats() const;

    void setBeginMonth(int month);
    void setBeginDayOfMonth(int day);
    void setEndMonth(int month);
    void setEndDayOfMonth(int day);
    void setUseWeatherFileHolidays(bool use);
    void setUseWeatherFileDaylightSavings(bool use);
    void setApplyWeekendHolidayRule(bool apply);
    void setUseWeatherFileRainInd(bool rainInd);
    void setUseWeatherFileSnowInd(bool snowInd);
    void setNumTimePeriodRepeats(int numRepeats);

    // ensure that this object does not contain the date 2/29
    void ensureNoLeapDays();

    // return the parent object in the hierarchy
    virtual boost::optional<ParentObject> parent() const override;

    // set the parent, child may have to call methods on the parent
    virtual bool setParent(ParentObject& newParent) override;

    // return any children objects in the hierarchy
    virtual std::vector<ModelObject> children() const override;

    /// get a vector of allowable children types
    virtual std::vector<IddObjectType> allowableChildTypes() const override;

    // Get all output variable names that could be associated with this object.
    virtual const std::vector<std::string>& outputVariableNames() const override;

    virtual IddObjectType iddObjectType() const override;

    /** @name Queries */
    //@{

    bool isAnnual() const;

    bool isPartialYear() const;

    bool isRepeated() const;

    //@}

   private:

    REGISTER_LOGGER("openstudio.model.RunPeriod");
  };

} // detail

} // model
} // openstudio

#endif // MODEL_RUNPERIOD_IMPL_HPP
