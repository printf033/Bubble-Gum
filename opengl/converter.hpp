#include <string>
#include <sstream>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#ifndef CONVERTER_HPP
#define CONVERTER_HPP

class Converter
{
public:
	static inline glm::mat4 convertMatrix2GLMFormat(const aiMatrix4x4 &src)
	{
		glm::mat4 dst;
		dst[0][0] = src.a1;
		dst[1][0] = src.a2;
		dst[2][0] = src.a3;
		dst[3][0] = src.a4;
		dst[0][1] = src.b1;
		dst[1][1] = src.b2;
		dst[2][1] = src.b3;
		dst[3][1] = src.b4;
		dst[0][2] = src.c1;
		dst[1][2] = src.c2;
		dst[2][2] = src.c3;
		dst[3][2] = src.c4;
		dst[0][3] = src.d1;
		dst[1][3] = src.d2;
		dst[2][3] = src.d3;
		dst[3][3] = src.d4;
		return dst;
	}
	static inline glm::vec3 getGLMVec(const aiVector3D &vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}
	static inline glm::quat getGLMQuat(const aiQuaternion &pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
	static inline std::string convertMatrix2String(const glm::mat4 &src)
	{
		std::stringstream dst;
		dst << std::to_string(src[0][0]) << '#'
			<< std::to_string(src[1][0]) << '#'
			<< std::to_string(src[2][0]) << '#'
			<< std::to_string(src[3][0]) << '#'
			<< std::to_string(src[0][1]) << '#'
			<< std::to_string(src[1][1]) << '#'
			<< std::to_string(src[2][1]) << '#'
			<< std::to_string(src[3][1]) << '#'
			<< std::to_string(src[0][2]) << '#'
			<< std::to_string(src[1][2]) << '#'
			<< std::to_string(src[2][2]) << '#'
			<< std::to_string(src[3][2]) << '#'
			<< std::to_string(src[0][3]) << '#'
			<< std::to_string(src[1][3]) << '#'
			<< std::to_string(src[2][3]) << '#'
			<< std::to_string(src[3][3]);
		return dst.str();
	}
	static inline glm::mat4 convertString2Matrix(const std::string &src)
	{
		glm::mat4 dst;
		std::stringstream ss(src);
		std::string item;
		int i = 0;
		while (std::getline(ss, item, '#'))
		{
			if (i < 16)
			{
				dst[i / 4][i % 4] = std::stof(item);
				++i;
			}
			else
				break;
		}
		return dst;
	}
};

#endif