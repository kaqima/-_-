#pragma once
//        用此文件需要主要的事项				//
/*
①		需要配置sb7的	include	lib		dll		的引用目录
②		Vertex					location=0
③		Color						location=1
④     Normal					location=2
⑤     texCoord				location=3

*/


#include"C:\Users\liubi\Documents\sb7code-master\include\vmath.h"
#include"C:\Users\liubi\Documents\sb7code-master\include\sb7.h"
#include<iostream>
#include<fstream>
#include<string>
#include<iomanip>
#include<time.h>

using namespace vmath;
using namespace std;

namespace LB_OPENGL {


	
		enum shader_type		//enum of shader type
		{
			vertex,
			tessellation_control,
			tessellation_evaluation,
			geometry,
			fragment,
			compute
		};

		struct vertex_info	//position information
		{
			vec4 position;
			vec4 color;
			vec3 normal;
			vec2 texCoord;
		};

	//----------------------------------------------------------------------------------------//
	//									class of vertex array							  			//
	//---------------------------------------------------------------------------------------//
	class VertexArrayObject {
	public:
		VertexArrayObject() 
		{
			handle = 0;
			glCreateVertexArrays(1,&handle);
			if (glIsVertexArray(handle) == GL_FALSE)
				cerr << "Vertex Error:" << endl
				<< "Fail to create Vertex Array Object" << endl;
			else
				cerr << "Vertex Infor:" << endl
				<< "Succeed to creat Vertex Array Object " << handle << endl;
		}

		~VertexArrayObject()
		{
			if (handle > 0)
				if (glIsVertexArray(handle) == GL_TRUE)
				{
					cerr << "Vertex Info:" << endl
						<< "Succeed to delete Vertex Array Object " << handle << endl;
					glDeleteVertexArrays(1, &handle);
				}
		}

