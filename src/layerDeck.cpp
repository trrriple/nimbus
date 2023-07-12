#include "layerDeck.hpp"

namespace nimbus
{

LayerDeck::~LayerDeck()
{
    for (auto& layer : m_deck)
    {
        layer->onRemove();
        delete layer;
    }
}

void LayerDeck::insertLayer(Layer* layer, int32_t location)
{
    if (location == -1)
    {
        m_deck.push_back(layer);
    }
    else
    {
        m_deck.insert(m_deck.begin() + location, layer);
    }

    layer->onInsert();
}

void LayerDeck::removeLayer(Layer* layer)
{
    auto it = std::find(m_deck.begin(), m_deck.end(), layer);
    if (it != m_deck.end())
    {
        m_deck.erase(it);
        layer->onRemove();
    }
}

}  // namespace nimbus