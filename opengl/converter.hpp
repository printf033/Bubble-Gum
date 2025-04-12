#include<assimp/matrix4x4.h>
#include<assimp/quaternion.h>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>

#ifndef CONVERTER_HPP
#define CONVERTER_HPP

class Converter
{
public:
	static inline glm::mat4 convertMatrixToGLMFormat(const aiMatrix4x4& src)
	{
		glm::mat4 dst;
		dst[0][0] = src.a1; dst[1][0] = src.a2; dst[2][0] = src.a3; dst[3][0] = src.a4;
		dst[0][1] = src.b1; dst[1][1] = src.b2; dst[2][1] = src.b3; dst[3][1] = src.b4;
		dst[0][2] = src.c1; dst[1][2] = src.c2; dst[2][2] = src.c3; dst[3][2] = src.c4;
		dst[0][3] = src.d1; dst[1][3] = src.d2; dst[2][3] = src.d3; dst[3][3] = src.d4;
		return dst;
	}
	static inline glm::vec3 getGLMVec(const aiVector3D& vec) 
	{ 
		return glm::vec3(vec.x, vec.y, vec.z); 
	}
	static inline glm::quat getGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
};

#endif