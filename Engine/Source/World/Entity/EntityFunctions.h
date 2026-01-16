#pragma once

namespace House {
	template<typename T>
	bool Entity::HasComponent() { return _Scene->GetRegistry().any_of<T>(_Handle); }

	template<typename T, typename ...Args>
	T& Entity::AddComponent(Args && ...args)
	{
		CHECKF(HasComponent<T>(), "Failed to add component it is already in entity");
		T& component = _Scene->GetRegistry().emplace<T>(_Handle, std::forward<Args>(args)...);
		return component;
	}

	template<typename T, typename ...Args>
	T& Entity::AddOrReplaceComponent(Args && ...args)
	{
		T& component = _Scene->GetRegistry().emplace_or_replace<T>(_Handle, std::forward<Args>(args)...);
		return component;
	}

	template<typename T>
	T& Entity::GetComponent()
	{
		CHECKF(!HasComponent<T>(), "Entity doesnt have this component");
		return _Scene->GetRegistry().get<T>(_Handle);
	}

	template<typename T>
	const T& Entity::GetComponent() const
	{
		CHECKF(!HasComponent<T>(), "Entity doesnt have this component");
		return _Scene->GetRegistry().get<T>(_Handle);
	}

	template<typename T>
	void Entity::RemoveComponent()
	{
		CHECKF(!HasComponent<T>(), "Tried to remove a non exsisting component");
		_Scene->GetRegistry().remove<T>(_Handle);
	}
}