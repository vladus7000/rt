#pragma once
#include <d3d11.h>
#include <xnamath.h>

namespace rt
{
	class Renderable;
	namespace object
	{
		struct CoreComponents
		{
			CoreComponents();
			~CoreComponents();
			Renderable* renderable;
		};

		class Object
		{
		public:
			Object();
			virtual ~Object();

			virtual void update(float delta);

			const XMMATRIX& getWorldTransform() { return m_world; }
			void setWorldTransform(const XMMATRIX& transform) { m_world = transform; }

			const CoreComponents& getCoreComponents() const { return m_coreComponents; }

		protected:
			XMMATRIX m_world;
			CoreComponents m_coreComponents;
		};
	}
}