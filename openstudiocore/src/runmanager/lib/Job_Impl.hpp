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

#ifndef RUNMANAGER_LIB_JOB_IMPL_HPP
#define RUNMANAGER_LIB_JOB_IMPL_HPP

#include <QObject>
#include <QThread>
#include <QFileInfo>
#include "../../utilities/core/Checksum.hpp"
#include "../../utilities/core/UUID.hpp"
#include "../../utilities/core/String.hpp"
#include "FileInfo.hpp"
#include "JobErrors.hpp"
#include "JobParam.hpp"
#include "JobType.hpp"
#include "ProcessCreator.hpp"
#include "TreeStatus.hpp"
#include "JobState.hpp"
#include <QReadWriteLock>
#include <QDateTime>

Q_DECLARE_METATYPE(QProcess::ExitStatus);
Q_DECLARE_METATYPE(QProcess::ProcessError);

namespace openstudio {
namespace runmanager {
  class MergedJobResults;

namespace detail {

  /// Public interface is defined in openstudio::runmanager::Job, see it for more details
  /// This interface is used by all Job implementations, such as EnergyPlusJob
  class Job_Impl : public QThread
  {
    Q_OBJECT

    public:
      Job_Impl(const UUID &t_id,
          const JobType &t_jobType,
          const Tools &t_tools,
          const JobParams &t_params,
          const Files &t_inputfiles,
          const JobState &t_restoreData);

      virtual ~Job_Impl();

      /// return all specific tools this job has appended onto the list of
      /// tools acquired from all dependencies
      Tools allTools() const;

      /// return all the specific params this job has appended onto the list of
      /// params acquired from all dependencies
      JobParams allParams() const;

      /// return all the specific input and output files this job has appended onto the list of
      /// inputs and outputs acquired from all dependencies
      Files allFiles() const;

      /// Returns the set of all files that can be considered input files for this job
      /// that is - all input and output files from all dependencies plus
      /// the set of input files provided to this job
      Files allInputFiles() const;

      /// \returns all of the tools from all of the children of this job
      Tools allChildTools() const;

      /// \returns all of the params from all of the children of this job
      JobParams allChildParams() const;

      /// \returns all of the input and output files from all of the children of this job
      Files allChildFiles() const;

      /// Return true if the job is out of date OR
      /// if any dependency is out of date OR if any dependency
      /// last executed before this job
      virtual bool outOfDate() const;

      /// Return true if the job processing has been canceled
      bool canceled() const;

      /// Set the canceled state of the job. It's up to the
      /// implementation to honor the canceled state
      void setCanceled(bool t_canceled);

      /// Return all output files generated by the job
      Files outputFiles() const;

      /// Return a vector of all input files the job was created with, with no base path application
      Files rawInputFiles() const;

      /// Return the input files used to create this job
      Files inputFiles() const;

      /// Return the tools used to create this job
      Tools tools() const;

      /// Return the params used to create this job
      JobParams params() const;

      /// Return all stdout ouput generated by the job
      virtual std::string getOutput() const = 0;

      /// Return datetime of the last run
      boost::optional<QDateTime> lastRun() const;

      /// Return the maximum date that any child was last run
      boost::optional<QDateTime> childrenLastRun() const;

      /// Return true if any child is running
      bool childrenRunning() const;

      /// Return true if any child's tree is running
      bool childrenTreesRunning() const;

      /// Return a user friendly description of the Job
      virtual std::string description() const = 0;

      /// return the UUID, UUID is stored in this base class
      UUID uuid() const;

      /// Return the JobType of the job, stored in the base class
      JobType jobType() const;

      /// Returns the last set of generated errors/warnings from the Job execution
      JobErrors errors() const;

      /// Remove all output files generated by the job
      virtual void cleanup() = 0;

      /// Display the errors generated by this job to the user
      void showErrors() const;

      /// Display the errors generated by this job and all children jobs to the user
      void showTreeErrors() const;

      /// Set the internal runnable state to true. Check runnable() documentation for what
      /// makes a job fully runnable.
      /// \param[in] force if set, the job will be runnable regardless of other states
      void setRunnable(bool force);

      /// \returns true if the job is out of date not canceled and is runnable OR if setRunnable(true)
      ///          had been called previously, forcing the runnable state to be true regardless of
      ///          out of date status or canceled status.
      bool runnable() const;

      /// \returns the detailed status information about the job
      AdvancedStatus status() const;

      /// Adds a new child job to the given job. All child jobs
      /// will be executed in parallel after the parent finishes
      /// Any child added will be reparented to this Job
      static void addChild(const std::shared_ptr<Job_Impl> &t_parent, const std::shared_ptr<Job_Impl> &t_job);

