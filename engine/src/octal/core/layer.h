#pragma once
#include "octal/defines.h"
#include <string>
#include <vector>

namespace octal {

	/// A layer to our application. This is how we add functionality to an app.
	class Layer {
		public:
			/// Constructor
			/// @param name optional name for this layer
			Layer(const std::string& name = "Layer");
			/// Default destructor that can be overriden
			virtual ~Layer() = default;

			/// What to do when this layer is first added
			virtual void OnPush() {} 
			/// What to do when the layer is removed
			virtual void OnPop() {} 
			/// Code to run on each update (could be more than once a frame)
			/// @param dt the time that has passed since the last time this was called
			virtual void OnUpdate(double dt) {}
			/// Rendering code for this layer
			/// @param dt the time that has passed since the last time this was called
			virtual void OnRender(double dt) {}
			/// Code to handle events this layer is subscribed to
			virtual void OnEvent(double dt) {}

		protected:
			/// Name of this layer for debuggin purposes
			std::string m_DebugName;

	};

	/// Maintains a collection of layer pointers
	/// The order of the pointer in the stack determines rendering order
	class LayerStack {

		public:
			/// Default constructor
			LayerStack() = default;
			/// Destructor
			/// Deletes all the layers for us
			~LayerStack();

			/// Add a layer to the stack
			void PushLayer(Layer* layer);
			/// Add an overlay to the stack
			void PushOverlay(Layer* overlay);
			/// Remove a layer from the stack
			void PopLayer(Layer* layer);
			/// Remove an overlay from the stack
			void PopOverlay(Layer* overlay);

			/// Iterator to the beginning of the layer stack
			std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
			/// Iterator to the end of the layer stack
			std::vector<Layer*>::iterator end() { return m_Layers.end(); }
			/// Iterator to the end of the layer stack
			std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
			/// Iterator to the beginning of the layer stack
			std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

			/// Iterator to the beginning of the layer stack
			std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
			/// Iterator to the end of the layer stack
			std::vector<Layer*>::const_iterator end()	const { return m_Layers.end(); }
			/// Iterator to the end of the layer stack
			std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
			/// Iterator to the beginning of the layer stack
			std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }

		private:
			/// Vector of layer pointers we are storing
			std::vector<Layer*> m_Layers;
			/// Index of the last layer so we know where to insert
			u32 m_LayerIdx{0};
	};
}
