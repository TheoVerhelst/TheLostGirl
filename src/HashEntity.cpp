#include <functional>
#include <entityx/Entity.h>

#include <TheLostGirl/HashEntity.h>

std::size_t HashEntity::operator()(entityx::Entity entity) const
{
	return std::hash<uint32_t>()(entity.id().index());
}
