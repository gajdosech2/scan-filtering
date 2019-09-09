#pragma once
#pragma warning(disable: 4996)
#define GLM_ENABLE_EXPERIMENTAL

#include "..\HeaderFiles\NeuralNetworkFormatter.h"
#include "..\HeaderFiles\ImageGenerator.h"
#include <COGS/Scan.h>
#include <fstream>
#include <direct.h>

NeuralNetworkFormatter::NeuralNetworkFormatter(std::string path) : min_intensity_(INFINITY), max_intensity_(-INFINITY), min_normal_(INFINITY), max_normal_(-INFINITY)
{
  Import(path);
}

bool NeuralNetworkFormatter::Import(std::string path)
{
  cogs::Scan scan;
  path = "DataSets/" + path;
  scan.Import(path);
  uint32_t width = scan.GetWidth();
  uint32_t height = scan.GetHeight();
  auto positions = scan.GetPositions();
  auto normals = scan.GetNormals();
  auto intensities = scan.GetIntensities();
  for (uint32_t i = 0; i < height; i++)
  {
    std::vector<PointFeatures> row;
    for (uint32_t j = 0; j < width; j++)
    {
      auto coord = glm::uvec2(j, i);
      if (scan.IsPointAt(coord))
      {
        auto index = scan.GetPointAt(coord);
        auto normal = normals[index];
        auto intensity = intensities[index];
        auto depth = positions[index].z;
        PointFeatures point_feature(normal.x, normal.y, normal.z, intensity, depth);
        row.push_back(point_feature);

        min_intensity_ = std::min(min_intensity_, intensity);
        max_intensity_ = std::max(max_intensity_, intensity);
        min_normal_ = std::min(min_normal_, normal.x);
        min_normal_ = std::min(min_normal_, normal.y);
        min_normal_ = std::min(min_normal_, normal.z);
        max_normal_ = std::max(max_normal_, normal.x);
        max_normal_ = std::max(max_normal_, normal.y);
        max_normal_ = std::max(max_normal_, normal.z);
      }
      else
      {
        row.push_back(PointFeatures());
      }
    }
    data_.push_back(row);
  }

  PrepareFileName(path);
  return true;
}

void NeuralNetworkFormatter::GenerateImageFiles()
{
	ImageGenerator image_generator(file_name_, &data_);
	image_generator.GenerateBinaryMap();
	image_generator.GenerateGrayMap(max_intensity_, min_intensity_);
	image_generator.GenerateNormalMap(max_normal_, min_normal_);
}

TrimValues NeuralNetworkFormatter::FindTrimValues()
{
  size_t height = data_.size();
  size_t width = data_[0].size();
  uint32_t min_x = (uint32_t)width;
  uint32_t min_y = (uint32_t)height;
  uint32_t max_x = 0;
  uint32_t max_y = 0;
  for (uint32_t i = 0; i < height; i++)
  {
    for (uint32_t j = 0; j < width; j++)
    {
      if (data_[i][j].intensity > EMPTY_POINT_INTENSITY)
      {
        min_y = std::min(min_y, i);
        max_y = std::max(max_y, i);
        min_x = std::min(min_x, j);
        max_x = std::max(max_x, j);
      }
    }
  }
  return TrimValues(min_x, max_x, min_y, max_y);
}

void NeuralNetworkFormatter::Trim(TrimValues trim_values)
{
  last_trim_values_ = trim_values;
  std::vector<std::vector<PointFeatures>> new_data;
  for (uint32_t i = trim_values.min_y; i <= trim_values.max_y; i++)
  {
    std::vector<PointFeatures> row;
    for (uint32_t j = trim_values.min_x; j <= trim_values.max_x; j++)
    {
      row.push_back(data_[i][j]);
    }
    new_data.push_back(row);
  }
  data_ = new_data;
}

void NeuralNetworkFormatter::Trim()
{
  Trim(FindTrimValues());
}

