#pragma once

template<typename T>
bool Entity::HasComponent() { return GetSceneRegistry().any_of<T>(_Handle); }

template<typename T, typename ...Args>
T& Entity::AddComponent(Args && ...args)
{
	CHECKF(HasComponent<T>(), "Failed to add component it is already in entity");
	T& component = GetSceneRegistry().emplace<T>(_Handle, std::forward<Args>(args)...);
	return component;
}

template<typename T, typename ...Args>
T& Entity::AddOrReplaceComponent(Args && ...args)
{
	T& component = GetSceneRegistry().emplace_or_replace<T>(_Handle, std::forward<Args>(args)...);
	return component;
}

template<typename T>
T& Entity::GetComponent()
{
	CHECKF(!HasComponent<T>(), "Entity doesnt have this component");
	return GetSceneRegistry().get<T>(_Handle);
}

//template<typename T>
//const T& Entity::GetComponent()
//{
//	CHECKF(!HasComponent<T>(), "Entity doesnt have this component");
//	return GetSceneRegistry().get<T>(_Handle);
//}

template<typename T>
void Entity::RemoveComponent()
{
	CHECKF(!HasComponent<T>(), "Tried to remove a non exsisting component");
	GetSceneRegistry().remove<T>(_Handle);
}