		// use  glVertexAttribPointer to set the VertexArray
		void setVertexPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* offset_pointer)
		{
			bind();
			glVertexAttribPointer(index, size, type, normalized, stride, offset_pointer);
			glEnableVertexAttribArray(index);
		}
		void bind() {
			glBindVertexArray(handle);
		}
		void unbind()
		{
			glBindVertexArray(0);
		}
		GLuint getHnadle()
		{
			return handle;
		}
	private:	
		GLuint handle;


	};

	//------------------------------------------------------------------------------------------//
	//								class of BufferObject_Immutable							   //
	//------------------------------------------------------------------------------------------//
	class BufferObject_Immutable {
	public:
		BufferObject_Immutable(GLsizeiptr s,const void*data,GLbitfield f)
			/*
				(
				size-------------------Size of data,
				data---------------------------data,
				flag---------------------------usage
				)
			*/
		{
			handle = 0;
			size = s;
			glCreateBuffers(1, &handle);
			glNamedBufferStorage(handle, s, data, f);
			this->flag = f;
		}
		~BufferObject_Immutable()
		{
			if (handle > 0)
				if (glIsBuffer(handle) == GL_TRUE)
					glDeleteBuffers(1, &handle);
		}

		void updataData(GLintptr off, GLsizeiptr s, const void*d)
		{
			if ((off + s) <= size)
			{
				if (this->flag == GL_DYNAMIC_STORAGE_BIT)
				{
					glNamedBufferSubData(handle, off, s, d);
				}
				else
				{
					cerr << "Buffer Error:" << endl
						<< "		Can't updata the data of Buffer Object "<<handle<<". because of not GL_DYNAMIC_STORAGE_BIT" << endl;
				}
			}
			else
				cerr << "Buffer Error:" << endl
				<< "		Can't updata the data of Buffer Object " << handle << ". Because the size of update data is exceed the size of Buffer Object" << endl;
		}
		void bind(GLenum target)
		{
			glBindBuffer(target, handle);
		}
	private:
		GLuint handle;
		GLsizeiptr size;
		GLbitfield flag;
	};

	class BufferObject_Mutable
	{
	public:
		BufferObject_Mutable()
		{
			glCreateBuffers(1, &name);;
		}
		~BufferObject_Mutable()
		{
			if (name > 0)
			{
				if (glIsBuffer(name) == GL_TRUE)
				{
					glDeleteBuffers(1, &name);
					if (glIsBuffer(name) == GL_FALSE)
					{
						cerr << "Mutable Buffer Object Info:" << endl
							<< "Mutable Buffer Object Name delete successfully" << endl;
					}
					else
					{
						cerr << "Mutable Buffer Object Error:" << endl
							<< "Failed to Delete Mutable Buffer Object Name" << endl;
					}
				}
				else
				{
					cerr << "Mutable Buffer Object Info:" << endl
						<< "Mutable Buffer Object Name is empty" << endl;
				}
			}
			else
			{
				cerr << "Mutable Buffer Object Info:" << endl
					<< "Mutable Buffer Object Name is empty" << endl;
			}

			cerr << "Mutable Buffer Object Class release successfully" << endl;
		}



	private:
		GLuint name;

	};

	//--------------------------------------------------------------------------//
	//									Shader Class									//
	//-------------------------------------------------------------------------//
	class Shader {
	public:
		Shader(LB_OPENGL::shader_type t)			//		construction function(shader_type)
		{
			this->link = false;
			shaderType = t;
			initialize();
		}
		Shader(LB_OPENGL::shader_type t, const char* file_name)			//construction function(shadertype , filename)
		{
			link = false;
			shaderType = t;
			initialize();
			loadShaderFromFile(file_name);
		}
		~Shader()
		{
			if (glIsShader(handle) == GL_TRUE)
			{
				glDeleteShader(handle);
			}
		}

		//////////////		method function		//////////////
		shader_type& getType()				//return the type of shader
		{
			return shaderType;
		}
		void loadShaderFromFile(const char*filename)					//		load shader from file
		{
			const GLchar* source = readFile(filename);
			loadShader(source);
			if (source == NULL)
				return;
			delete[]source;

		}
		void loadShader(const GLchar*source)			// compile source of shader
		{
			if (source == NULL)
				return;
			glShaderSource(handle, 1, &source, NULL);

			glCompileShader(handle);
			GLint compiled;
			glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);
			if (!compiled)
			{
				GLsizei len;
				glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &len); \

					GLchar* log = new GLchar[len + 1];
				glGetShaderInfoLog(handle, len, &len, log);
				cerr << "Shader Error:" << endl
					<< "		Fail to compile shader !	" << log << endl;
				delete[]log;
			}
			else
				link = true;
		}
		bool getLink() const
		{
			return link;
		}
		GLuint getHandle() const
		{
			return handle;
		}
		GLenum getType()const {
			GLenum t;
			switch (shaderType)
			{
			case LB_OPENGL::vertex:
				t = GL_VERTEX_SHADER;
				break;
			case LB_OPENGL::tessellation_control:
				t = GL_TESS_CONTROL_SHADER;
				break;
			case LB_OPENGL::tessellation_evaluation:
				t = GL_TESS_EVALUATION_SHADER;
				break;
			case LB_OPENGL::geometry:
				t = GL_GEOMETRY_SHADER;
				break;
			case LB_OPENGL::fragment:
				t = GL_FRAGMENT_SHADER;
				break;
			case LB_OPENGL::compute:
				t = GL_COMPUTE_SHADER;
				break;
			default:
				break;
			}
			return t;
		}
	private:
		GLuint handle;
		shader_type shaderType;
		bool link;

		///////					method function						////////
		void initialize()		// create a sahder with the shaderType
		{
			//		initialize the shader according shader type		//
			GLenum t;															//
			switch (shaderType)											//
			{																		//
			case LB_OPENGL::vertex:									//
				t = GL_VERTEX_SHADER;								//
				break;															//
			case LB_OPENGL::tessellation_control:					//
				t = GL_TESS_CONTROL_SHADER;					//
				break;															//
			case LB_OPENGL::tessellation_evaluation:				//
				t = GL_TESS_EVALUATION_SHADER;				//
				break;															//
			case LB_OPENGL::geometry:								//
				t = GL_GEOMETRY_SHADER;							//
				break;															//
			case LB_OPENGL::fragment:								//
				t = GL_FRAGMENT_SHADER;							//
				break;															//
			case LB_OPENGL::compute:									//
				t = GL_COMPUTE_SHADER;								//
				break;															//
			default:																//
				t = GL_VERTEX_SHADER;								//
				break;															//
			}																		//
			handle = glCreateShader(t);									//
		////////////													////////////
			if (handle <= 0 || glIsShader(handle) == GL_FALSE)		//judge whether glcreateshader successfully or not
				cerr << "Shader Error:" << endl
				<< "		failed to create shader object !" << endl;
			else
				cerr << "Shader Info:"
				<< "		succeed to creat shader" << endl;
		}
		static const  GLchar* readFile(const char*filename)			//		read file content 
		{
			FILE* f;
#ifdef WIN32
			fopen_s(&f, filename, "rb");
#else
			f = fopen(filename, "rb");
#endif // WIN32
			if (!f)
			{
				std::cerr << "Shader Error:" << endl
					<< "Unable to open file !" << endl;
				return NULL;
			}
			fseek(f, 0, SEEK_END);
			int len = ftell(f);
			fseek(f, 0, SEEK_SET);

			GLchar *source = new GLchar[len + 1];
			fread(source, 1, len, f);
			fclose(f);

			source[len] = 0;

			return const_cast<const GLchar*>(source);
		}

		
	};

	//-------------------------------------------------------------//
	//-------------------program class --------------------------//
	//-------------------------------------------------------------//
	class Program {
	public:
		Program()
		{
			this->linked = false;
			handle = 0;
			handle = glCreateProgram();
		}
		~Program()
		{
			if (glIsProgram(handle) == GL_TRUE)
			{
				glDeleteProgram(handle);
			}
		}
		void attach(Shader& s) const
		{
			glAttachShader(handle, s.getHandle());
		}
		void detach(Shader& s)const {
			glDetachShader(handle, s.getHandle());
		}
		void link()
		{
			glLinkProgram(handle);
			GLint status;
			glGetProgramiv(handle, GL_LINK_STATUS, &status);
			if (GL_FALSE == status)
			{
				cerr << "Program Error:" << endl
					<< "		Failed to link shader program !" << endl;
				GLint loglen;
				glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &loglen);
				if (loglen > 0)
				{
					char*log = new char[loglen];
					GLsizei written;
					glGetProgramInfoLog(handle, loglen, &written, log);
					cerr << "Error Info:" << "  " << log << endl;
					delete[]log;
				}
				cerr << "		And there is no log information ! " << endl;
			}
			else
				this->linked = true;
		}
		void use()
		{
			if (linked)
				glUseProgram(handle);
			else
				cerr << "Program Error:" << endl
				<< "		Can't not use because of not linked program" << endl;
		}
		GLuint getHandle()const {
			return handle;
		}
		GLuint getlocation(GLchar *na)
		{
			return glGetUniformLocation(handle, na);
		}
		void setUniform(GLchar* unifName, const mat4 &&m)
		{
			GLuint mat4_location = glGetUniformLocation(handle, unifName);
			glUniformMatrix4fv(mat4_location, 1, GL_FALSE, m);
		}
		void setUniform(GLuint location, const mat4 &m)
		{
			glUniformMatrix4fv(location,1,GL_FALSE, m);
		}
		void setUniform(GLuint location, const vec4 &m)
		{
			glUniform4fv(location,1, m);

		}
		//void setUniform(GLuint location,const vec4)
	private:
		GLuint handle;
		bool linked;
	};

	//----------------------------------------------------------------//
	//						可分离Program  class-------------------//
	//---------------------------------------------------------------//
	class SeperableProgram {
	public:
		SeperableProgram(shader_type t)
		{
			handle = 0;
			this->linked = false;
						handle = glCreateProgram();
			if (glIsProgram(handle) == GL_TRUE)
				glProgramParameteri(handle, GL_PROGRAM_SEPARABLE, GL_TRUE);
			else
			{
				cerr << "SeperableProgram Error:" << endl
					<< "		failed to creat program handle of SeperableProgram Class" << endl;
			}
		}
		~SeperableProgram()
		{
			if (glIsProgram(handle) == GL_TRUE)
			{
				glDeleteProgram(handle);
			}
		}
		void link() {
			glLinkProgram(handle);
		}
		void attach(Shader& s)
		{
			glAttachShader(handle, s.getHandle());
		}
		void detach(Shader& s)
		{
			glDetachShader(handle, s.getHandle());
		}		
		GLuint getHandle()
		{
			return handle;
		}
	private:
		GLuint handle;
		bool linked;

	};




