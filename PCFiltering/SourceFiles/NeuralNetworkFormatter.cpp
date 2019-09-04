#pragma once
#pragma warning(disable: 4996)
#define GLM_ENABLE_EXPERIMENTAL

#include "..\HeaderFiles\NeuralNetworkFormatter.h"
#include <COGS/Scan.h>
#include <fstream>


NeuralNetworkFormatter::NeuralNetworkFormatter(std::string path) : min_intensity_(INFINITY), max_intensity_(-INFINITY), min_normal_(INFINITY), max_normal_(-INFINITY)
{
  Import(path);
}

bool NeuralNetworkFormatter::Import(std::string path)
{
  cogs::Scan scan;
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

bool NeuralNetworkFormatter::GenerateBinaryMap()
{
  if (data_.size() == 0 || data_[0].size() == 0)
  {
    return false;
  }
  size_t height = data_.size();
  size_t width = data_[0].size();
  std::ofstream image;
  image.open("Visualizations/" + file_name + "_bitmap.pbm");
  image << "P1" << std::endl;
  image << width << " " << height << std::endl;
  for (uint32_t i = 0; i < height; i++)
  {
    for (uint32_t j = 0; j < width; j++)
    {
      image << (data_[i][j].intensity != 0) << " ";
    }
    image << std::endl;
  }
  image.close();
  return true;
}

bool NeuralNetworkFormatter::GenerateGrayMap()
{
  if (data_.size() == 0 || data_[0].size() == 0)
  {
    return false;
  }
  int gray_levels = 255;
  size_t height = data_.size();
  size_t width = data_[0].size();
  std::ofstream image;
  image.open("Visualizations/" + file_name + "_bumpmap.pgm");
  image << "P2" << std::endl;
  image << width << " " << height << std::endl;
  image << gray_levels << std::endl;
  float a = (gray_levels * 2) / (max_intensity_ - min_intensity_);
  float b = (gray_levels * 2) - a * max_intensity_;
  for (uint32_t i = 0; i < height; i++)
  {
    for (uint32_t j = 0; j < width; j++)
    {
      int normalized_intensity = std::max(0, std::min((int)(a * data_[i][j].intensity + b), 255));
      image << 255 - normalized_intensity << " ";
    }
    image << std::endl;
  }
  image.close();
  return true;
}

bool NeuralNetworkFormatter::GenerateNormalMap()
{
  size_t height = data_.size();
  size_t width = data_[0].size();
  std::ofstream image;
  image.open("Visualizations/" + file_name + "_normalmap.ppm");
  image << "P3" << std::endl;
  image << width << " " << height << std::endl;
  image << 255 << std::endl;
  float a = (255) / (max_normal_ - min_normal_);
  float b = 255 - a * max_normal_;
  for (uint32_t i = 0; i < height; i++)
  {
    for (uint32_t j = 0; j < width; j++)
    {
      int normalized_x = a * data_[i][j].normal_x + b;
      int normalized_y = a * data_[i][j].normal_y + b;
      int normalized_z = a * data_[i][j].normal_z + b;
      image << normalized_x << " " << normalized_y << " " << normalized_z << " ";
    }
    image << std::endl;
  }
  image.close();
  return true;
}

void NeuralNetworkFormatter::Trim()
{
  size_t height = data_.size();
  size_t width = data_[0].size();
  uint32_t min_x = width;
  uint32_t min_y = height;
  uint32_t max_x = 0;
  uint32_t max_y = 0;
  for (uint32_t i = 0; i < height; i++)
  {
    for (uint32_t j = 0; j < width; j++)
    {
      if (data_[i][j].intensity != 0)
      {
        min_y = std::min(min_y, i);
        max_y = std::max(max_y, i);
        min_x = std::min(min_x, j);
        max_x = std::max(max_x, j);
      }
    }
  }
  std::vector<std::vector<PointFeatures>> new_data;
  for (uint32_t i = min_y; i <= max_y; i++)
  {
    std::vector<PointFeatures> row;
    for (uint32_t j = min_x; j < max_x; j++)
    {
      row.push_back(data_[i][j]);
    }
    new_data.push_back(row);
  }
  data_ = new_data;
}

void NeuralNetworkFormatter::Pad(int size)
{
  int height = data_.size();
  int width = data_[0].size();
  int dif_x = size - width;
  int dif_y = size - height;

  if (dif_x < 0 || dif_y < 0)
  {
    return;
  }

  int pad_x = dif_x / 2;
  int pad_y = dif_y / 2;
  int size_x = width + pad_x;
  int size_y = height + pad_y;

  std::vector<std::vector<PointFeatures>> new_data;
  for (int i = -pad_y; i < size_y; i++)
  {
    std::vector<PointFeatures> row;
    for (int j = -pad_x; j < size_x; j++)
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

void NeuralNetworkFormatter::PrepareFileName(std::string path)
{
  file_name = "";
  bool flag = false;
  for (int i = path.size(); i >= 0; i--)
  {
    if (path[i] == '/' || path[i] == '\\')
    {
      break;
    }
    if (flag)
    {
      file_name = path[i] + file_name;
    }
    if (path[i] == '.')
    {
      flag = true;
    }
  }
}

void NeuralNetworkFormatter::GenerateDataFiles()
{

}

void NeuralNetworkFormatter::GeneratePredictionFile()
{
}


