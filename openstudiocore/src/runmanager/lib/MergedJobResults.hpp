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

#ifndef RUNMANAGER_LIB_MERGEDJOBRESULTS_HPP
#define RUNMANAGER_LIB_MERGEDJOBRESULTS_HPP

#include "RunManagerAPI.hpp"
#include "../../utilities/core/UUID.hpp"
#include "FileInfo.hpp"
#include "JobParam.hpp"
#include "JobErrors.hpp"


namespace openstudio {
namespace runmanager {

  class RUNMANAGER_API MergedJobResults
  {
    public:
      MergedJobResults(const openstudio::UUID &t_uuid, const JobErrors &t_errors, const Files &t_outputFiles)
        : uuid(t_uuid), errors(t_errors), outputFiles(t_outputFiles) {}
      MergedJobResults() {}

      openstudio::UUID uuid;
      JobErrors errors;
      Files outputFiles;
  };

}
}


#endif // RUNMANAGER_LIB_MERGEDJOBRESULTS_HPP

