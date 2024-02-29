#include "Scene.h"

AIR_NAMESPACE_BEGIN

SceneBase::~SceneBase() {
    for (auto system : m_systems)
        delete system.second;
}


void SceneBase::_start_systems() {
	for (auto system : m_systems)
		system.second->start();
}


void SceneBase::_update_systems() {
	for (auto system : m_systems)
		system.second->update();
}


void SceneBase::_last_update_systems() {
	if (m_systems.empty()) return;
	for (auto it = --m_systems.end(); it != m_systems.begin(); --it)
		it->second->last_update();
}

AIR_NAMESPACE_END