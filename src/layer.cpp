#include "nmpch.hpp"
#include "core.hpp"

#include "layer.hpp"

namespace nimbus
{

Layer::Layer(Type type, const std::string& name) : m_type(type), m_name(name)
{
}

}  // namespace nimbus
