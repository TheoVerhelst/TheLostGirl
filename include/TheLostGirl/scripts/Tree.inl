template <typename T>
typename Tree<T>::Ptr Tree<T>::create(const T& value, const std::vector<Tree<T>::Ptr>& children)
{
	return Ptr(new Tree<T>(value, children));
}

template <typename T>
typename Tree<T>::Ptr Tree<T>::copy(Ptr other)
{
	Ptr res = create(other->getValue());
	for(size_t i{0}; i < other->childrenNumber(); ++i)
		res->pushChild(copy(other->getChild(i)));
	return res;
}

template <typename T>
Tree<T>::Tree(const T& value, const std::vector<Tree<T>::Ptr>& children):
	m_children(children),
	m_data(value)
{}

template <typename T>
Tree<T>::Tree(Ptr other):
	m_data(other->getValue())
{
	for(size_t i{0}; i < other->childrenNumber(); ++i)
		pushChild(create(other->getChild(i)));
}

template <typename T>
const T& Tree<T>::getValue() const
{
	return m_data;
}

template <typename T>
T& Tree<T>::getValue()
{
	return m_data;
}

template <typename T>
void Tree<T>::setValue(const T& newValue)
{
	m_data = newValue;
}

template <typename T>
size_t Tree<T>::childrenNumber() const
{
	return m_children.size();
}

template <typename T>
bool Tree<T>::noChildren() const
{
	return m_children.empty();
}

template <typename T>
void Tree<T>::pushChild(Tree<T>::Ptr child)
{
	m_children.push_back(child);
}

template <typename T>
void Tree<T>::setChild(size_t n, Tree<T>::Ptr newChild)
{
	m_children[n] = newChild;
}

template <typename T>
void Tree<T>::removeChild(size_t n)
{
	m_children.erase(m_children.begin()+n);
}

template <typename T>
const typename Tree<T>::Ptr Tree<T>::getChild(size_t n) const
{
	return m_children[n];
}

template <typename T>
typename Tree<T>::Ptr Tree<T>::getChild(size_t n)
{
	return m_children[n];
}

template <typename T>
void Tree<T>::resolveChildren(Tree<T>::Ptr self)
{
	for(size_t i{0}; i < self->childrenNumber(); ++i)
		self->getChild(i)->setParent(self);
}

template <typename T>
void Tree<T>::setParent(Tree<T>::Ptr newParent)
{
	m_parent = newParent;
}

template <typename T>
const typename Tree<T>::Ptr Tree<T>::getParent() const
{
	return m_parent;
}

