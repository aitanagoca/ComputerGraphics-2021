#include "shader.h"
#include "utils.h"
#include <cassert>
#include <iostream>

#ifndef __APPLE__
REGISTER_GLEXT( GLuint, glCreateProgramObjectARB, void )
REGISTER_GLEXT( void, glLinkProgramARB, GLuint programObj )
REGISTER_GLEXT( void, glGetObjectParameterivARB, GLuint obj, GLenum pname, GLint *params )
REGISTER_GLEXT( void, glValidateProgramARB, GLuint obj )
REGISTER_GLEXT( GLuint, glCreateShaderObjectARB, GLenum shaderType )
REGISTER_GLEXT( void, glShaderSourceARB, GLuint shaderObj, GLsizei count, const GLcharARB* *string, const GLint *length)
REGISTER_GLEXT( void, glCompileShaderARB, GLuint shaderObj )
REGISTER_GLEXT( void, glAttachObjectARB, GLuint containerObj, GLuint obj )
REGISTER_GLEXT( void, glDetachObjectARB, GLuint containerObj, GLuint attachedObj )
REGISTER_GLEXT( void, glDeleteObjectARB, GLuint obj )
REGISTER_GLEXT( void, glUseProgramObjectARB, GLuint obj )
REGISTER_GLEXT( void, glActiveTexture, GLenum texture )
REGISTER_GLEXT( void, glGetInfoLogARB, GLuint obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog )
REGISTER_GLEXT( GLint, glGetUniformLocationARB, GLuint programObj, const GLcharARB *name)
REGISTER_GLEXT( GLint, glGetAttribLocationARB, GLuint programObj, const GLcharARB *name)
REGISTER_GLEXT( void, glUniform1iARB, GLint location, GLint v0 )
REGISTER_GLEXT( void, glUniform2iARB, GLint location, GLint v0, GLint v1 )
REGISTER_GLEXT( void, glUniform3iARB, GLint location, GLint v0, GLint v1, GLint v2 )
REGISTER_GLEXT( void, glUniform4iARB, GLint location, GLint v0, GLint v1, GLint v2, GLint v3 )
REGISTER_GLEXT( void, glUniform1ivARB, GLint location, GLsizei count, const GLint *value )
REGISTER_GLEXT( void, glUniform2ivARB, GLint location, GLsizei count, const GLint *value )
REGISTER_GLEXT( void, glUniform3ivARB, GLint location, GLsizei count, const GLint *value )
REGISTER_GLEXT( void, glUniform4ivARB, GLint location, GLsizei count, const GLint *value )
REGISTER_GLEXT( void, glUniform1fARB, GLint location, GLfloat v0 )
REGISTER_GLEXT( void, glUniform2fARB, GLint location, GLfloat v0, GLfloat v1)
REGISTER_GLEXT( void, glUniform3fARB, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
REGISTER_GLEXT( void, glUniform4fARB, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
REGISTER_GLEXT( void, glUniform1fvARB, GLint location, GLsizei count, const GLfloat *value)
REGISTER_GLEXT( void, glUniform2fvARB, GLint location, GLsizei count, const GLfloat *value)
REGISTER_GLEXT( void, glUniform3fvARB, GLint location, GLsizei count, const GLfloat *value)
REGISTER_GLEXT( void, glUniform4fvARB, GLint location, GLsizei count, const GLfloat *value)
REGISTER_GLEXT( void, glUniformMatrix4fvARB, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
#else
#define glCreateProgramObjectARB glCreateProgram
#define glLinkProgramARB glLinkProgram
#define glGetObjectParameterivARB glGetProgramiv
#define glValidateProgramARB glValidateProgram
#define glCreateShaderObjectARB glCreateShader
#define glShaderSourceARB glShaderSource
#define glCompileShaderARB glCompileShader
#define glAttachObjectARB glAttachShader
#define glDetachObjectARB glDetachShader
#define glDeleteObjectARB glDeleteShader
#define glUseProgramObjectARB glUseProgram
#define glActiveTexture glActiveTexture
#define glGetInfoLogARB glGetProgramInfoLog
#define glGetUniformLocationARB glGetUniformLocation
#define glGetAttribLocationARB glGetAttribLocation
#define glUniform1iARB glUniform1i
#define glUniform2iARB glUniform2i
#define glUniform3iARB glUniform3i
#define glUniform4iARB glUniform4i
#define glUniform1ivARB glUniform1iv
#define glUniform2ivARB glUniform2iv
#define glUniform3ivARB glUniform3iv
#define glUniform4ivARB glUniform4iv
#define glUniform1fARB glUniform1f
#define glUniform2fARB glUniform2f
#define glUniform3fARB glUniform3f
#define glUniform4fARB glUniform4f
#define glUniform1fvARB glUniform1fv
#define glUniform2fvARB glUniform2fv
#define glUniform3fvARB glUniform3fv
#define glUniform4fvARB glUniform4fv
#define glUniformMatrix4fvARB glUniformMatrix4fv

#ifdef GL_VERTEX_SHADER_ARB
#undef GL_VERTEX_SHADER_ARB
#endif
#ifdef GL_FRAGMENT_SHADER_ARB
#undef GL_FRAGMENT_SHADER_ARB
#endif

#define GL_VERTEX_SHADER_ARB GL_VERTEX_SHADER
#define GL_FRAGMENT_SHADER_ARB GL_FRAGMENT_SHADER
#endif


std::map<std::string,Shader*> Shader::s_Shaders;
bool Shader::s_ready = false;


Shader::Shader()
{
	if(!Shader::s_ready)
		Shader::init();
	compiled = false;
}

Shader::~Shader()
{
	release();
}

void Shader::setFilenames(const std::string& vsf, const std::string& psf)
{
	vs_filename = vsf;
	ps_filename = psf;
}

bool Shader::load(const std::string& vsf, const std::string& psf)
{
	assert(	compiled == false );
	assert (glGetError() == GL_NO_ERROR);

	std::string vs_fullpath = absResPath(vsf);
	std::string ps_fullpath = absResPath(psf);

	vs_filename = vsf;
	ps_filename = psf;

	printf("Vertex shader:\n%s\n", vsf.c_str());
	printf("Fragment shader:\n%s\n", psf.c_str());
	std::string vsm,psm;
	if (!readFile(vs_fullpath,vsm) || !readFile(ps_fullpath,psm))
		return false;

	//printf("Vertex shader from memory:\n%s\n", vsm.c_str());
	//printf("Fragment shader from memory:\n%s\n", psm.c_str());

	if (!compileFromMemory(vsm,psm))
		return false;

	assert (glGetError() == GL_NO_ERROR);

	return true;
}

Shader* Shader::Get(const char* vsf, const char* psf)
{
	std::string name = std::string(vsf) + "," + std::string(psf);
	std::map<std::string,Shader*>::iterator it = s_Shaders.find(name);
	if (it != s_Shaders.end())
		return it->second;

	Shader* sh = new Shader();
	if (!sh->load(vsf,psf))
		return NULL;
	s_Shaders[name] = sh;
	return sh;
}

void Shader::ReloadAll()
{
	for( std::map<std::string,Shader*>::iterator it = s_Shaders.begin(); it!=s_Shaders.end();it++)
		it->second->recompile();
	std::cout << "Shaders recompiled" << std::endl;
}

bool Shader::compile()
{
	assert(!compiled && "Shader already compiled" );
    return load(vs_filename,ps_filename);
}

bool Shader::recompile()
{
	release(); //remove old shader
    return load(vs_filename,ps_filename);
}

bool Shader::readFile(const std::string& filename, std::string& content)
{
	content.clear();

	int count=0;

	FILE *fp = fopen(filename.c_str(),"rb");
	if (fp == NULL) 
	{
		printf("Shader::readFile: file not found %s\n",filename.c_str());
		return false;
	}

	fseek(fp, 0, SEEK_END);
	count = ftell(fp);
	rewind(fp);

	content.resize(count);
	if (count > 0) 
	{
		count = fread(&content[0],sizeof(char),count,fp);
	}
	fclose(fp);

	return true;
}

std::string Shader::getInfoLog() const
{
	return info_log;
}

bool Shader::hasInfoLog() const
{
	return info_log.size() > 0; 
}

// ******************************************

bool Shader::compileFromMemory(const std::string& vsm, const std::string& psm)
{
	
	if (glCreateProgramObjectARB == 0)
	{
		std::cout << "Error: your graphics cards dont support shaders. Sorry." << std::endl;
		exit(0);
	}

	program = glCreateProgramObjectARB();
	assert (glGetError() == GL_NO_ERROR);

	if (!createVertexShaderObject(vsm))
	{
		printf("Vertex shader compilation failed\n");
		return false;
	}

	if (!createFragmentShaderObject(psm))
	{
		printf("Fragment shader compilation failed\n");
		return false;
	}

	glLinkProgramARB(program);
	assert (glGetError() == GL_NO_ERROR);

	GLint linked=0;
#ifdef __APPLE__
	glGetProgramiv( program, GL_LINK_STATUS, &linked );
#else
	glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &linked);
#endif
	assert(glGetError() == GL_NO_ERROR);

	if (!linked)
	{
		saveInfoLog(program);
		release();
		return false;
	}

#ifdef _DEBUG
	validate();
#endif

	compiled = true;

	return true;
}

bool Shader::validate()
{
	glValidateProgramARB(program);
	assert ( glGetError() == GL_NO_ERROR );

	GLint validated = 0;
#ifdef __APPLE__
	glGetProgramiv( program, GL_LINK_STATUS, &validated );
#else	
	glGetObjectParameterivARB(program,GL_OBJECT_VALIDATE_STATUS_ARB,&validated);
#endif
	assert(glGetError() == GL_NO_ERROR);
	
	if (!validated)
	{
		printf("Shader validation failed\n");
		saveInfoLog(program);
		return false;
	}

	return true;
}

bool Shader::createVertexShaderObject(const std::string& shader)
{
	return createShaderObject(GL_VERTEX_SHADER_ARB,vs,shader);
}

bool Shader::createFragmentShaderObject(const std::string& shader)
{
	return createShaderObject(GL_FRAGMENT_SHADER_ARB,fs,shader);
}

bool Shader::createShaderObject(unsigned int type, GLuint& handle, const std::string& shader)
{
	handle = glCreateShaderObjectARB(type);
	assert (glGetError() == GL_NO_ERROR);

	const char* ptr = shader.c_str();
	glShaderSourceARB(handle, 1, &ptr, NULL);
	assert ( glGetError() == GL_NO_ERROR);
	
	glCompileShaderARB(handle);
	assert( glGetError() == GL_NO_ERROR);

	GLint compile=0;
#ifdef __APPLE__
	glGetShaderiv( handle, GL_COMPILE_STATUS, &compile );
#else
	glGetObjectParameterivARB(handle,GL_OBJECT_COMPILE_STATUS_ARB,&compile);
#endif
	assert( glGetError() == GL_NO_ERROR);

	//we want to see the compile log if we are in debug (to check warnings)
	if (!compile)
	{
		saveInfoLog(handle);
		printf("Shader code:%s\n", shader.c_str());
		return false;
	}

	glAttachObjectARB(program,handle);
	assert( glGetError() == GL_NO_ERROR);

	return true;
}


void Shader::release()
{
	if (program && vs)
	{
		glDetachObjectARB(program,vs);
		assert (glGetError() == GL_NO_ERROR);
		glDeleteObjectARB(vs);
		assert (glGetError() == GL_NO_ERROR);
		vs = 0;
	}

	if (program && fs)
	{
		glDetachObjectARB(program,fs);
		assert (glGetError() == GL_NO_ERROR);
		glDeleteObjectARB(fs);
		assert (glGetError() == GL_NO_ERROR);
		fs = 0;
	}

	if (program)
	{
		glDeleteObjectARB(program);
		assert (glGetError() == GL_NO_ERROR);
		program = 0;
	}

	compiled = false;
}


void Shader::enable()
{
	glUseProgramObjectARB(program);
	assert (glGetError() == GL_NO_ERROR);

	last_slot = 0;
}


void Shader::disable()
{
	glUseProgramObjectARB(0);
	glActiveTexture(GL_TEXTURE0);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::disableShaders()
{
	glUseProgramObjectARB(0);
	assert (glGetError() == GL_NO_ERROR);
}


void Shader::saveInfoLog(GLuint obj)
{
	int len = 0;
#ifdef __APPLE__	
	glGetShaderiv( obj, GL_INFO_LOG_LENGTH, &len );
#else
	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &len);
#endif
	assert(glGetError() == GL_NO_ERROR);

	if (len > 0)
	{
		char* ptr = new char[len+1];
		GLsizei written=0;
		glGetInfoLogARB(obj, len, &written, ptr);
		ptr[written-1]='\0';
		assert(glGetError() == GL_NO_ERROR);
		log.append(ptr);
		delete[] ptr;

		printf("LOG **********************************************\n%s\n",log.c_str());
	}
}

GLint Shader::getLocation(const char* varname,loctable* table)
{
	if(varname == 0 || table == 0)
		return 0;

	GLint loc = 0;
	loctable* locs = table;

	loctable::iterator cur = locs->find(varname);
	
	if(cur == locs->end()) //not found in the locations table
	{
		loc = (GLint)glGetUniformLocationARB(program, varname);
		if (loc == -1)
		{
			return -1;
		}

		//insert the new value
		locs->insert(loctable::value_type(varname,loc));
	}
	else //found in the table
	{
		loc = (*cur).second;
	}
	return loc;
}

int Shader::getAttribLocation(const char* varname)
{
	GLint loc = glGetAttribLocationARB(program, varname);
	if (loc == -1)
	{
		return loc;
	}
	assert(glGetError() == GL_NO_ERROR);

	return loc;
}

int Shader::getUniformLocation(const char* varname)
{
	int loc = getLocation(varname, &locations);
	if (loc == -1)
	{
		return loc;
	}
	assert(glGetError() == GL_NO_ERROR);
	return loc;
}

void Shader::setTexture(const char* varname, Texture* tex)
{
	glActiveTexture(GL_TEXTURE0 + last_slot);
	glBindTexture(GL_TEXTURE_2D, tex->texture_id);
	setUniform1(varname, last_slot);
	last_slot++;
	glActiveTexture(GL_TEXTURE0 + last_slot);
}

void Shader::setTexture(const char* varname, unsigned int tex)
{
	glActiveTexture(GL_TEXTURE0 + last_slot);
	glBindTexture(GL_TEXTURE_2D,tex);
	setUniform1(varname,last_slot);
	last_slot++;
	glActiveTexture(GL_TEXTURE0 + last_slot);
}

void Shader::setTexture(const char* varname, Texture* tex, unsigned int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex->texture_id);
	setUniform1(varname, (int)slot);
	last_slot++;
	glActiveTexture(GL_TEXTURE0 + last_slot);
}

void Shader::setUniform1(const char* varname, int input1)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform1iARB(loc, input1);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform2(const char* varname, int input1, int input2)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform2iARB(loc, input1, input2);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform3(const char* varname, int input1, int input2, int input3)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform3iARB(loc, input1, input2, input3);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform4(const char* varname, const int input1, const int input2, const int input3, const int input4)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform4iARB(loc, input1, input2, input3, input4);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform1Array(const char* varname, const int* input, const int count)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform1ivARB(loc,count,input);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform2Array(const char* varname, const int* input, const int count)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform2ivARB(loc,count,input);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform3Array(const char* varname, const int* input, const int count)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform3ivARB(loc,count,input);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform4Array(const char* varname, const int* input, const int count)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform4ivARB(loc,count,input);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform1(const char* varname, const float input1)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform1fARB(loc, input1);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform2(const char* varname, const float input1, const float input2)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform2fARB(loc, input1, input2);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform3(const char* varname, const float input1, const float input2, const float input3)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform3fARB(loc, input1, input2, input3);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform4(const char* varname, const float input1, const float input2, const float input3, const float input4)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform4fARB(loc, input1, input2, input3, input4);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform1Array(const char* varname, const float* input, const int count)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform1fvARB(loc,count,input);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform2Array(const char* varname, const float* input, const int count)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform2fvARB(loc,count,input);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform3Array(const char* varname, const float* input, const int count)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform3fvARB(loc,count,input);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setUniform4Array(const char* varname, const float* input, const int count)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniform4fvARB(loc,count,input);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setMatrix44(const char* varname, const float* m)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniformMatrix4fvARB(loc, 1, GL_FALSE, m);
	assert (glGetError() == GL_NO_ERROR);
}

