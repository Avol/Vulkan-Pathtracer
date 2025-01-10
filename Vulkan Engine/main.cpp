#include <iostream>
#include <sstream>

#include <cstdio>
#include <ctime>
#include <chrono>

#include <glm\glm.hpp>

#include "src/Renderer.h"
#include "src/Window.h"

#include "src/Platform.h"
#include "src/Texture.h"
#include "src/PathTracer.h"

int main()
{
	// create our renderer
	Renderer renderer;

	// open a window & clear it
	renderer.OpenWindow(800, 600, "Avol Vulkan Engine 0.05");
	renderer.GetWindow()->GetPresentation()->Clear();

	// create our pathtracer
	PathTracer * path_tracer = new PathTracer(&renderer, 800, 600);

	std::cout << "-------------------------------------- Rendering -----------------------------------" << std::endl;

	while ( renderer.Run() ) 
	{
		auto begin = std::chrono::high_resolution_clock::now();


		path_tracer->Dispatch();


		std::stringstream ss;
		auto end = std::chrono::high_resolution_clock::now();
		ss << "Frame time: " <<std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000 << " miliseconds";
		std::cout << ss.str().c_str() << std::endl;
	}

	return 0;
}