//================================================================================================//

	////旋转vec4函数

	enum direction {
		front, back, left, right, up, down
	};

	enum coordinate {
		X, Y, Z, W
	};

	
	inline vmath::vec4 rotate(vmath::vec4 a, float angle, coordinate d)
	{
		float rads = float(angle) * 0.0174532925f;

		switch (d)
		{
		case LB_OPENGL::X:
			return vmath::vec4(
				a.getdata(0),
				a.getdata(1) * cosf(rads) - a.getdata(2) * sinf(rads),
				a.getdata(2) * cosf(rads) + a.getdata(1) * sinf(rads),
				a.getdata(3)
			);
			break;
		case LB_OPENGL::Y:
			return vmath::vec4(
				a.getdata(0) * cosf(rads) + a.getdata(2) * sinf(rads),
				a.getdata(1),
				a.getdata(2) * cosf(rads) - a.getdata(0) * sinf(rads),
				a.getdata(3)
			);
			break;
		case LB_OPENGL::Z:
			return vmath::vec4(a.getdata(0)*cosf(rads) - a.getdata(1)*sinf(rads),
				a.getdata(1)*cosf(rads) + a.getdata(0)*sinf(rads),
				a.getdata(2),
				a.getdata(3)
			);
			break;
		case LB_OPENGL::W:
		default:
			return a;
			break;
		}
	}

	inline	vmath::vec3 rotate(vmath::vec3 a, float angle, coordinate d)
	{
		vmath::vec4 out = rotate(vmath::vec4(a, 1.0f), angle, d);
		return vmath::vec3(out.getdata(0), out.getdata(1), out.getdata(2));
	}

	class FreeCamera {
	public:
		////construct function
		FreeCamera()
		{
			position_eye = vmath::vec3(0.0f, 0.0f, 1.0f);
			angle_vertical = 0;
			angle_horizontal = 0;
			angle_speed = 0.5;
			move_speed = 0.002;
		}

		FreeCamera(vmath::vec3 eye_position)
		{
			position_eye = eye_position;
			angle_vertical = 0;
			angle_horizontal = 0;

		}

		FreeCamera(float v, float h)/*****v为仰角，h为绕y轴顺时针的角度****/
		{
			angle_vertical = v;
			angle_horizontal = h;
			position_eye = vmath::vec3(0.0f, 0.0f, 1.0f);
			angle_speed = 0.5;
			move_speed = 0.002;
		}

		FreeCamera(vmath::vec3 eye_position, float v, float h)
		{
			position_eye = eye_position;
			angle_vertical = v;
			angle_horizontal = h;
			angle_speed = 0.5;
			move_speed = 0.002;
		}
		///normal function
		inline void move_step(direction d)
		{
			vmath::vec3 direction = vmath::vec3(0.0f, 0.0f, -move_speed);

			switch (d)
			{
			case LB_OPENGL::front:
				position_eye += rotate(rotate(direction, angle_horizontal, X), angle_vertical, Y);
				break;
			case LB_OPENGL::back:
				position_eye -= rotate(rotate(direction, angle_horizontal, X), angle_vertical, Y);
				break;
			case LB_OPENGL::left:
				position_eye += rotate(direction, angle_vertical + 90, Y);
				break;
			case LB_OPENGL::right:
				position_eye -= rotate(direction, angle_vertical + 90, Y);
				break;
			case LB_OPENGL::up:
				position_eye += vmath::vec3(0.0f, move_speed, 0.0f);
				break;
			case  LB_OPENGL::down:
				position_eye -= vmath::vec3(0.0, move_speed, 0.0f);
				break;
			default:
				break;
			}
		}

		inline void move_angle(direction d)
		{

			switch (d)
			{
			case LB_OPENGL::up:
				if (angle_horizontal + angle_speed<90)
					angle_horizontal += angle_speed;
				break;
			case LB_OPENGL::down:
				if (angle_horizontal - angle_speed>-90)
					angle_horizontal -= angle_speed;
				break;
			case LB_OPENGL::left:
				angle_vertical += angle_speed;
				break;
			case LB_OPENGL::right:
				angle_vertical -= angle_speed;
				break;
			default:
				break;
			}
			if (angle_vertical >= 360)
				angle_vertical -= 360;
			if (angle_vertical <= -360)
				angle_vertical += 360;
		}

		inline vmath::vec3 get_position_eye()
		{
			vmath::vec3 p = position_eye;
			return p;
		}

		inline vmath::vec3 get_position_look()
		{
			vmath::vec3 p = rotate(rotate(vmath::vec3(0.0f, 0.0f, -1.0f), angle_horizontal, X), angle_vertical, Y) + position_eye;
			return p;
		}
		inline void setEyePosition(vmath::vec3 p)
		{
			position_eye = p;
		}
		inline void setAngleSpeed(float s)
		{
			angle_speed = s;
		}
		inline void setMoveSpeed(float s)
		{
			move_speed = s;
		}
	private:
		vmath::vec3 position_eye;
		float angle_vertical;
		float angle_horizontal;

		float angle_speed;
		float move_speed;
	};
