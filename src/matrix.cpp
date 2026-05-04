#include "matrix.h"

glm::vec3 extractCameraPosition(const glm::mat4& viewMatrix)
{
	//
	// The camera position can be extracted from the view matrix by
	// negating the translation components of the matrix
	//
	return -glm::vec3(viewMatrix[3][0], viewMatrix[3][1], viewMatrix[3][2]);
}
