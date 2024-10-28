#include "refract.h"

std::vector<std::string> split(std::string s, std::string delimiter) {	//splits a string into several smaller strings separated by a given delimiter, from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

void ParseOBJ(std::string objFilePath, std::vector<Eigen::Vector3d>* vertices, std::vector<Eigen::Vector3d>* normals) {	//takes in an .obj file and populates vertices and normals from the file
	
	std::string line;
	std::ifstream file(objFilePath);
	if (file.is_open()) {
		while (std::getline(file, line))
		{
			if (line.substr(0,2) == "v ") {
				std::vector<std::string> lineContents = split(line, " ");
				Eigen::Vector3d vertex(std::stod(lineContents[1]), std::stod(lineContents[2]), std::stod(lineContents[3]));
				vertices->push_back(vertex);
			}
			else if (line.substr(0, 2) == "vn") {
				std::vector<std::string> lineContents = split(line, " ");
				Eigen::Vector3d normal(std::stod(lineContents[1]), std::stod(lineContents[2]), std::stod(lineContents[3]));
				normals->push_back(normal);
			}
			else if (line.substr(0, 2) == "vt") { break; }	//we don't care about anything beyond the vertices and normals, no point reading stuff we're not going to use
		}
		file.close();
	}
	else { std::cout << "Invalid file\n"; }
}

void Refract(std::vector<Eigen::Vector3d> normals, std::vector<Eigen::Vector3d>* refracteds, double eta) {	//computes refracted light vectors from incident and normal vectors, reference https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf

	int numPoints = int(normals.size());		//vertices, normals, and refracteds will all have the same number of elements
	Eigen::Vector3d incident(0, 0, 1);			//assume light always arrives at the interface pointing in the positive z direction
	Eigen::Vector3d TIR(.9999, 0, 0.0141418);	//in the case of total internal reflection, shoot the light way off to the side in an arbitrary direction so that it doesn't show up on the part of the screen we see

	for (int i = 0; i < numPoints; i++) {
		double cosIncidenceAngle = normals[i].z();	//incident dot normal = 0*Nx + 0*Ny + 1*Nz = Nz
		double sinRefractedAngle2 = eta * eta * (1 - cosIncidenceAngle * cosIncidenceAngle);	//eta1/eta2 = eta1 = eta, since the second medium is just air with eta2 = 1
		
		if (sinRefractedAngle2 <= 1) {		//check for total interal reflection
			Eigen::Vector3d refracted = eta * incident - (eta * cosIncidenceAngle - sqrtl(1 - sinRefractedAngle2)) * normals[i];
			refracteds->push_back(refracted);
		}
		else
		{
			refracteds->push_back(TIR);		//out of sight, out of mind :)
		}									
	}
}

void CalculateIntersections(std::vector<Eigen::Vector3d> vertices, std::vector<Eigen::Vector3d> refracteds, std::vector<Eigen::Vector2d>* intersections, double receiver_plane) {	//returns the points on the receiver plane where the light rays from each vertex intersect

	intersections->clear();	//clear the intersections every time we call the function 
	int numPoints = int(vertices.size());

	for (int i = 0; i < numPoints; i++) {
		double t = (receiver_plane - vertices[i].z()) / refracteds[i].z();	//solve for t in vertex.z + ray.z*t = receiever_plane.z
		Eigen::Vector2d intersection(vertices[i].x() + refracteds[i].x() * t, vertices[i].y() + refracteds[i].y() * t);
		intersection = intersection * 128 + Eigen::Vector2d(128, 128); //vertices x,y range between (-1,1), transform to go from (0,256) to match the 256x256 target image
		intersections->push_back(intersection);
	}													
}
