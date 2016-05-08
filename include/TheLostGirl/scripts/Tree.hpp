#ifndef TREE_HPPPP
#define TREE_HPPPP

#include <string>
#include <vector>
#include <memory>

/// Stores data in a n-tree.
/// Each node of the tree is another tree, and each node store a value of type
/// T.
/// Each node can have any number of children, that are nodes too.
/// For a better use of the memory, this class uses smart pointers typedefed as
/// Tree<T>::Ptr. To create a tree, use Tree<T>::create instead of the new
/// keyword.
/// \tparam T The type to store.
template <typename T>
class Tree
{
	public:
		/// Convenient typedef for a better memory handling.
		typedef std::shared_ptr<Tree<T>> Ptr;

		/// Allocates a new instance of a tree.
		/// \param value Value to store in the tree.
		/// \param children Children nodes of the tree.
		/// \return A smart pointer to the created tree.
		static Ptr create(const T& value, const std::vector<Ptr>& children=std::vector<Ptr>());

		/// Allocates a new instance of a tree by copying the content of another one.
		/// \param other Tree to copy.
		/// \return A smart pointer to the created tree.
		static Ptr copy(Ptr other);

		/// Constructor.
		/// \param value Value to store in the tree.
		/// \param children Children nodes of the tree.
		Tree(const T& value, const std::vector<Ptr>& children=std::vector<Ptr>());

		/// Copy constructor.
		/// \param other Tree to copy.
		Tree(Ptr other);

		/// Gets the value in this node.
		/// \return T Value.
		const T& getValue() const;

		/// Overload, gets the value in this node.
		/// \return Value.
		T& getValue();

		/// Sets the value in this node.
		/// \param newValue Value to set.
		void setValue(const T& newValue);

		/// Gets the number of direct children that have this node.
		/// \return The number of children.
		size_t childrenNumber() const;

		/// Checks if the tree has children.
		/// \return True if the tree has some children, false otherwise.
		bool noChildren() const;

		/// Adds a child after other childs.
		/// \param child The tree to add.
		void pushChild(Ptr child);

		/// Assigns the given child to the given position.
		/// \param n Position where to set the child. Must be lower than
		/// childrenNumber().
		/// \param newChild Tree to set.
		void setChild(size_t n, Ptr newChild);

		/// Removes the child at the given position.
		/// \param n The position where to remove the child.
		void removeChild(size_t n);

		/// Gets a constant pointer to a child.
		/// \param n The position of the child.
		/// \return A Tree::Ptr to the corresponding child.
		const Ptr getChild(size_t n) const;

		/// Gets a pointer to a child.
		/// \param n The position of the child.
		/// \return A Tree::Ptr to the corresponding child.
		Ptr getChild(size_t n);

		/// Sets the pointer to parents in the childs of the given instance.
		/// This function must be called after that a child is added, or after
		/// constructing.
		/// \param self Instance to resolve.
		void resolveChildren(Ptr self);

		/// Sets the parent of the tree.
		/// \param newParent New parent of the tree.
		void setParent(Ptr newParent);

		/// Gets a pointer to to parent.
		/// \return A constant pointer to the parent.
		const Ptr getParent() const;

	private:
		std::vector<Ptr> m_children;///< Array of children.
		Ptr m_parent;				///< Pointer to the parent.
		T m_data;                   ///< Stored value of the tree.
};

#include <TheLostGirl/scripts/Tree.inl>

#endif//TREE_HPP