      /// Deparents the provided Job from this Job.
      /// \return false if t_job is not a child of this Job
      bool removeChild(const std::shared_ptr<Job_Impl> &t_job);

      /// Returns a vector of all children that this job has.
      std::vector<std::shared_ptr<Job_Impl> > children() const;

      /// Returns true if any of the regular child jobs are out of date
      bool childrenOutOfDate() const;

      /// Returns true if any of the regular child jobs' trees are out of date
      bool childrenTreesOutOfDate() const;

      /// Returns the collected set of errors generated from the children
      JobErrors childrenErrors() const;

      /// Returns the collected set of errors generated from the children and their trees
      JobErrors childrenTreesErrors() const;

      /// \returns the job finished job.
      /// \sa setFinishedJob
      std::shared_ptr<Job_Impl> finishedJob() const;

      /// Sets the "finishedJob" This is a job that executes after
      /// the parent and all parallel children have finished executing.
      static void setFinishedJob(const std::shared_ptr<Job_Impl> &t_parent,
          const std::shared_ptr<Job_Impl> &t_job);

      /// \returns the job's parent, if it exists
      std::shared_ptr<Job_Impl> parent() const;

      /// Sets the ProcessCreator to be used when the Job is executed
      void setProcessCreator(const std::shared_ptr<ProcessCreator> &t_pc);


      /// \returns the Job's index in the work queue. Or -1 if unset
      int index() const;

      /// \param[in] index Set the job's index in the work queue
      void setIndex(int index);

      /// Returns the output directory that should be used
      /// \param [in] th_throws throw if a proper outdir could not be built, otherwise return rootless path
      /// on error
      openstudio::path outdir(bool t_throws=false) const;

      /// Return a detailed description of the job
      virtual std::string detailedDescription() const;

      /// \returns the time the job was started
      boost::optional<QDateTime> startTime() const;

      /// \returns the time the job completed
      boost::optional<QDateTime> endTime() const;

      /// \returns the time the job was started
      boost::optional<openstudio::DateTime> osStartTime() const;

      /// \returns the time the job completed
      boost::optional<openstudio::DateTime> osEndTime() const;


      // Functions for getting information about the overall state of this job tree

      /// \returns true if this job or any job under it is running
      bool treeRunning() const;

      /// \returns a string describing the job tree from this point down
      std::string treeDescription() const;

      /// \returns true if this job or any job under it is out of date
      bool treeOutOfDate() const;

      /// \returns the amalgamation of all warnings / errors reported by this job and all child jobs
      JobErrors treeErrors() const;

      /// \returns the overall status of the job tree
      TreeStatusEnum treeStatus() const;

      /// \returns the contactenation of the job tree's detailed description fields
      std::vector<std::string> treeDetailedDescription() const;

      /** \returns all input and output files for all jobs in this job tree. */
      Files treeAllFiles() const;

      /// \returns all output files generated by all jobs in this job tree
      Files treeOutputFiles() const;

      /// \returns all output files relative to this Job's working dir
      Files relativeOutputFiles() const;


      /// \returns the time the tree was completed running. Only returns a value if all jobs in the tree have a
      ///          last run time and reutrns the latest of the returned values
      boost::optional<QDateTime> treeLastRun() const;

      /// Sets the runnable state for this job and all child jobs
      void setTreeRunnable(bool force);

      /// Set the canceled state of this job and all child jobs
      void setTreeCanceled(bool t_canceled);


      /// Merge the t_job into the current job, if possible.
      /// If not possible, throw MergeJobError with description as to why.
      /// accepts NullJobs automatically, passes off 
      /// to mergeJobImpl if otherwise.
      void mergeJob(const std::shared_ptr<Job_Impl> &t_parent, const std::shared_ptr<Job_Impl> &t_job);

      /// Requests that the job stop
      virtual void requestStop() = 0;

      /// \returns the history of each status change with high resolution timestamps
      std::vector<std::pair<boost::posix_time::ptime, AdvancedStatus> > history() const;

      /// \returns the base path for this process execution
      openstudio::path getBasePath() const;

      /// Sets the base path by which relative paths will be evaluated from this job
      void setBasePath(const openstudio::path &t_basePath);

      /// Sets the base path for this job and all child jobs
      void setBasePathRecursive(const openstudio::path &t_basePath);

      /// Update this job tree with the details from the other job tree
      void updateJob(const std::shared_ptr<Job_Impl> &t_other, bool t_allowUUIDUpdate);


