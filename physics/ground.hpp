#include "model.hpp"
#include "collider.hpp"

#ifndef GROUND_HPP
#define GROUND_HPP

class Ground : public Model
{
    std::unordered_map<std::string, Collider> colliders_;

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
    void detectNcorrect()
    {
        for (auto &collider : colliders_)
        {
            if (collider.second.isMoved())
            {
                ///////////////////////////////////////////////////
                std::clog << "x" << collider.second.getPosition().x
                          << "y" << collider.second.getPosition().y
                          << "z" << collider.second.getPosition().z
                          << std::endl;
                ///////////////////////////////////////////////////

                collider.second.clearMoved();
            }
        }
    }
};

#endif