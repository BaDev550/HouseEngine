#pragma once
#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <functional>
#include <entt/entt.hpp>

class UUID {
public:
	UUID();
	UUID(uint64_t uuid);
	UUID(const std::string& str);
	UUID(entt::entity handle);

	std::string ToString() const;

	operator uint64_t() const { return _UUID; }
private:
	uint64_t _UUID;
};

namespace std {
	template<>
	struct hash<UUID> {
		std::size_t operator()(const UUID& uuid) const noexcept {
			return std::hash<uint64_t>()(static_cast<uint64_t>(uuid));
		}
	};
}