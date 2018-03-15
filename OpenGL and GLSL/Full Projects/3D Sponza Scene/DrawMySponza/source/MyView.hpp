#pragma once

#include <sponza/sponza_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>

#include <vector>
#include <map>
#include <memory>

class MyView : public tygra::WindowViewDelegate
{
public:
    
    MyView();
    
    ~MyView();
    
    void setScene(const sponza::Context * scene);

private:

    void windowViewWillStart(tygra::Window * window) override;
    
    void windowViewDidReset(tygra::Window * window,
                            int width,
                            int height) override;

    void windowViewDidStop(tygra::Window * window) override;
    
    void windowViewRender(tygra::Window * window) override;


private:

    const sponza::Context * scene_;

	GLuint test_texture_{ 0 };
	GLuint program_{ 0 };

	/*
	*	Define integer constants to improve readability. Any integers
	*	found in the code should be counting numbers, not ids.
	*/
	const static GLuint kNullId = 0;

	// The enums are used to keep track of the magic numbers in OpenGL.
	enum VertexAttribIndexes 
	{
		kVertexPosition = 0,
		kVertexNormal = 1,
		kTextureCoord = 2
	};

	enum FragmentDataIndexes 
	{
		kFragmentColour = 0
	};

	/*
	*	Encapsulate the vertex array and buffer objects into a data
	*   structure to make it easy to have many of them in the future.
	*/
	struct MeshGL
	{
		
		// VertexBufferObject for the normal
		GLuint normal_vbo{ 0 };

		// VertexBufferObject for the vertex positions
		GLuint position_vbo{ 0 };
		
		// VertexBufferObject for the texCoords
		GLuint texCoord_vbo{ 0 };
		
		// VertexBufferObject for the elements (indices)
		GLuint element_vbo{ 0 };

		// VertexArrayObject for the shape's vertex array settings
		GLuint vao{ 0 };

		// Needed for when we draw using the vertex arrays
		int element_count{ 0 };
	};

	//Holds all the meshes in the scene
	std::map<sponza::MeshId, MeshGL> meshMap;

	//------Refactoring------//
	GLuint CreateShader(GLenum shaderType, std::string path);
	glm::vec3 CastVector3(sponza::Vector3 s_Vec3);
};
