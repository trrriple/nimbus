#pragma once

#include "core.hpp"
#include "layer.hpp"

namespace nimbus
{

    static const int32_t k_insertLocationHead = -1;

class LayerDeck
{
   public:
    LayerDeck() = default;

    ~LayerDeck();

    void insertLayer(Layer* layer, int32_t location = k_insertLocationHead);

    void removeLayer(Layer* layer);

    ////////////////////////////////////////////////////////////////////
    // Foward Interator Helpers
    ////////////////////////////////////////////////////////////////////
    std::deque<Layer*>::iterator begin()
    {
        return m_deck.begin();
    }

    std::deque<Layer*>::iterator end()
    {
        return m_deck.end();
    }

    ////////////////////////////////////////////////////////////////////
    // Reverse Interator Helpers
    ////////////////////////////////////////////////////////////////////
    std::deque<Layer*>::reverse_iterator rbegin()
    {
        return m_deck.rbegin();
    }

    std::deque<Layer*>::reverse_iterator rend()
    {
        return m_deck.rend();
    }

    ////////////////////////////////////////////////////////////////////
    // Constant Interator Helpers
    ////////////////////////////////////////////////////////////////////
    std::deque<Layer*>::const_iterator begin() const
    {
        return m_deck.begin();
    }

    std::deque<Layer*>::const_iterator end() const
    {
        return m_deck.end();
    }

    ////////////////////////////////////////////////////////////////////
    // Constant Reverse Interator Helpers
    ////////////////////////////////////////////////////////////////////
    std::deque<Layer*>::const_reverse_iterator rbegin() const
    {
        return m_deck.rbegin();
    }

    std::deque<Layer*>::const_reverse_iterator rend() const
    {
        return m_deck.rend();
    }

   private:
    std::deque<Layer*> m_deck;
};
}  // namespace nimbus