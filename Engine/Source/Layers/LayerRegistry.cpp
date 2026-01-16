#include "hepch.h"
#include "LayerRegistry.h"

namespace House {
	void LayerRegistry::PushLayer(Layer* layer) {
		_Layers.emplace(_Layers.begin() + _LayerInsertIndex, layer);
		_LayerInsertIndex++;
	}

	void LayerRegistry::PushOverlay(Layer* layer)
	{
		_Layers.emplace_back(layer);
	}

	void LayerRegistry::PopLayer(Layer* layer) {}

	void LayerRegistry::Clear() {
		for (Layer* layer : _Layers) {
			layer->OnDetach();
			delete layer;
			layer = nullptr;
		}
		_Layers.clear();
		_LayerInsertIndex = 0;
		LOG_CORE_INFO("LayerStack cleared");
	}
}