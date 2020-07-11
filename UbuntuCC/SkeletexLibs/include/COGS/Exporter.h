/*
  Copyright (C) Skeletex Research, s.r.o. - All Rights Reserved
  Unauthorized copying of this file, via any medium is strictly prohibited
  Proprietary and confidential
*/
#pragma once
#include <vector>
#include <memory>
#include <string>

#include <COGS/API.h>



namespace cogs // forward declarations
{
  class PointCloud;
  class Scan;
  struct Mesh;
}



namespace cogs
{
  // #TODO Update tested file formats.
  /*!
    \brief
      Universal file exporter for COGS structures.

    Supports all COGS file extensions which can be found in \ref cogs-file-format-extensions.

    Internally uses Assimp library which supports common data formats found in
    http://assimp.sourceforge.net/main_features_formats.html. However,
    compatibility is not guaranteed for all file formats.

    Assimp export was tested on the following file formats: _in progress..._
    - Stanford Triangle Format (.ply)
    - Wavefront (.obj)
    - Stereo lithography (.stl)
  */
  class COGS_API Exporter
  {
  public:

    Exporter();
    Exporter(const Exporter &other) = delete;
    Exporter(const Exporter &&other) = delete;
    Exporter &operator=(const Exporter &other) = delete;
    Exporter &operator=(const Exporter &&other) = delete;
    ~Exporter();

    //! Saves all currently added data to a single file. Returns success.
    bool SaveToFile(const std::string &filename) const;

    //! Removes all previously added data from exporter.
    void Clear();

    //! Adds a referenced object to the list of exported objects.
    void AddPointCloud(const cogs::PointCloud *pointcloud);

    //! Adds a referenced object to the list of exported objects.
    void AddScan(const cogs::Scan *scan);

    //! Adds a referenced object to the list of exported objects.
    void AddMesh(const cogs::Mesh *mesh);

  private:
    struct Impl;
    std::unique_ptr<Impl> m;
    bool RunAssimpExporter(const std::string &filename) const;
    bool RunCogsExporter(const std::string &filename) const;
  };

}