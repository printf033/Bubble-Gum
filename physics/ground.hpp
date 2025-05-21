#include "model.hpp"
#include "collider.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

#ifndef GROUND_HPP
#define GROUND_HPP

// 默认第一个Mesh为地面！！！
#define GRAVITY_ACCELERATION 9.8f

class Ground : public Model
{
    std::unordered_map<std::string, Collider> colliders_;
    float half_g_ = GRAVITY_ACCELERATION * 0.5f;
    float sumTime_ = -1.0f;

public:
    Ground(const std::filesystem::path &path)
        : Model(path) {}
    Ground(Model &&model)
        : Model(std::move(model)) {}
    ~Ground() = default;
    Ground(const Ground &) = delete;
    Ground &operator=(const Ground &) = delete;
    Ground(Ground &&) = delete;
    Ground &operator=(Ground &&) = delete;
    void addCollider(const std::string &name, const std::filesystem::path &path)
    {
        colliders_.emplace(name, Collider(path));
    }
    void addCollider(const std::string &name, const std::filesystem::path &path, const std::string &animName)
    {
        colliders_.emplace(name, Collider(path, animName));
    }
    void addCollider(const std::string &name, Collider &&collider)
    {
        colliders_.emplace(name, std::move(collider));
    }
    Collider &getCollider(const std::string &name)
    {
        return colliders_.at(name);
    }
    void detectNcorrect(float deltaTime)
    {
        Mesh &groundMesh = getMeshes()[0]; // 默认第一个Mesh为地面！！！
        // 处理某个Collider与地面Mesh的碰撞
        for (auto &collider : colliders_)
        {
            if (collider.second.isMoved())
            {
                //////// octree later... ////////
                //  重力模拟
                glm::vec3 down(0.0f, -1.0f, 0.0f);
                for (int i = 0; i < groundMesh.getIndices().size(); i += 3)
                {
                    auto &v0 = groundMesh.getVertices()[groundMesh.getIndices()[i]];
                    auto &v1 = groundMesh.getVertices()[groundMesh.getIndices()[i + 1]];
                    auto &v2 = groundMesh.getVertices()[groundMesh.getIndices()[i + 2]];
                    glm::vec2 intersection;
                    float distance = .0f;
                    if (glm::intersectRayTriangle(collider.second.getPosition(),
                                                  down,
                                                  v0.position,
                                                  v1.position,
                                                  v2.position,
                                                  intersection,
                                                  distance) &&
                        (distance > .01f || distance < -.01f))
                    {
                        // ///////////////////////////////////////////////////////////
                        // std::clog << collider.second.getPosition().y << std::endl;
                        // ///////////////////////////////////////////////////////////
                        if (-1.0f == sumTime_)
                            sumTime_ = 0.0f;
                        else
                        { // d = 1/2 * g_ * ( t^2 - t0^2 )
                            float t0_square = sumTime_ * sumTime_;
                            sumTime_ += deltaTime;
                            float t_square = sumTime_ * sumTime_;
                            float d = half_g_ * (t_square - t0_square);
                            collider.second.getPosition().y -= d;
                            if (collider.second.getPosition().y < 0.01f)
                            {
                                sumTime_ = -1.0;
                                collider.second.getPosition().y -= distance;
                                collider.second.clearMoved();
                            }
                        }
                        break;
                    }
                }
                // 处理与地面中其他Mesh的碰撞
                /////////////////////////////////////////////////////////
            }
            // 处理Collider之间的碰撞
            /////////////////////////////////////////////////////////
        }
    }
};

#endif