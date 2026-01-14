#include "hepch.h"
#include "Entity.h"
#include "World/Scene/Scene.h"

entt::registry& Entity::GetSceneRegistry()
{
    return _Scene->GetRegistry();
}
