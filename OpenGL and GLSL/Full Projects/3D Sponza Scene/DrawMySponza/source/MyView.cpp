#include "MyView.hpp"
#include <sponza/sponza.hpp>
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>


MyView::MyView()
{
}

MyView::~MyView() 
{
}

void MyView::setScene(const sponza::Context * scene)
{
    scene_ = scene;
}

/*
*   Shaders are loaded from a text file into a string which is
*	passed to OpenGL for compiling.  Compile errors can be
*	queried via the info log.
*/
GLuint MyView::CreateShader(GLenum shaderType, std::string path)
{
	GLint compile_status = GL_FALSE;

	GLuint shader = glCreateShader(shaderType);
	std::string shader_string = tygra::createStringFromFile(path);

	const char * shader_code = shader_string.c_str();
	glShaderSource(shader, 1, (const GLchar **)&shader_code, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

	if (compile_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	return shader;
}

//Casts from sponza::Vector3 to glm::vec3 and returns a glm::vec3
glm::vec3 MyView::CastVector3(sponza::Vector3 s_Vec3)
{
	glm::vec3 glm_Vec3 = glm::vec3(s_Vec3.x, s_Vec3.y, s_Vec3.z);

	return glm_Vec3;
}


void MyView::windowViewWillStart(tygra::Window * window)
{
    assert(scene_ != nullptr);

	//CREATES A VERTEX SHADER
	GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, "resource:///sponza_vs.glsl");

	//CREATES A FRAGMENT SHADER
	GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, "resource:///sponza_fs.glsl");

	/*
	*	A program object is made to host the shaders and their
	*   settings. This program object attempts to bind the shaders
	*   together when it is linked. If the shaders aren't compatible
	*   the link process will fail.
	*/
	program_ = glCreateProgram();
	glAttachShader(program_, vertex_shader); 
	glBindAttribLocation(program_, kVertexPosition, "vertex_position"); 
	glBindAttribLocation(program_, kVertexNormal, "vertex_normal");
	glBindAttribLocation(program_, kTextureCoord, "texCoord");
	glDeleteShader(vertex_shader);
	glAttachShader(program_, fragment_shader);
	glBindFragDataLocation(program_, kFragmentColour, "fragment_colour");
	glDeleteShader(fragment_shader);
	glLinkProgram(program_);


	/*
	*	Test if the shader program linked successfully. If not then
	*   get the error log and display in the console window.
	*/
	GLint link_status = GL_FALSE;
	glGetProgramiv(program_, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) 
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(program_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	//Holds all the data for the meshes in the scene. 
	sponza::GeometryBuilder builder;

	 /*
	 *	 Get geometry data (vertex and element data) of the meshes and 
	 *	 fills the map of meshes. 
	 */
	for (auto mesh : builder.getAllMeshes()) //Happens 27 times
	{
		//Creates a new temporary MeshGL.
		MeshGL newMesh;

		//Gets the geometry data from the builder 
		const auto& newNormals = mesh.getNormalArray(); //Vec3
		const auto& newPositions = mesh.getPositionArray();//Vec3
		const auto& newElements = mesh.getElementArray();
		const auto& newTexCoordinates = mesh.getTextureCoordinateArray();
		
		/*
		*	 Creates OpenGL buffers to hold the geometry data. This is
		*    how we pass our geometry data to OpenGL so it can draw it.
		*/

		//NORMAL VBO
		glGenBuffers(1, &newMesh.normal_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.normal_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			newNormals.size() * sizeof(glm::vec3), 
			newNormals.data(), 
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);

		//Position VBO
		glGenBuffers(1, &newMesh.position_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.position_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			newPositions.size() * sizeof(glm::vec3), 
			newPositions.data(), 
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, kNullId); 

		//TextureCoordinates VBO
		glGenBuffers(1, &newMesh.texCoord_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.texCoord_vbo);
		glBufferData(GL_ARRAY_BUFFER,
			newTexCoordinates.size() * sizeof(glm::vec2),
			newTexCoordinates.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);

		//Element VBO
		glGenBuffers(1, &newMesh.element_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.element_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			newElements.size() * sizeof(unsigned int),
			newElements.data(),
			GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kNullId);
		newMesh.element_count = newElements.size();


		// Vertex array object
		glGenVertexArrays(1, &newMesh.vao);
		glBindVertexArray(newMesh.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.element_vbo);

		//Handle Position data
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.position_vbo); //1. Bind Pos to VAO.
		glEnableVertexAttribArray(kVertexPosition); //2. Enable Pos
		glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec3), TGL_BUFFER_OFFSET(0)); //3. Set up Attributes for Pos

		//Handle Normal data
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.normal_vbo);
		glEnableVertexAttribArray(kVertexNormal);
		glVertexAttribPointer(kVertexNormal, 3, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));

		//Handle TexCoord data
		glBindBuffer(GL_ARRAY_BUFFER, newMesh.texCoord_vbo);
		glEnableVertexAttribArray(kTextureCoord);
		glVertexAttribPointer(kTextureCoord, 2, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec2), TGL_BUFFER_OFFSET(0));

		//Null the VAO after evrything is added
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);
		glBindVertexArray(kNullId);

		
		//Creates a texture object from pixel data read from an PNG.
		tygra::Image texture_image
			= tygra::createImageFromPngFile("resource:///hex.png");

		if (texture_image.doesContainData()) 
		{

			glGenTextures(1, &test_texture_);
			glBindTexture(GL_TEXTURE_2D, test_texture_);

			//Filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_LINEAR);

			//Wrapping
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };

			//Loading Img
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGBA,
				texture_image.width(),
				texture_image.height(),
				0,
				pixel_formats[texture_image.componentsPerPixel()],
				texture_image.bytesPerComponent() == 1
				? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT,
				texture_image.pixelData());

			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, kNullId);
		}

		//Add the temporary mesh to the map.
		meshMap.insert(std::make_pair(mesh.getId(), newMesh));
	}
}

