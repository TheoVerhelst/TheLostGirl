#ifndef TREE_HPP
#define TREE_HPP

#include <string>
#include <vector>
#include <memory>

template <typename T>
class Tree
{
	public:
		typedef std::shared_ptr<Tree<T>> Ptr;
		static Ptr create(const T& value,
				const std::vector<Ptr>& children=std::vector<Ptr>());
		Tree(const T& value,
				const std::vector<Ptr>& children=std::vector<Ptr>());
		const T& getValue() const;
		T& getValue();
		void setValue(const T& newValue);

		size_t childrenNumber() const;
		bool noChildren() const;
		void pushChild(Ptr child);
		void setChild(size_t n, Ptr newChild);
		void removeChild(size_t n);
		const Ptr getChild(size_t n) const;
		Ptr getChild(size_t n);
		void resolveChildren(Ptr self);

		void setParent(Ptr newParent);
		const Ptr getParent() const;
	private:
		std::vector<Ptr> m_children;
		Ptr m_parent;
		T m_data;
};

#include "Tree.impl"

#endif