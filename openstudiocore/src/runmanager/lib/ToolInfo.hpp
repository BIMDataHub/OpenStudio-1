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

#ifndef RUNMANAGER_LIB_TOOLINFO_HPP
#define RUNMANAGER_LIB_TOOLINFO_HPP

#include "RunManagerAPI.hpp"

#include <string>
#include "../../utilities/core/Path.hpp"
#include "../../utilities/core/Logger.hpp"
#include <boost/regex.hpp>
#include <algorithm>

namespace openstudio {
namespace runmanager {

  /// Utility for holding and comparing ToolVersion numbers
  class RUNMANAGER_API ToolVersion
  {
    public:
      ToolVersion();

      ToolVersion(int t_major);

      ToolVersion(int t_major, int t_minor);

      ToolVersion(int t_major, int t_minor, int t_build);

      ToolVersion(int t_major, int t_minor, int t_build, std::string t_tag);

      ToolVersion(boost::optional<int> t_major, boost::optional<int> t_minor, boost::optional<int> t_build, boost::optional<std::string> t_tag);

      /// \returns true if no part of the ToolVersion info is set
      bool empty() const;

      /// Compare for exact equality. 
      /// \returns true if major, minor and build all match 
      bool operator==(const ToolVersion &rhs) const;

      /// Compare less than.
      /// \returns true if any element is less than the corresponding element of rhs 
      ///          or if the LHS element (major,minor,build) is not defined
      bool operator<(const ToolVersion &rhs) const;
      
      /// Compare the requirements to the current object
      /// For each element that "requirements" has set. If an element in requirements is not
      /// set, then any version number for that element will be accepted.
      /// \param[in] requirements requirements to check against
      /// \returns true if the object matches the requirements
      bool matches(const ToolVersion &requirements) const;

      /// Return a string representation of the version object
      std::string toString() const;

      static ToolVersion fromString(const std::string &);

      /// \returns the major version number
      boost::optional<int> getMajor() const;

      /// \returns the minor version number
      boost::optional<int> getMinor() const;

      /// \returns the build version number
      boost::optional<int> getBuild() const;

      /// \returns the build tag (SHA/MD5/whatever) if set
      boost::optional<std::string> getTag() const;

    private:
      boost::optional<int> m_major;
      boost::optional<int> m_minor;
      boost::optional<int> m_build;
      boost::optional<std::string> m_tag;
  };


  /// Operator overload for streaming ToolVersion to ostream
  RUNMANAGER_API std::ostream &operator<<(std::ostream &os, const ToolVersion &tv);

  /// Used for holding details about a tool binary location.
  struct RUNMANAGER_API ToolInfo
  {
    /// Default constructor, not really a valid ToolInfo object
    ToolInfo()
    {
    }

    /** Create a ToolInfo object
     *  \param[in] t_name Name of tool
     *  \param[in] t_version Tool version identifier
     *  \param[in] t_localBinPath Binary location for running locally
     *  \param[in] t_outFileFilter Filter to use when determining which files were generated by 
     *  this tool */
    ToolInfo(const std::string &t_name,
             const ToolVersion &t_version,
             const openstudio::path &t_localBinPath,
             const boost::regex &t_outFileFilter)
      : name(t_name), version(t_version), localBinPath(t_localBinPath),
        outFileFilter(t_outFileFilter)
    {
    }

    /// Create a ToolInfo object. Useful only for locally executing jobs
    /// \param[in] t_name Name of tool
    /// \param[in] t_localBinPath Binary location for running locally
    ToolInfo(const std::string &t_name,
             const openstudio::path &t_localBinPath)
      : name(t_name), localBinPath(t_localBinPath)
    {
    }

    /// Create a ToolInfo object. Useful only for locally executing jobs
    /// \param[in] t_name Name of tool
    /// \param[in] t_version Version info
    /// \param[in] t_localBinPath Binary location for running locally
    ToolInfo(const std::string &t_name,
             const ToolVersion &t_version,
             const openstudio::path &t_localBinPath)
      : name(t_name), version(t_version), localBinPath(t_localBinPath)
    {
    }

    /// Simplest ToolInfo constructor, useful only for locally executing jobs
    /// \param[in] t_localBinPath Binary location for running locally
    ToolInfo(const openstudio::path &t_localBinPath)
      : localBinPath(t_localBinPath)
    {
    }

    bool operator<(const ToolInfo &ti) const
    {
      return name < ti.name 
         || (name == ti.name && version < ti.version);
    }

    bool operator==(const ToolInfo &t_rhs) const
    {
      return name == t_rhs.name
        && version == t_rhs.version
        && localBinPath == t_rhs.localBinPath
        && outFileFilter == t_rhs.outFileFilter;
    }


