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

#ifndef RUNMANAGER_LIB_MODELTORADJOB_HPP
#define RUNMANAGER_LIB_MODELTORADJOB_HPP

#include <boost/filesystem.hpp>
#include <string>
#include "../../utilities/core/Logger.hpp"
#include "Job_Impl.hpp"
#include "JobParam.hpp"
#include "../../utilities/core/Checksum.hpp"

#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QDateTime>

namespace openstudio {
namespace runmanager {
namespace detail {

  /**
   * Job type that converts a Model file into a rad input in process.
   */
  class ModelToRadJob : public Job_Impl
  {
    Q_OBJECT;

    public:
      /// ModelToRad constructor. 
      ModelToRadJob(const UUID &t_uuid,
          const Tools &tools,
          const JobParams &params,
          const Files &files,
      const JobState &t_restoreData);

      virtual ~ModelToRadJob();

      // Reimplemented virtual functions from Job_Impl
      virtual bool outOfDateImpl(const boost::optional<QDateTime> &t_lastrun) const override;
      virtual std::string description() const override;
      virtual Files outputFilesImpl() const override;
      virtual std::string getOutput() const override;
      virtual void cleanup() override;

      virtual void requestStop() override;

    protected:
      virtual void startImpl(const std::shared_ptr<ProcessCreator> &t_creator) override;
      virtual void basePathChanged() override;
      virtual void standardCleanImpl() override { /* nothing to do for this job type */ }


    private:
      REGISTER_LOGGER("openstudio.runmanager.ModelToRad");

      FileInfo modelFile() const;
      FileInfo sqlFile() const;

      mutable QReadWriteLock m_mutex;

      std::map<openstudio::path, FileTrack> m_files; //< Files tracked for outOfDate status
      boost::optional<FileInfo> m_model; //< Model that is being converted
      boost::optional<FileInfo> m_sql; //< SQL file from previous simulation run
      mutable boost::optional<Files> m_outputfiles; //< Rad files that were created from run

      std::string m_description; //< Description of job
  }; 

}
}
}
#endif // RUNMANAGER_LIB_MODELTORADJOB_HPP
