#include <functional>
#include <entityx/entityx.h>
#include <TheLostGirl/HashEntity.hpp>

std::size_t HashEntity::operator()(entityx::Entity entity) const
{
	return std::hash<uint32_t>()(entity.id().index());
}
