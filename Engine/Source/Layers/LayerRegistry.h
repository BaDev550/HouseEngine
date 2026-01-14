#pragma once
#include "Layer.h"
#include <vector>

class LayerRegistry {
public:
	LayerRegistry() {}
	~LayerRegistry() {}

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);
	void PopLayer(Layer* layer);
	void Clear();

	std::vector<Layer*>::iterator begin() { return _Layers.begin(); }
	std::vector<Layer*>::iterator end()   { return _Layers.end(); }
	std::vector<Layer*>::reverse_iterator rbegin() { return _Layers.rbegin(); }
	std::vector<Layer*>::reverse_iterator rend()   { return _Layers.rend(); }

	std::vector<Layer*>::const_iterator begin() const { return _Layers.begin(); }
	std::vector<Layer*>::const_iterator end()   const { return _Layers.end(); }
	std::vector<Layer*>::const_reverse_iterator rbegin() const { return _Layers.rbegin(); }
	std::vector<Layer*>::const_reverse_iterator rend()   const { return _Layers.rend(); }
private:
	std::vector<Layer*> _Layers;
	uint32_t _LayerInsertIndex = 0;
};