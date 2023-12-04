#pragma once
#ifndef PARSER_H
#define PARSER_H

#include <json.hpp>
#include <fstream>
#include <memory>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using json = nlohmann::json;

class Scene;

/**
* Parse JSON files that specify a scene using nlohmann C++ JSON parser.
* Not required to be modified.
* By Loïc Nassif
*/
class Parser
{
public:
	Parser(const std::string&& _filename);
	Parser() {}
	virtual ~Parser();

	void createScene(std::shared_ptr<Scene> scene);
	void setJSONfilename(const std::string _filename);

private:
	std::string filename;
};

#endif
