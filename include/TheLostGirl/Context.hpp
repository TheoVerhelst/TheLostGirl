#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <string>
#include <TheLostGirl/TypeTraits.hpp>

//Forward declarations
namespace sf
{
   class RenderWindow;
   class RenderTexture;
   class Font;
   class Texture;
   class SoundBuffer;
}

namespace tgui
{
   class Gui;
}

namespace entityx
{
   class EventManager;
   class EntityManager;
   class SystemManager;
}

class Application;
struct Parameters;
template <typename Resource, typename Identifier>
class ResourceManager;
class Interpreter;
class b2World;
class Player;
class LangManager;
class StateStack;

/// Enumeration of all elements in the contexts. This enumeration will help in
/// the assertion when a class wants to access an element of the context.
enum class ContextElement
{
#define MACRO_CONTEXT_ELEMENT(Type, Name, attribute) Name,
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT
};

/// Class that has to be inherited in order to gain access to some members of
/// the global context. The context elements that need to be accessed are
/// specified with the template parameter list.
template <ContextElement... Elements>
class ContextAccessor
{
	protected:
#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) static inline Type& get##Name();
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT
};

/// Class that statically holds pointers to context elements. An unique instance
/// of this class has to be instanciated with a reference to the Application
/// main object.
class Context
{
	public:
		/// Constructor.
		/// Initializes the global variable according to the given Application
		/// instance.
		/// \param application The main Application instance.
		Context(Application& application);

	private:
		template <ContextElement... Elements>
		friend class ContextAccessor;
#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute) static Type* m_attribute;
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT

};

#define MACRO_CONTEXT_ELEMENT(Type, Name, m_attribute)                         \
template <ContextElement... Elements>                                          \
inline Type& ContextAccessor<Elements...>::get##Name()                         \
{                                                                              \
	static_assert(ContainsValue<ContextElement, ContextElement::Name, Elements...>::value,\
			"You have not access to this context element");                    \
	return *Context::m_attribute;                                              \
}
#include <TheLostGirl/ContextElements.inl>
#undef MACRO_CONTEXT_ELEMENT

#endif//CONTEXT_HPP
