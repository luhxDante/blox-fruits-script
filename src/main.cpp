#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Eigen/Core"
#include "SDL.h"
#include "refract.h"

const double eta = 1.457;	//refractive index that was used to generate the lens
int windowWidth = 256;		//dimensions of the display window
int windowHeight = 256;

void DrawIntersections(SDL_Renderer* renderer, std::vector<Eigen::Vector2d> intersections) {	//display the intersections onto the window
	int numPoints = int(intersections.size());
	float scaleX = windowWidth / 256.0f;		//initially, we draw to a 256x256 window, but we want to be able to account for changing the window size
	float scaleY = windowHeight / 256.0f;

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);	//clear the screen with black
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);	//then draw the intersections
	for (int i = 0; i < numPoints; i++) {	
		SDL_RenderDrawPointF(renderer, intersections[i].x() * scaleX, intersections[i].y() * scaleY);	//scaling up the image to match the window size
	}
	SDL_RenderPresent(renderer);

}

int main(int argc, char** argv) {
	
	std::vector<Eigen::Vector3d> vertices;			//points, these are the positions where we refract rays through the lens
	std::vector<Eigen::Vector3d> normals;			//normal vectors, these are used to calculate the refraction through the above points
	std::vector<Eigen::Vector3d> refracteds;		//refracted ray vectors, these are the normalized directions that light leaves from each of the points
	std::vector<Eigen::Vector2d> intersections;		//x,y positions on the receiver plane where light intersects, scaled up to match the 256x256 of the target image

	ParseOBJ(argv[1], &vertices, &normals);			//first command line argument is the path to the obj file
	double receieverPlane = std::stod(argv[2]);		//second argument is the initial distance between the lens and the receiver plane/the wall, in the z direction, wall is parallel to x-y plane

	Refract(normals, &refracteds, eta);				//find the refracted ray directions at each point

	//make a window to display an image of the computed caustics
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("Caustics Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	CalculateIntersections(vertices, refracteds, &intersections, receieverPlane);
	DrawIntersections(renderer, intersections);

	bool quit = false;
	SDL_Event e;
	while (!quit) //main loop
	{
		while (SDL_PollEvent(&e) != 0)	//loop through events
		{
			if (e.type == SDL_QUIT) { quit = true; }
			else if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
				windowWidth = e.window.data1;
				windowHeight = e.window.data2;
				DrawIntersections(renderer, intersections);
			}
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_w:	//for fine-tuning the position of the lens
					receieverPlane += 0.1;
					CalculateIntersections(vertices, refracteds, &intersections, receieverPlane);
					DrawIntersections(renderer, intersections);
					break;
				case SDLK_s:	//for fine-tuning the position of the lens
					receieverPlane -= 0.1;
					CalculateIntersections(vertices, refracteds, &intersections, receieverPlane);
					DrawIntersections(renderer, intersections);
					break;
				case SDLK_q:	//for fine-tuning the position of the lens
					std::cout << "Current distance between wall and lens: " << receieverPlane << "\n";
					break;
				case SDLK_ESCAPE:
					quit = true;
					break;
				default:
					break;
				}
			}
		}

		
	}

	return 0;
}