//=============================================================================================================||
	class FixedCamera
	{
	public:
	//---			CONSTRUCTION			---//
		FixedCamera()
		{
			position_focus = vmath::vec3(0.0f, 0.0f, 0.0f);
			distance = 1.0f;
			angle_horizontal = 0.0f;
			angle_vertical = 0.0f;
			speed_move = 0.1f;
			speed_roll = 1.0f;
		}

		FixedCamera(vmath::vec3 p)
		{
			position_focus = p;
			distance = 1.0f;
			angle_horizontal = 0.0f;
			angle_vertical = 0.0f;
			speed_move = 0.1f;
			speed_roll = 1.0f;
		}
		~FixedCamera()
		{

		}

		inline void move(direction d)
		{
			float tem;
			switch (d)
			{
			case LB_OPENGL::front:
				tem = distance - speed_move;
				if (tem > 0)
					distance = tem;
				break;
			case LB_OPENGL::back:
				distance += speed_move;
				break;
			case LB_OPENGL::left:
					angle_vertical += speed_roll*90/(90-angle_horizontal);
				if (angle_vertical >= 360)
					angle_vertical -= 360;
				break;
			case LB_OPENGL::right:
					angle_vertical -= speed_roll*90/(90-angle_horizontal);
				if (angle_vertical <= -360)
					angle_vertical += 360;
				break;
			case LB_OPENGL::up:
				tem = angle_horizontal + speed_roll;
				if(tem<90)
					angle_horizontal =tem;
				break;
			case LB_OPENGL::down:
				tem = angle_horizontal - speed_roll;
				if (tem > -90)
					angle_horizontal = tem;

				break;
			default:
				break;
			}
		}
		inline vmath::mat4 getViewMatrix()
		{
			vec3 t = rotate(vec3(0.0f, 0.0f, distance), angle_horizontal, X);
			vec3 up = rotate(rotate(t, angle_horizontal - 90, X), angle_vertical, Y);
			vec3 eye = rotate(t, angle_vertical, Y);
			return vmath::lookat(eye+position_focus, position_focus, up);
		}
		inline void setDistance(float d)
		{
			distance = d;
		}
		inline void setSpeedRoll(float s)
		{
			speed_roll = s;
		}
		inline void setSpeedMove(float s)
		{
			speed_move = s;
		}
		inline void setFocusPosition(vmath::vec3 p)
		{
			position_focus = p;
		}
	private:
		vmath::vec3 position_focus;
		float distance;
		float angle_vertical;
		float angle_horizontal;
		float speed_roll;
		float speed_move;
	};
