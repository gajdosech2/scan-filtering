#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include "PointFeatures.h"
#include "TrimValues.h"

namespace cogs
{
  class Scan;
}

class PointCleanNetFormatter
{
public:
    PointCleanNetFormatter() : min_intensity_(INFINITY), max_intensity_(-INFINITY), min_normal_(INFINITY), max_normal_(-INFINITY), min_depth_(INFINITY), max_depth_(-INFINITY) {};
    PointCleanNetFormatter(std::string path);
    bool Import(std::string path);
    void GenerateInputFile();
    void GenerateOutliersFile(std::string truth_path);
    TrimValues FindTrimValues();
    void Trim(TrimValues trim_values);
    void Trim();
    std::vector<std::vector<PointFeatures>> GetData() { return data_; };
    TrimValues GetLastTrimValues() { return last_trim_values_; }
private:
    void PrepareFileName(std::string path);
    std::vector<std::vector<PointFeatures>> data_;
    TrimValues last_trim_values_;
    std::string file_name_;

    float min_intensity_;
    float max_intensity_;
    float min_normal_;
    float max_normal_;
    float min_depth_;
    float max_depth_;
};