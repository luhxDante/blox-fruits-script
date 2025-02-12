#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Eigen/Core"

void ParseOBJ(std::string objFilePath, std::vector<Eigen::Vector3d>* vertices, std::vector<Eigen::Vector3d>* normals);

void Refract(std::vector<Eigen::Vector3d> normals, std::vector<Eigen::Vector3d>* refracteds, double n);

void CalculateIntersections(std::vector<Eigen::Vector3d> vertices, std::vector<Eigen::Vector3d> refracteds, std::vector<Eigen::Vector2d>* intersections, double d);
