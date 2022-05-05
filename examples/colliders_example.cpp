

#include "turtle.h"

void render();

//Shader loading utility programs
void printProgramLog( GLuint program );
void printShaderLog( GLuint shader );


GLint gVertexPos2DLocation = -1;



int main( int argc, char* args[] )
{
	
	// Create Window
	Window_Class window = Window_Class(60,SDL_WINDOW_OPENGL| SDL_WINDOW_SHOWN ); 
	
	
	// Shader
	GPUcodes gpucodes0=GPUcodes("./shaders/shader0");	

	gpucodes0.Load("simple0","simple1", "simple");
	
	glClearColor( 0.f, 0.f, 0.f, 1.f );

	PhysicsObject2D Square(2,4,1,2,GL_QUADS), Triangle(2,3,1,2,GL_TRIANGLES);
	
	
	
	
	Triangle.SpecifyBuffersAttributes("aPos", 2) ;
	Square.SpecifyBuffersAttributes("aPos", 2) ;
	
	Square.LinkUniformToVariable("status", 2);
	Triangle.LinkUniformToVariable("status", 2);

	
	
	GenQuad(Square);
	GenTriangle(Triangle);	
	Square.SetToOrigin(0);
	Triangle.SetToOrigin(0);
	
	Triangle.Rescale(0, 5.0);
	Triangle.SetToOrigin(0);
	Square.Rescale(0, 2.0);
	
	Triangle.SameShapeCollider(0, GENERIC);
	Square.SameShapeCollider(0, GENERIC);

	
	
	Scene test = Scene();
	test.LoadObj(Square, gpucodes0.glprograms[0]);
	test.LoadObj(Triangle, gpucodes0.glprograms[0]);
	
	Square.collider->xc = &(Square.position[0]);
	Square.collider->yc = &(Square.position[1]);
	Triangle.collider->xc = &(Triangle.position[0]);
	Triangle.collider->yc = &(Triangle.position[1]);
	
		
	test.Prepare();

	const Uint8* kb = SDL_GetKeyboardState(NULL);
	int counter = 0;
	
	
	while( window.IsAlive() )
	{
		window.CycleStart();
	
		Triangle.position[0]+= 0.005*(-kb[SDL_SCANCODE_A] + kb[SDL_SCANCODE_D]) - 2*float(Triangle.position[0]>1.0) + 2*float(Triangle.position[0]<-1.0);
		Triangle.position[1] += 0.005*(-kb[SDL_SCANCODE_S] + kb[SDL_SCANCODE_W]) - 2*float(Triangle.position[1]>1.0) + 2*float(Triangle.position[1]<-1.0);
		Triangle.SetUniform("status",0,Triangle.position[0]);
		Triangle.SetUniform("status",1,Triangle.position[1]);
		Square.SetUniform("status",0,Square.position[0]);
		Square.SetUniform("status",1,Square.position[1]);
		
		Square.collider->xc = &(Square.position[0]);
		Square.collider->yc = &(Square.position[1]);
		Triangle.collider->xc = &(Triangle.position[0]);
		Triangle.collider->yc = &(Triangle.position[1]);
	
		test.Update();
		test.Prepare();
	
		window.CycleEnd();
	}
	

	return 0;
}
