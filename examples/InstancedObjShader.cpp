

#include "turtle.h"

void render();

//Shader loading utility programs
void printProgramLog( GLuint program );
void printShaderLog( GLuint shader );


GLint gVertexPos2DLocation = -1;



int main( int argc, char* args[] )
{
	
	// Create Window
	Window_Class window = Window_Class(60,SDL_WINDOW_OPENGL| SDL_WINDOW_SHOWN, "Name", 620,620 ); 
	window.SetMainWindow();
	
	// Shader
	//GPUcodes gpucodes0=GPUcodes(&window,"./shaders/vectorizedobject.shader");	
	GPUcodes gpucodes0=GPUcodes(&window,"./shaders/instanced.shader");	

	//gpucodes0.Load("vecv","vecf", "PositionRotationColor");
	gpucodes0.Load("instancedv","instancedf", "shader");
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	
	// A polygon will be made of triangles
	// As many as the sides
	VectorizedObject Pentagon(2,5+1,5,2,GL_TRIANGLES);
	
	GenPolygon(&Pentagon,5,0.1);
	//GenPolygon(&Pentagon,3,1.0);
	
	Pentagon.SpecifyBuffersAttributes("vposition", 2);
	//Pentagon.SpecifyBuffersAttributes("c_", 4);
	
	
	Pentagon.SetToOrigin(0);
	
	
	//Pentagon.LinkUniformToVariable("CM", 4);
	
	Scene test = Scene(&window);
	test.LoadObj(Pentagon, gpucodes0.glprograms[0]);
	
		
	test.Prepare();

	const Uint8* kb = SDL_GetKeyboardState(NULL);
	
	
	Pentagon.position[0] = 0.0;
	Pentagon.position[1] = 0.0;
	Pentagon.angle       = 0.0;
	
	
	
	//window.CycleEnd();
	while( window.IsAlive() )
	{
		window.CycleStart();
	
		
		Pentagon.angle+=0.001;
		
		Pentagon.position[0]+= 0.02*(-kb[SDL_SCANCODE_A] + kb[SDL_SCANCODE_D]) - 2*float(Pentagon.position[0]>1.0) + 2*float(Pentagon.position[0]<-1.0);
		Pentagon.position[1] += 0.02*(-kb[SDL_SCANCODE_S] + kb[SDL_SCANCODE_W]) - 2*float(Pentagon.position[1]>1.0) + 2*float(Pentagon.position[1]<-1.0);

	
		
		//dbglog(Pentagon.angle);
		/*Pentagon.SetUniform("CM",0,Pentagon.position[0]);
		Pentagon.SetUniform("CM",1,Pentagon.position[1]);
		Pentagon.SetUniform("CM",2,Pentagon.angle);
		Pentagon.SetUniform("CM",3,0.0);*/
		
		
		test.Prepare();
		test.Update();
		window.CycleEnd();
	}
	

	return 0;
}