//============================================================================================
	class Object {
	public:
		virtual ~Object(){}
		virtual void upDate() = 0;
		virtual void draw() = 0;
		vmath::mat4 rotate;
		vmath::mat4 scale;
		vmath::mat4 translate;
		VertexArrayObject *format;
		BufferObject_Immutable *data;
	}; 
//============================================================================================
	class ReadSTL {
	public:
		ReadSTL(char* filename)
		{
			file.open(filename, ios_base::in|ios_base::binary);
			if (file.bad())
			{
				std::cerr << "ReadStlFile	Error:" << endl
					<< "Can't open the \""<<filename<<"\"STL file 	!" << endl;
			}
			else
			{
				std::cerr << "Succeed to open " << filename << endl;
				
				file.seekg(0,ios::end);
				long lSize = file.tellg();
				cout<<filename<<"的长度是:"<< lSize<<" BIT "<<endl;
				file.seekg(0, ios::beg);

				if (file.peek() == 's')
				{
					is_binary = false;
					readAscll();
				}
				else
				{
					size = lSize;
					is_binary = true;
					readBinary();
				}
			}				
		}

		~ReadSTL()
		{

			if (file.is_open())
			{
				file.close();
				std::cerr << "Succeed to close STL file	!" << endl;
			}
			if (buffer != NULL)
			{
				delete[]buffer;
			}
		}

		long getSize()
		{
			return size;
		}
		char *getBuffer()
		{
			return buffer;
		}

		bool isBinary()
		{
			return is_binary;
		}
	private:
		void readBinary()
		{
			char stl_info[81];
			stl_info[80] = 0;
			file.read(stl_info, 80);
			cerr << "BinarySTL Info:" << endl << stl_info << endl;
			///////////////////////////////////////
			//此块是读取stl模块
#define uint unsigned int
			uint vertex_number=0;
			file.read((char*)&vertex_number, sizeof(uint));
			cerr << "total " << vertex_number << " 个三角形 !" << endl;

			size -= (80 + sizeof(uint));
			buffer= new char[size];
			file.read(buffer, size);
#undef uint
			///////////////////////////////////////

		}
		void readAscll()
		{
			cerr << "无法读取ASCLL格式的STL文件，因为还没有完成这个功能" << endl;
			return;
		}
		ifstream file;
		long size;
		char *buffer;
		bool is_binary;
	};
}


