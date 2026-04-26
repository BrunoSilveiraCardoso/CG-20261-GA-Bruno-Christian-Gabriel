#ifndef LIGHT_HPP_
#define LIGHT_HPP_

#include <glm/glm.hpp>

struct Distance {
	float constant;
	float linear;
	float quadratic;
};

struct Light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	
	Distance distance;
};

const Distance LIGHT_DIST_7 = {
	.constant = 1.0,
	.linear = 0.7,
	.quadratic = 1.8
};
const Distance LIGHT_DIST_50 = {
	.constant = 1.0,
	.linear = 0.09,
	.quadratic = 0.032
};
const Distance LIGHT_DIST_200 = {
	.constant = 1.0,
	.linear = 0.022,
	.quadratic = 0.0019
};

const Distance LIGHT_DIST_3250 = {
    .constant = 1.0,
    .linear = 0.0014,
    .quadratic = 0.000007
};

const Distance LIGHT_DIST_600 = {
    .constant = 1.0,
    .linear = 0.007,
    .quadratic = 0.0002
};

const Distance LIGHT_DIST_325 = {
    .constant = 1.0,
    .linear = 0.014,
    .quadratic = 0.0007
};

const Distance LIGHT_DIST_160 = {
    .constant = 1.0,
    .linear = 0.027,
    .quadratic = 0.0028
};

const Distance LIGHT_DIST_100 = {
    .constant = 1.0,
    .linear = 0.045,
    .quadratic = 0.0075
};

const Distance LIGHT_DIST_65 = {
    .constant = 1.0,
    .linear = 0.07,
    .quadratic = 0.017
};

const Distance LIGHT_DIST_32 = {
    .constant = 1.0,
    .linear = 0.14,
    .quadratic = 0.07
};

const Distance LIGHT_DIST_20 = {
    .constant = 1.0,
    .linear = 0.22,
    .quadratic = 0.20
};

const Distance LIGHT_DIST_13 = {
    .constant = 1.0,
    .linear = 0.35,
    .quadratic = 0.44
};

const std::vector<Distance> LIGHT_DISTANCES = {
    LIGHT_DIST_3250,
    LIGHT_DIST_600,
    LIGHT_DIST_325,
    LIGHT_DIST_200,
    LIGHT_DIST_160,
    LIGHT_DIST_100,
    LIGHT_DIST_65,
    LIGHT_DIST_50,
    LIGHT_DIST_32,
    LIGHT_DIST_20,
    LIGHT_DIST_13,
    LIGHT_DIST_7
};

#endif
