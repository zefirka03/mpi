#pragma once
#include <type_traits>

#include "AirUtils.h"
#include <entt.hpp>

AIR_NAMESPACE_BEGIN

class Entity;

struct ECS {
	static entt::registry registry;
};


class Entity {
public:
	Entity() : m_entity(ECS::registry.create()) {}
	operator entt::entity() { return m_entity; }

	template<class TComponent, class... Args>
	inline TComponent& add_component(Args... args);

	template<class TComponent>
	inline TComponent& get_component() {
		return ECS::registry.get<TComponent>(m_entity);
	}

	template<class...  TComponents>
	inline bool has_components() {
		return ECS::registry.all_of<TComponents...>(m_entity);
	}

protected:
	entt::entity m_entity;
};


class Component {
public:
	Entity get_entity() {
		return m_entity_handler;
	}

private:
	friend class Entity;
	Entity m_entity_handler;
};


template<class TComponent, class... Args>
inline TComponent& Entity::add_component(Args... args) {
	static_assert(std::is_base_of<Component, TComponent>::value, "Class TComponent must inherit from air::Component");

	auto& comp = ECS::registry.emplace<TComponent>(m_entity, std::forward<Args>(args)...);
	comp.m_entity_handler = *this;

	return comp;
}


template<class... TComponents>
class Prefab : public Entity {
public:
	Prefab() : Entity() {
		(add_component<TComponents>(), ...);
	}
};


class System {
public:
	virtual void start() = 0;
	virtual void update() = 0;
	virtual void last_update() {};
	virtual ~System() {}
};

AIR_NAMESPACE_END