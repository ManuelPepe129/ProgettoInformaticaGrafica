#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

class BufferObject
{
public:
	GLuint location; // memory location

	// type of buffer
	GLenum type;

	BufferObject() {};

	BufferObject(GLenum type)
		: type(type)
	{ };

	// generate object
	void generate() 
	{
		glGenBuffers(1, &location);
	}

	// bind object
	void bind()
	{
		glBindBuffer(type, location);
	}

	// set data
	template<typename T>
	void setData(GLuint noElements, T* data, GLenum usage)
	{
		glBufferData(type, noElements * sizeof(T), data, usage);
	}

	// update
	template<typename T>
	void updateData(GLintptr offset, GLuint noElements, T* data)
	{
		glBufferSubData(type, offset, noElements * sizeof(T), data);
	}

	// set attribute pointers
	template<typename T>
	void setAttPointer(GLuint idx, GLint size, GLenum type, GLuint stride, GLuint offset, GLuint divisor = 0)
	{
		glVertexAttribPointer(idx, size, type, GL_FALSE, stride * sizeof(T), (void*)(offset*sizeof(T)));
		glEnableVertexAttribArray(idx);

		if (divisor)
		{
			// reset idx attribute every divisor iteration
			glVertexAttribDivisor(idx, divisor);
		}
	}

	// clear buffer object
	void clear()
	{
		glBindBuffer(type, 0);
	}

	// cleanup
	void cleanup()
	{
		glDeleteBuffers(1, &location);
	}
};

class ArrayObject 
{
public:
	GLuint location;

	// map of names to buffers
	std::map<const char*, BufferObject> buffers;

	// get buffer (override [])
	BufferObject& operator[](const char* key) 
	{
		return buffers[key];
	}

	// generate object
	void generate()
	{
		glGenVertexArrays(1, &location);
	}

	// bind
	void bind()
	{
		glBindVertexArray(location);
	}

	// cleanup
	void cleanup()
	{
		glDeleteVertexArrays(1, &location);
		for (auto& buffer : buffers)
		{
			buffer.second.cleanup();
		}
	}

	//draw
	void draw(GLenum mode, GLuint count, GLenum type, GLint indices, GLuint instancecount = 1)
	{
		glDrawElementsInstanced(mode, count, type, (void*)indices, instancecount);
	}

	static void clear()
	{
		glBindVertexArray(0);
	}
};