void NeuralNetworkFormatter::Pad(int size)
{
  int height = (int)data_.size();
  int width = (int)data_[0].size();
  int dif_x = size - width;
  int dif_y = size - height;
  if (dif_x < 0 || dif_y < 0)
  {
    return;
  }
  int left_pad = dif_x / 2;
  int right_pad = dif_x / 2;
  if (dif_x % 2 != 0) {
	  right_pad = dif_x / 2 + 1;
  }
  int top_pad = dif_y / 2;
  int bottom_pad = dif_y / 2;
  if (dif_y % 2 != 0) {
	  bottom_pad = dif_y / 2 + 1;
  }
  std::vector<std::vector<PointFeatures>> new_data;
  for (int i = -top_pad; i < height + bottom_pad; i++)
  {
    std::vector<PointFeatures> row;
    for (int j = -left_pad; j < width + right_pad; j++)
    {
      if (i < 0 || j < 0 || i >= height || j >= width)
      {
        row.push_back(PointFeatures());
      }
      else
      {
        row.push_back(data_[i][j]);
      }
    }
    new_data.push_back(row);
  }
  data_ = new_data;
}

std::vector<PointFeatures> NeuralNetworkFormatter::GetNeighborhood(int i, int j, int tile_size)
{
  int pad = tile_size / 2;
  std::vector<PointFeatures> neighborhood;
  for (int y = i - pad; y <= i + pad; y++)
  {
    for (int x = j - pad; x <= j + pad; x++)
    {
      neighborhood.push_back(data_[y][x]);
    }
  }
  return neighborhood;
}

void NeuralNetworkFormatter::PrepareFileName(std::string path)
{
  file_name_ = "";
  bool flag = false;
  for (int i = (int)path.size(); i >= 0; i--)
  {
    if (path[i] == '/' || path[i] == '\\')
    {
      break;
    }
    if (flag)
    {
      file_name_ = path[i] + file_name_;
    }
    if (path[i] == '.')
    {
      flag = true;
    }
  }
}

void NeuralNetworkFormatter::GenerateDataFiles(int tile_size)
{
  size_t height = data_.size();
  size_t width = data_[0].size();
  int outer_pad = tile_size / 2;
  int counter = 0;
  size_t size = height * width;
  auto path = "../Model/NeuralNetworkFiles/" + file_name_ + "_data";
  bool outer_dir_created = _mkdir("../Model/NeuralNetworkFiles");
  bool dir_removed = _rmdir(path.c_str());
  bool dir_created = _mkdir(path.c_str());
  for (uint32_t i = outer_pad; i < height - outer_pad; i++)
  {
    for (uint32_t j = outer_pad; j < width - outer_pad; j++)
    {
      if (counter % 1000 == 0)
      {
        std::cout << counter << " out of " << size << std::endl;
      }
      if (data_[i][j].intensity > EMPTY_POINT_INTENSITY)
      {
        std::ofstream data_file;
        data_file.open("../Model/NeuralNetworkFiles/" + file_name_ + "_data/" + std::to_string(counter) + ".csv");
        data_file << counter << std::endl;
        auto neighborhood = GetNeighborhood(i, j, tile_size);
        for (PointFeatures &point : neighborhood)
        {
          data_file << point.normal_x << ","
            << point.normal_y << ","
            << point.normal_z << ","
            << point.intensity << ","
            << point.depth << std::endl;
        }
        data_file.close();
      }
      counter++;
    }
  }
}

void NeuralNetworkFormatter::GenerateTruthFile(std::string truth_path, int tile_size)
{
  int outer_pad = tile_size / 2;
  NeuralNetworkFormatter truth(truth_path);
  auto truth_data = truth.GetData();
  if (truth_data.size() != data_.size() || truth_data[0].size() != data_[0].size())
  {
    truth.Trim(last_trim_values_);
    truth.Pad((int)data_.size());
    truth_data = truth.GetData();
  }
  std::ofstream prediction;
  prediction.open("../Model/NeuralNetworkFiles/" + file_name_ + "_truth.csv");
  size_t height = data_.size();
  size_t width = data_[0].size();
  int counter = 0;
  for (uint32_t i = outer_pad; i < height - outer_pad; i++)
  {
    for (uint32_t j = outer_pad; j < width - outer_pad; j++)
    {
      if (data_[i][j].intensity > EMPTY_POINT_INTENSITY)
      {
        prediction << counter << "," << (truth_data[i][j].intensity > EMPTY_POINT_INTENSITY) << std::endl;
      }
      counter++;
    }
  }
  prediction.close();
}