void MyView::windowViewDidReset(tygra::Window * window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void MyView::windowViewDidStop(tygra::Window * window)
{
	
	//This is where we must free any OpenGL resources we've created.
	std::map<unsigned int, MeshGL>::iterator it = meshMap.begin();
	for (it = meshMap.begin(); it != meshMap.end(); ++it)
	{
		glDeleteProgram(program_);
		glDeleteBuffers(1, &(it->second.position_vbo));
		glDeleteBuffers(1, &(it->second.normal_vbo));
		glDeleteBuffers(1, &(it->second.element_vbo));
		glDeleteVertexArrays(1, &(it->second.vao));
	}
}

void MyView::windowViewRender(tygra::Window * window)
{
    assert(scene_ != nullptr);

	//Configure OpenGL pipeline settings before drawing.
	glEnable(GL_DEPTH_TEST);

    glClearColor(0.f, 0.f, 0.25f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Compute aspect ratio 
	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);
	const float aspect_ratio = viewport_size[2] / (float)viewport_size[3];

	//Compute values needed by shader uniform variables.
	glm::mat4 projection_xform = glm::perspective(glm::radians(scene_->getCamera().getVerticalFieldOfViewInDegrees()),
												aspect_ratio,
												1.f, 1000.f);

	//Vecs needed to compute the view xform
	glm::vec3 camPos = CastVector3(scene_->getCamera().getPosition());
	glm::vec3 camDir = CastVector3(scene_->getCamera().getDirection());

	glm::vec3 center = camPos + camDir;//look pos
	glm::vec3 upDir = CastVector3(scene_->getUpDirection());

	glm::mat4 view_xform = glm::lookAt(camPos, center, upDir);


	//Multiply the matrices together
	glm::mat4 projection_view_xform = projection_xform * view_xform;

	glUseProgram(program_);

	//Create all the uniform vars.
	GLuint pv_xform_id = glGetUniformLocation(program_, "projection_view_xform");
	glUniformMatrix4fv(
		pv_xform_id,
		1,
		GL_FALSE,
		glm::value_ptr(projection_view_xform)); 

	//Sends the texture as a uniform
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, test_texture_);
	glUniform1i(glGetUniformLocation(program_, "tex"), 0); 


	std::map<unsigned int, MeshGL>::iterator it = meshMap.begin();

	for (it = meshMap.begin(); it != meshMap.end(); ++it)
	{
		//Get the list of instances that need drawing
		std::vector<unsigned int> instances = scene_->getInstancesByMeshId(it->first);
		
		//Bind (attach to the pipeline) the geometry 27 times as there are 27 meshes
		glBindVertexArray(it->second.vao);
		
		//for each instance of this mesh
		for (auto instanceID : instances)
		{
			//Get the instance
			sponza::Instance instance = scene_->getInstanceById(instanceID);

			//Get the model matrix
			sponza::Matrix4x3 m_xform = instance.getTransformationMatrix();

			//Converts from sponza Mat4X3 to glm mat4x3
			glm::mat4x3 xform = glm::mat4x3(m_xform.m00, m_xform.m01, m_xform.m02,
											m_xform.m10, m_xform.m11, m_xform.m12, 
											m_xform.m20, m_xform.m21, m_xform.m22,
											m_xform.m30, m_xform.m31, m_xform.m32);

			//Cast to mat4
			glm::mat4 model_xform = glm::mat4(xform);

			//Pass the model xform to the shaders
			GLuint model_xform_id = glGetUniformLocation(program_, "model_xform");
			glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform)); 

			glDrawElements(GL_TRIANGLES, it->second.element_count, GL_UNSIGNED_INT, 0);//Draw 81 times
		}
	}
}