      /// \returns true if this job is externallyManaged
      bool externallyManaged() const;

      // send job state and file output signals as if the job had gone from no state
      // to the current state
      void sendSignals();

      /// sets this job (and children) as being externally managed
      void makeExternallyManaged();

      /// Sets the advancedstatus of the current job. Only allowed on externally managed jobs
      void setStatus(const AdvancedStatus &t_status);

      bool hasMergedJobs() const;
      std::vector<MergedJobResults> mergedJobResults() const;


    protected:
      /// Called when the base path has changed
      virtual void basePathChanged() = 0;

      /// Overridable method for providing job merging capabilities.
      /// By default only throw 
      virtual void mergeJobImpl(const std::shared_ptr<Job_Impl> &t_parent, const std::shared_ptr<Job_Impl> &t_job);

      virtual Files outputFilesImpl() const = 0;

      /// Implementation of QThread::run()
      virtual void run() override;

      /// Begin execution of the job
      virtual void startImpl(const std::shared_ptr<ProcessCreator> &t_pc) = 0;

      /// Return true if the job is out of date, needs to be implemented
      /// by base classes.
      /// \sa Job_Impl::outOfDate
      virtual bool outOfDateImpl(const boost::optional<QDateTime> &t_lastrun) const = 0;

      /// Update the errors object
      void setErrors(const JobErrors &t_e);

      /// Emits the started() signal
      void emitStarted();

      /// Emits the outputFileChanged(const openstudio::runmanager::FileInfo &) signal
      void emitOutputFileChanged(const openstudio::runmanager::FileInfo& file);

      /// Emits the stateChanged() signal
      void emitStateChanged() const;

      /// Emits the outputDataAdded(const std::string &) signal
      void emitOutputDataAdded(const std::string &data);

      /// Emits the statusChanged signal
      void emitStatusChanged(const openstudio::runmanager::AdvancedStatus &status);

      /// Emits the childrenChanged signal
      void emitChildrenChanged();

      /// Update parent, simply changes out the pointer and emits the signal
      void setParent(const std::shared_ptr<Job_Impl> &impl);

      /// \returns a default description for this job
      std::string buildDescription(const std::string &extension) const;

      /// Sets a new set of JobParams for this Job
      void setParams(const openstudio::runmanager::JobParams &t_newParams);

      /// Sets a new set of Files for this Job
      void setFiles(const openstudio::runmanager::Files &t_newFiles);

      virtual void standardCleanImpl() = 0;

      struct FileTrack
      {
        std::string checksum;
        QDateTime lastModified;

        explicit FileTrack(const openstudio::path &t_path)
          : checksum(openstudio::checksum(t_path)),
            lastModified(QFileInfo(openstudio::toQString(t_path)).lastModified())
        {
        }

        FileTrack()
        {
        }

      };

      /// adds a file to the list of files being tracked
      static void addFile(std::map<openstudio::path, FileTrack> &t_files, const openstudio::path &t_path);

      /// \returns true if the files have changed since the last run
      static bool filesChanged(const std::map<openstudio::path, FileTrack> &t_files, const QDateTime &t_lastRun);
      static void resetFiles(std::map<openstudio::path, FileTrack> &t_files);
      static void resetFiles(std::map<openstudio::path, FileTrack> &t_files, const boost::optional<FileInfo> &t_file);
      static void resetFiles(std::map<openstudio::path, FileTrack> &t_files, const boost::optional<FileInfo> &t_file, const boost::optional<FileInfo> &t_file2);

      virtual bool hasMergedJobsImpl() const;
      virtual std::vector<MergedJobResults> mergedJobResultsImpl() const;


      /// Updates the list of tracked files for timestamps and checksums
//      void updateFiles(std::map<openstudio::path, FileTrack> &t_files);

    private slots:

      void threadFinished();

    signals:
      /// Emitted after the job has started
      void started(const openstudio::UUID &id);

      /// Emitted after the job has finished
      void finished(const openstudio::UUID &id, const openstudio::runmanager::JobErrors& errors);

      /// Emitted after the job has finished
      void finishedExt(const openstudio::UUID &id, const openstudio::runmanager::JobErrors& errors, 
          const openstudio::DateTime &lastRun, const std::vector<openstudio::runmanager::FileInfo> &outputfiles);

      /// Emitted after generated output file has changed.
      void outputFileChanged(const openstudio::UUID &id, const openstudio::runmanager::FileInfo& file);

      /// Emitted after the state (runnable/canceled/outofdate) or index of a job has changed
      void stateChanged(const openstudio::UUID &id) const;

