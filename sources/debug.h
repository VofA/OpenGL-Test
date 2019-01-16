#ifndef DEBUG_H
#define DEBUG_H

#include <glad/glad.h>

class Debug {
public:
	bool polygonMode = false;

	void process() {
		if (polygonMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

	}
};
#endif