void Shader::setMatrix44(const char* varname, const Matrix44 &m)
{
	GLint loc = getLocation(varname, &locations);
	CHECK_SHADER_VAR(loc,varname);
	glUniformMatrix4fvARB(loc, 1, GL_FALSE, m.m);
	assert (glGetError() == GL_NO_ERROR);
}


void Shader::init()
{
	static bool firsttime = true;
	Shader::s_ready = true;
#ifndef __APPLE__	
	if(firsttime)
	{
		IMPORT_GLEXT( glCreateProgramObjectARB );
		IMPORT_GLEXT( glLinkProgramARB );
		IMPORT_GLEXT( glGetObjectParameterivARB );
		IMPORT_GLEXT( glValidateProgramARB );
		IMPORT_GLEXT( glCreateShaderObjectARB );
		IMPORT_GLEXT( glShaderSourceARB );
		IMPORT_GLEXT( glCompileShaderARB );
		IMPORT_GLEXT( glAttachObjectARB );
		IMPORT_GLEXT( glDetachObjectARB );
		IMPORT_GLEXT( glDeleteObjectARB );
		IMPORT_GLEXT( glUseProgramObjectARB );
		IMPORT_GLEXT( glActiveTexture );
		IMPORT_GLEXT( glGetInfoLogARB );
		IMPORT_GLEXT( glGetUniformLocationARB );
		IMPORT_GLEXT( glGetAttribLocationARB );
		IMPORT_GLEXT( glUniform1iARB );
		IMPORT_GLEXT( glUniform2iARB );
		IMPORT_GLEXT( glUniform3iARB );
		IMPORT_GLEXT( glUniform4iARB );
		IMPORT_GLEXT( glUniform1ivARB );
		IMPORT_GLEXT( glUniform2ivARB );
		IMPORT_GLEXT( glUniform3ivARB );
		IMPORT_GLEXT( glUniform4ivARB );
		IMPORT_GLEXT( glUniform1fARB );
		IMPORT_GLEXT( glUniform2fARB );
		IMPORT_GLEXT( glUniform3fARB );
		IMPORT_GLEXT( glUniform4fARB );
		IMPORT_GLEXT( glUniform1fvARB );
		IMPORT_GLEXT( glUniform2fvARB );
		IMPORT_GLEXT( glUniform3fvARB );
		IMPORT_GLEXT( glUniform4fvARB );
		IMPORT_GLEXT( glUniformMatrix4fvARB );
	}
#endif
	
	firsttime = false;
}
