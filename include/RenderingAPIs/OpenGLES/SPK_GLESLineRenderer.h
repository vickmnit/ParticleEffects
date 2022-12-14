//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
//////////////////////////////////////////////////////////////////////////////////


#ifndef H_SPK_GLESLINERENDERER
#define H_SPK_GLESLINERENDERER

#include "RenderingAPIs/OpenGLES/SPK_GLESRenderer.h"
#include "Extensions/Renderers/SPK_LineRendererInterface.h"

#include "Core/SPK_Shader.h"

namespace SPK
{
namespace GLES
{
	/**
	* @class GLESLineRenderer
	* @brief A Renderer drawing particles as OpenGL lines
	*
	* The length of the lines is function of the Particle velocity and is defined in the universe space
	* while the width is fixed and defines in the screen space (in pixels).<br>
	* <br>
	* Below are the parameters of Particle that are used in this Renderer (others have no effects) :
	* <ul>
	* <li>SPK::PARAM_RED</li>
	* <li>SPK::PARAM_GREEN</li>
	* <li>SPK::PARAM_BLUE</li>
	* <li>SPK::PARAM_ALPHA (only if blending is enabled)</li>
	* </ul>
	*/

	class SPK_GL_PREFIX GLESLineRenderer : public GLESRenderer, public LineRendererInterface
	{
		SPK_IMPLEMENT_REGISTERABLE(GLESLineRenderer)

	public :

		//////////////////
		// Constructors //
		//////////////////

		/**
		* @brief Constructor of GLLineRenderer
		* @param length : the length multiplier of this GLLineRenderer
		* @param width : the width of this GLLineRenderer in pixels
		*/
		GLESLineRenderer(float length = 1.0f,float width = 1.0f);

		/**
		* @brief Creates and registers a new GLLineRenderer
		* @param length : the length multiplier of this GLLineRenderer
		* @param width : the width of this GLLineRenderer in pixels
		* @return A new registered GLLineRenderer
		* @since 1.04.00
		*/
		static GLESLineRenderer* create(float length = 1.0f,float width = 1.0f);

		///////////////
		// Interface //
		///////////////
		virtual void createBuffers(const Group& group);
		virtual void destroyBuffers(const Group& group);

		virtual void render(const Group& group);

protected :

		virtual bool checkBuffers(const Group& group);

private :

		// vertex buffers and iterators
		static float* gpuBuffer;
		static float* gpuIterator;

		// buffers names
		static const std::string GPU_BUFFER_NAME;

		Shader RainShader;
		glm::mat4 modelview;
		glm::mat4 projection;
	};


	inline GLESLineRenderer* GLESLineRenderer::create(float length,float width)
	{
		GLESLineRenderer* obj = new GLESLineRenderer(length,width);
		registerObject(obj);
		return obj;
	}
}
}

#endif
