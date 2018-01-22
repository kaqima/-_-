#pragma once
#include"C:\Users\liubi\Documents\sb7code-master\include\sb7.h"

const static GLchar* verShader=
{
"		#version 450 core																														\n"

"			//	**********					MACRO				DEFINATION		*************//							\n"
"		#define transform_normal(x) transpose(inverse(x))																		\n"
"		#define MAX_PARALLEL_LIGHT_NUMBER		10																			\n"
//				defination		fo		struction			//
"	struct parallei_light																															\n"
"	{																																					\n"
"		vec3 direction;																															\n"
"		vec3 color;																																	\n"
"		float 	intensity;																															\n"
"	};																																				\n"

//			 the information of per vertex				//
"																																						\n"																			
"																																						\n "
"//======-------------			THE		INFORMATION		OF			INPUT		------------======//			\n"
"			layout ( location = 0 )  in  vec3 position;																					\n"
"																																						\n"
"			layout ( location = 2 )  in  vec3 normal;																						\n"
"           layout ( location = 3 )  in  vec2 texCoord;																					\n"

//			output the in of transformed vertex
"		out Info																																		\n"
"		{																																				\n"
"				vec4 Color;																															\n"
"				vec2 TexCoord;																													\n"
"																																						\n"
"		};																																				\n"
/////////////////////////		vertex shader		/////////////////////////
/////////////			用来调试的部分		/////////////////
"		out vec4 nor;																	\n"



////////////////////////////////////////////////////////
//					UNIFORM		VARIABLE
"		 uniform mat4 project;		//	PROJECTION MATRIX																		\n"
"		 uniform mat4 view;			//	MATRIX TRANSFORMING WORLD TO CAMERA COORDINATE				\n"
"		 uniform mat4 scale;			//	SCALE THE MODEL																			\n"
"		 uniform mat4 rotate;			// ROTATE THE MODEL																		\n"
"		 uniform mat4 translate;		// TRANSLATE THE MODEL																	\n"
"		 uniform vec4 color;			//	THE	COLOR OF	OBJECT																										\n"			
"		 																																			\n"
"																																						\n"
"																																						\n"
"																																						\n"
"																																						\n"
"																																						\n"
"																																						\n"
"																																						\n"
//=======						main						=======//
"		void main()																																\n"
"		{																														\n"
"																						 														\n"
"			mat4 model_to_view=view*translate*rotate*scale;														\n"			
"			vec4 n=transform_normal(model_to_view)*vec4(normal,0.0f);																																			\n"
//					evalue the transform
			
//			calculate the light
"			vec4 light1=normalize(	transform_normal(view)*	vec4(0.0f,1.0f,1.0f,0.0f) );																																			\n"
"			vec4 light2	=normalize(	transform_normal(view)*vec4(1.0f,-1.0f,0.0f,0.0f) );																																		\n"
"			vec4 light3=normalize(	transform_normal(view)*vec4 (0.0f,1.0f,1.0f,0.0f) );																																			\n"
"			vec4 light4	=normalize(	transform_normal(view)*vec4(1.0f,0.0f,1.0f,0.0f) );																																		\n"
//						transform the Position									//			
"			gl_Position = project*model_to_view*vec4(position,1.0f);																							\n"
//			calculate the color of diffuse light
"			vec4 haha=vec4(0.0f,0.0f,1.0f,0.0f);																																						\n"
"			vec4 diffuse=   max(	0.0f, -dot(	n ,light1	)	)*color*vec4(0.5f,0.0f,0.5f,1.0f);			\
								//+max(	0.0f, -dot(	n ,light2	)	)*color*vec4(0.3f,0.5f,0.4f,1.0f)			\
								//+max(	0.0f, -dot(	n ,light3	)	)*color*vec4(0.0f,0.5f,0.5f,1.0f)			\
								//+max(	0.0f, -dot(	n ,light4	)	)*color*vec4(0.5f,0.5f,0.0f,1.0f);																																\n"
//			calculate the color of ambient light
"			vec4 ambient=color*0.1f;																																		\n"
"			Color=min(diffuse+ambient,1.0f);																																							\n"
"																																					\n"
"			nor=n;																																			\n"
"		}																																				\n"
}
	;

const GLchar* geoShader =
{
	"#version 450 core																			\n"

	//定义输入				
	"layout(triangles) in;																				\n"

	//定义输出																
	"layout(line_strip,max_vertices=10) out;	"

	//定义输入，名字为	lb_in
	"in Info																										\n"
	"{																											\n"
	"		vec4 Color;																											\n"
	"		vec2 TexCoord;																										\n"
	"}lb_in[];																											\n"

	//定义输出，名字为	lb_out
	"out Info										\n"
	"{													\n"
	"		vec4 Color;									\n"
	"		vec2 TexCoord;							\n"
	"};												\n"
	
	//调试部分
	"in vec4 nor[];																			\n"

	//////////////////////
	"void main()"
	"{"
	"	for(int i=0;i<gl_in.length();i++)						\n"		
	"	{																	\n"
	"		gl_Position=gl_in[i].gl_Position;														\n"
	"		Color=lb_in[i].Color;																								\n"									
	"		TexCoord=lb_in[i].TexCoord;																	\n"
	//提交顶点
	"		EmitVertex();																	\n"
	"	}															\n"
	//提交多边形
	"	EndPrimitive();																		\n"
	//////////////////		调试		///////////////////////////////
	"	gl_Position=vec4(0.0f,0.0f,0.0,1.0f);																		\n"
	"	Color=vec4(1.0f,0.0f,0.0f,1.0f);																		\n"
	"	EmitVertex();																		\n"
	"	gl_Position=nor[0];																\n"
	"	Color=vec4(0.0f,1.0f,0.0f,1.0f);																		\n"
	"	EmitVertex();																		\n"
	"	EndPrimitive();																		\n"
	"}																			\n"
};




const GLchar* fragShader =
{
"	#version 450 core																															\n"
"																																						\n"
"	in Info																																			\n"
"	{																																					\n"
"		vec4 Color;																																	\n"
"		vec2 TexCoord;																															\n"
"	};																																					\n"
"																																		\n"
"	out vec4 color;																																\n"
"	void main()																																	\n"
"	{																																					\n"
"																																						\n"
"		color=Color;																															\n"
"		//color=vec4(1.0f,1.0f,0.0f,1.0f);																																				\n"
"	}																																					\n"
};