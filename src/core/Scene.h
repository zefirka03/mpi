#pragma once
#include "AirUtils.h"
#include "ECS.h"

#include <unordered_map>
#include <typeindex>

AIR_NAMESPACE_BEGIN

class SceneBase {
public:
    virtual void start() = 0;
    ~SceneBase();

    template<typename TSystem>
    TSystem* get_system() {
        return (TSystem*)m_systems[typeid(TSystem)];
    }

protected:
    std::unordered_map<std::type_index, System*> m_systems;

    template<typename TSystem>
    void _add_system() {
        m_systems[typeid(TSystem)] = new TSystem();
    }
private:
    friend class Game;

    void _start_systems();
    void _update_systems();
    void _last_update_systems();
};


template<typename... Systems>
class Scene : public SceneBase {
public:
    Scene() {
        (_add_system<Systems>(), ...);
    }
private:
    
};

AIR_NAMESPACE_END