    std::string name;
    ToolVersion version;
    openstudio::path localBinPath;
    boost::regex outFileFilter;
  };

  /// Represents a collection of ToolInfo objects
  class RUNMANAGER_API Tools
  {
    public:
      Tools()
      {
      }

      /// Constructs a Tools object with the given vector of ToolInfo objects
      Tools(const std::vector<ToolInfo> &t_tools)
        : m_tools(t_tools)
      {}

      bool operator==(const Tools &t_rhs) const
      {
        return std::equal(m_tools.begin(), m_tools.end(), t_rhs.m_tools.begin());
      }

      /// Appends a ToolInfo object to this collection of ToolInfo objects
      void append(const ToolInfo &fi)
      {
        m_tools.push_back(fi);
      }

      /// Appends the collection of ToolInfo objects in the Tools parameters 
      /// to this collection of ToolInfo objects
      void append(const Tools &f)
      {
        append(f.m_tools.begin(), f.m_tools.end());
      }

      /// Returns the collection of ToolInfo objects in this object as a vector
      std::vector<ToolInfo> tools() const
      {
        return m_tools;
      }

      /// Returns the last ToolInfo in the collection matching the given name
      ToolInfo getLastByName(const std::string &t_name) const;

      /// Returns all ToolInfo objects in the collection that match the given name
      Tools getAllByName(const std::string &t_name) const;

      /// Returns the last ToolInfo object in the collection that match the given name
      /// 
      /// \sa openstudio::runmanager::ToolVersion for information on how ToolVersion
      ///                                         objects are compared
      ToolInfo getLastByVersion(const ToolVersion &t_version) const;

      /// Returns all ToolInfo objects in the collection that match the given name
      /// 
      /// \sa openstudio::runmanager::ToolVersion for information on how ToolVersion
      ///                                         objects are compared
      Tools getAllByVersion(const ToolVersion &t_version) const;

      /**
       * Returns the tool with the given name and the highest version number
       */
      ToolInfo getTool(const std::string &t_name) const
      {
        Tools ts = getAllByName(t_name);
        std::sort(ts.m_tools.begin(), ts.m_tools.end(), &Tools::versionLessThan);

        if (!ts.m_tools.empty())
        {
          return *ts.m_tools.rbegin();
        } else {
          throw std::runtime_error("No tool matching: " + t_name);
        }
      }


      /**
       * Returns the tool with the given name and the highest version number
       * that matches the passed in requirements.
       */
      ToolInfo getTool(const std::string &t_name, const ToolVersion &t_requirements) const
      {
        LOG(Debug, "getTool called: " << t_name << " " << t_requirements.toString());
        LOG(Debug, "total tools: " << m_tools.size());
        Tools ts = getAllByName(t_name);
        LOG(Debug, "getAllByName returns " << ts.tools().size());

        Tools byversion = ts.getAllByVersion(t_requirements);
        LOG(Debug, "getAllByVersion returns " << byversion.tools().size());

        std::sort(byversion.m_tools.begin(), byversion.m_tools.end(), &Tools::versionLessThan);

        if (!byversion.m_tools.empty())
        {
          return *byversion.m_tools.rbegin();
        } else {
          throw std::runtime_error("No tool matching: " + t_name + " " + t_requirements.toString());
        }
      }


    private:
      REGISTER_LOGGER("openstudio.runmanager.Tools");

      static bool nameCompare(const std::string &t_name, const ToolInfo &ti)
      {
        return ti.name == t_name;
      }

      static bool versionLessThan(const ToolInfo &lhs, const ToolInfo &rhs)
      {
        return lhs.version < rhs.version;
      }

      static bool versionCompare(const ToolVersion &t_requirements, const ToolInfo &ti)
      {
        return ti.version.matches(t_requirements);
      }

      template<typename InItr>
        void append(const InItr &begin, const InItr &end)
        {
          m_tools.insert(m_tools.end(), begin, end);
        }

      ToolInfo getLast(const std::function<bool (const ToolInfo &)> &f) const
      {
        auto itr = std::find_if(m_tools.rbegin(), m_tools.rend(), f);

        if (itr != m_tools.rend())
        {
          return *itr;
        }

        throw std::out_of_range("ToolInfo not found");
      }

      Tools getAll(const std::function<bool (const ToolInfo &)> &f) const
      {
        auto itr = m_tools.begin();
        const std::vector<openstudio::runmanager::ToolInfo>::const_iterator end = m_tools.end();

        Tools ret;

        while (itr != end)
        {
          if (f(*itr))
          {
            ret.append(*itr);
          }
          ++itr;
        }

        return ret;
      }


      std::vector<ToolInfo> m_tools;
  };


} // runmanager
} // openstudio

#endif // RUNMANAGER_LIB_TOOLINFO_HPP

