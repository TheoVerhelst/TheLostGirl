#ifndef COMMAND_H
#define COMMAND_H

#include <functional>
#include <entityx/entityx.h>
#include <TheLostGirl/Category.h>
#include <TheLostGirl/FlagSet.h>

/// Functor applied to an entity.
/// The Command class holds a function that will be applied to a specific entity,
/// or to a category of entities, but not both at the same time.
class Command
{
	public:
		/// Constructor.
		/// \param category The category.
		/// \param action The action.
		Command(const FlagSet<Category>& category, std::function<void(entityx::Entity)> action);

		/// Constructor.
		/// \param entity The entity.
		/// \param action The action.
		Command(entityx::Entity entity, std::function<void(entityx::Entity)> action);

		/// Copy constructor.
		/// \param other Command to copy.
		Command(const Command& other);

		/// Destructor.
		~Command();

		/// Copy assignment operator
		/// \param other Command to copy.
		Command& operator=(const Command& other);

		/// Get the action.
		/// \return The action.
		const std::function<void(entityx::Entity)>& getAction() const;

		/// Set the action.
		/// \param action The new action.
		void setAction(const std::function<void(entityx::Entity)>& action);

		/// Indicates whether the target is specific (an entity) or not (a category).
		/// \return true if the target is specific, false otherwise.
		bool isTargetSpecific() const;

		/// Set the category.
		/// \param category The new category.
		void setTarget(const FlagSet<Category>& category);

		/// Set the entity.
		/// \param entity The new entity.
		void setTarget(entityx::Entity entity);

		/// Get the category, if the target was a category.
		/// \return The category.
		/// \exception std::logic_error if an entity was given as target.
		const FlagSet<Category>& getCategory() const throw(std::logic_error);

		/// Get the entity, if the target was a entity.
		/// \exception std::logic_error if a category was given as target.
		entityx::Entity getEntity() const throw(std::logic_error);

	private:
		/// Destroys the category and replaces it by the given \a entity.
		/// \param entity The entity to construct in the enum.
		void allocate(entityx::Entity entity);

		/// Destroys the entity and replaces it by the given \a category.
		/// \param entity The category to construct in the enum.
		void allocate(const FlagSet<Category>& category);

		/// Stored function.
		/// \param entityx::Entity Entity which do the action.
		/// That function can be a lambda expression, a functor, a pointer to a function, a member function, ...
		/// It just need to take this argument as parameter.
		std::function<void(entityx::Entity)> m_action;

		/// Indicates if the command must be applied on a specific entity
		/// or on a global group en entities. If true, the action must be
		/// applied on the entity pointer, and if false on every entity with the category.
		bool m_targetIsSpecific;

		union
		{
			/// Category of entity concerned by \a action.
			FlagSet<Category> m_category;

			/// Specific entity concerned by \a action.
			entityx::Entity m_entity;
		};
};

#endif//COMMAND_H