      /// Emitted after any data has been sent to stdout from the job, does not wait for
      /// complete lines
      void outputDataAdded(const openstudio::UUID &id, const std::string &data);

      /// Emitted after new detailed status information becomes available
      void statusChanged(const openstudio::runmanager::AdvancedStatus &status);

      /// Emitted after the list of children or the finished job child changes
      void childrenChanged(const openstudio::UUID &id);

      /// Emitted after the the parent of this job changes
      void parentChanged(const openstudio::UUID &id);

      /// Emitted whenever this state of this job tree (this job and any child) changes
      void treeChanged(const openstudio::UUID &id) const;

      /// Emitted when a Job's UUID has changed via a updateJob() call
      void uuidChanged(const openstudio::UUID &t_oldUUID, const openstudio::UUID &t_newUUID);


    private:
      REGISTER_LOGGER("openstudio.runmanager.Job_Impl");

      enum CleanType {
        none,
        standard,
        maximum
      };

      CleanType m_cleanTypeToRun;
      void doCleanUp();

      boost::optional<QDateTime> lastRunInternal() const;

      mutable QReadWriteLock m_mutex;
      mutable QReadWriteLock m_cacheMutex;

      /// Emits the finished(const openstudio::runmanager::JobErrors &) signal
      void emitFinished(const openstudio::runmanager::JobErrors &t_e, const boost::optional<QDateTime> &t_lastRun, const openstudio::runmanager::Files &t_outputFiles);

      /// Log and set
      void addChildInternal(const std::shared_ptr<Job_Impl> &t_child);

      /// Lock and set
      void setFinishedJobInternal(const std::shared_ptr<Job_Impl> &t_job);

      /// Run the out of date assuming we are already inside of a thread, to avoid race conditions
      /// in relocking a thread that's already locked for read locking.
      bool outOfDateInternal(const std::shared_ptr<Job_Impl> &t_parent, const boost::optional<QDateTime> &t_lastrun) const;

      /// Return the index of this job in the list of children. Throw exception if the job is not found
      size_t childIndexInList(const UUID &t_uuid) const;

      static TreeStatusEnum compareTreeStatus(const TreeStatusEnum &lhs, const TreeStatusEnum &rhs);

      static bool fileComparitor(const openstudio::path &t_lhs, const openstudio::path &t_rhs);

      void maximumClean();

      void standardClean();

      void sendSignals(JobState oldState, JobState newState, const openstudio::UUID &t_oldUUID, const openstudio::UUID &t_newUUID);

      std::vector<FileInfo> relativeOutputFilesInternal(const openstudio::runmanager::Files &t_outputFiles, const openstudio::path &t_outdir) const;

      UUID m_id; //< Job's UUID
      JobType m_jobType; //< Job's type
      int m_index; //< Job's index in the queue

      Tools m_tools; //< Provided tools
      JobParams m_params; //< Provided params
      Files m_inputFiles; //< Provided input files

      JobErrors m_errors; //< Collected JobErrors
      mutable volatile bool m_outOfDate; //< cached OutOfDate value
      volatile bool m_canceled; //< canceled state default false
      volatile bool m_runnable; //< runnable state default true
      volatile bool m_force;    //< force state default false
      AdvancedStatus m_status; //< default idle

      std::shared_ptr<ProcessCreator> m_processCreator; //< ProcessCreator used during execution

      std::weak_ptr<Job_Impl> m_parent; //< parent of this job
      std::shared_ptr<Job_Impl> m_finishedJob; //< Job to execute after this job and all its children have finished
      std::vector<std::shared_ptr<Job_Impl> > m_children; //< List of children that this job has

      boost::optional<QDateTime> m_lastStartTime;
      boost::optional<QDateTime> m_lastEndTime;

      boost::optional<openstudio::DateTime> m_osLastStartTime;
      boost::optional<openstudio::DateTime> m_osLastEndTime;

      boost::optional<QDateTime> m_lastRun;

      std::vector<std::pair<boost::posix_time::ptime, AdvancedStatus> > m_history;

      openstudio::path m_basePath; //< Path from which relative paths in this job will be evaluated

      JobState m_jobState;
      bool m_hasRunSinceLoading;
      bool m_externallyManaged;

      mutable boost::optional<Tools> m_allTools;
      mutable boost::optional<JobParams> m_allParams;
      mutable boost::optional<Files> m_allInputFiles;
      mutable boost::optional<openstudio::path> m_outdir;
  };

}
}
}

#endif // RUNMANAGER_LIB_JOB_IMPL_HPP
