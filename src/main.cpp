#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "Scene.h"
#include "Parser.h"

using namespace std;

shared_ptr<Scene> scene;
shared_ptr<Parser> parser;

static int initScene(const std::string SCENE, const std::string outputFile) {
	parser->setJSONfilename(SCENE);
	scene = make_shared<Scene>(outputFile);

	// Catch in main for stack unwinding
	try {
		parser->createScene(scene);
	} catch (const std::exception& exc) {
		std::cerr << exc.what() << std::endl;
		return 1;
	}
	return 0;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		cout << "Please specify the scene to render and an output png file." << endl;
		return 0;
	}
	
	parser = make_shared<Parser>();
	int exitStatus = initScene( argv[1], argv[2] );
	if (exitStatus != 0) return exitStatus;

	scene->render();

	return 0;
}