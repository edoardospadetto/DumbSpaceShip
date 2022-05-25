#include "../turtle.h" 



/*
* Dummy constructor, it performs just a call to OpenGL
*/

Scene::Scene(Window_Class* parent_): parent(parent_) 
{

        collidersdebug = new GPUcodes(parent , "./data/debugutils/colliders_debug.gls");
	glGenBuffers( 1, &VBO );
	glGenBuffers( 1, &IBO );
	glGenBuffers( 1, &TBO );
}



/*
* PURPOSE
*
* Load an object into the scene and specify which program (OpenGL meaning) to use
* when drawing
*
* DESCRIPTION
*
* The function verify if it has already processed the program before
* if yes it queues the object with all the others assigned to the program
* otherwise it start a new queue for the requested program
*
* Then it checks into the uniforms declared for the object if they are found
* in the program.
*
* Info on the lenght of the buffers are obtained from the object
*
* Checks are performed on the names and location of the attributes of the object
*/




void Scene::LoadObj(VectorizedObject& obj, GLuint designatedprogram)
{

// ================================================================
//		            LOAD GRAPHICS
// ================================================================

	bool lfound = false;
	
	dbglog("designated program=  ", designatedprogram );
	
	for(int i=0; i<programs.size();i++)
	{
		if(programs[i] == designatedprogram)
		{
			assets[i].push_back(&obj);
			dbglog("	the requested program : ", programs[i], " has a queue, adding object");
			lfound = true;
			obj.sceneprog = i ;
			obj.sceneprogidx = assets[i].size()-1; 
			break;
		}
	}
	
	if(!lfound)
	{
		programs.push_back(designatedprogram);
		std::vector<VectorizedObject*> tmp{&obj};
		assets.push_back(tmp);
		obj.sceneprog = assets.size()-1 ;
		obj.sceneprogidx = 0; 
		dbglog("	the requested program : ", designatedprogram, " has no queue, creating one");
	}
	
	for(int i=0; i<obj.uniformnames.size(); i++)
	{
		GLint tmp = glGetUniformLocation(designatedprogram, obj.uniformnames[i].c_str());
		GLenum error = glGetError();
		if(error!=GL_NO_ERROR | tmp==-1) printf("ERROR: uniform %s \n", gl_error_string(error));
		obj.uniformlocationsprogram[i] = tmp;
		dbglog("	link uniform : ", obj.uniformnames[i], "at location " , tmp) ;
	}
	
	uint tmpvbo, tmpibo, vtlen;
	obj.GetBuffersInfo(tmpvbo, tmpibo, vtlen);
	
	vertexbuffersize += tmpvbo*vtlen;
	indexbuffersize +=tmpibo;
	
	for (int i =0; i<obj.attributenames.size(); i++)
	{
		obj.attributelocationsprogram[i] = glGetAttribLocation( designatedprogram, obj.attributenames[i].c_str() );	
		GLenum error = glGetError();
		if(error!=GL_NO_ERROR | obj.attributelocationsprogram[i] == -1) printf("ERROR: attribute %s \n", gl_error_string(error));
		dbglog("	linkbuffer attribute = ",obj.attributenames[i].c_str(), "location =", obj.attributelocationsprogram[i])   ;
		
	}
	
	InstancedObject *tmpInstanced = dynamic_cast<InstancedObject*>(&obj);
	if(tmpInstanced!=nullptr)
	{
		uint tmptbo, instancesize;
		for (int i =0; i<tmpInstanced->instanceattributenames.size(); i++)
		{
		
			tmpInstanced->instanceattributelocationsprogram[i] = 
							glGetAttribLocation( designatedprogram, tmpInstanced->instanceattributenames[i].c_str() );	
			tmpInstanced->InstancedBufferInfo(tmptbo, instancesize );
			instancebuffersize += tmptbo*instancesize;
			
			GLenum error = glGetError();
			if(error!=GL_NO_ERROR | tmpInstanced->instanceattributelocationsprogram[i] == -1)
			{
				printf("ERROR: instance attribute %s \n", gl_error_string(error));
			}
			
			dbglog("	linkbuffer instance attribute = ",
					tmpInstanced->instanceattributenames[i].c_str(), 
					"location =", 
					tmpInstanced->instanceattributelocationsprogram[i])   ;
		}
		
	}
	
	GLenum error = glGetError();
	if(error!=GL_NO_ERROR) 
	{
		printf("ERROR: Load Object: %s \n", gl_error_string(error));
		throw std::exception();
	}
	
	
// ================================================================
//		            LOAD COLLISIONS
// ================================================================

	sceneCollisionEngine.LoadCollidingObject(dynamic_cast<ColliderObject2D*>(&obj)); 
			

	
}



/*
* PURPOSE
*
* Render all the object stored in the scene
*
* DESCRIPTION
* 
* Render each asset in the scene according to asset render method
* using the right program
*/


void Scene::Collisions()
{
	sceneCollisionEngine.StartCollisions();
	sceneCollisionEngine.VerifyCollisions();
	sceneCollisionEngine.HandleCollisions();
	sceneCollisionEngine.EndCollisions();
	
	if(ldbgcolliders) sceneCollisionEngine.UpdateDbgColliders();	
}



void Scene::Render()
{
	
		
		GLenum error = glGetError();
		GLuint offsetibo=0 , offsetvbo=0, offsettbo=0,  tmpvbo=0, tmpibo=0, tmptbo=0, vertxlen=0;
		
		for(int i=0; i< assets.size(); i++)
		{
			
			glUseProgram( programs[i] );
			glBindBuffer( GL_ARRAY_BUFFER, VBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, TBO );
			//Enable vertex position
			for (int j =0; j< assets[i].size(); j++)
			{
			
				InstancedObject* tmpInstanced=dynamic_cast<InstancedObject*>(assets[i][j]);
				if( tmpInstanced != nullptr)
				{
					tmpInstanced->Render(VBO,IBO,TBO,offsetvbo,offsetibo,offsettbo);
				}
				else
				{
					 assets[i][j]->Render(VBO,IBO,offsetvbo,offsetibo);
				}
				
			
			}
				
		}
		error = glGetError();
		if(error!=GL_NO_ERROR) 
		{
			printf("ERROR: Scene::Render ");
			printf("%s \n" , gl_error_string(error));
			throw std::exception();
		}
		
		
		//DEBUG COLLIDERS
		
		
}
/*
* PURPOSE
*
* Handles collisions among physical objects
* 
* DESCRIPTION
* 
* 
*
*/


/*
* PURPOSE
*
* After Loading object in the scene all buffers to gpu have to be loaded
* 
* DESCRIPTION
* 
* Render each asset in the scene according to asset render method
* This is done ordering assets in the gpu buffers by program and then by appearence
*
*/
void Scene::Update()
{
	//Update Animation System
	this->Animations();
	this->Physics();
	this->Collisions();
	
	

	//Render
	this->Render();
}


void Scene::Physics()
{
	/*
	for (auto & objset : assets )
	{
		for (auto & obj : objset )
		{
			obj->velocity[0] = obj->position[0]- obj->lastPosition[0];
			obj->velocity[1] = obj->position[1]- obj->lastPosition[1];
			obj->lastPosition[0] = obj->position[0];
			obj->lastPosition[1] = obj->position[1];
		}
	}
	*/
}



void Scene::Animations()
{
	
	for (auto & objset : assets )
	{
		for (auto & obj : objset )
		{
			AnimatedObject2D* temp_obj{ dynamic_cast<AnimatedObject2D*>(obj) }; 
			
			if(temp_obj != nullptr) // Go on with animation temp_obj->Animate();
			{
				//Triggers not processed by SDL.
				//wenttimeout : If the state just ended and restarted. 
				bool wentTimeOut=false; 
				temp_obj->selfStateEngine->AnimateState(wentTimeOut); // 1st 
				temp_obj->selfStateEngine->ChangeState(wentTimeOut);  // 2nd
				temp_obj->selfStateEngine->UpdateVBatFrame(temp_obj); // 3rd
			}
		} 
	} 
}

void Scene::Prepare()
{
	
	//dbglog("SUMMARY");
	GLenum error = glGetError();
	


	
	for(int i=0; i< assets.size(); i++)
	{
		//dbglog("program", programs[i], "#assets", assets[i].size());
	}
	
	
	//Create VBO
	
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, vertexbuffersize*sizeof(GLfloat), NULL , GL_DYNAMIC_DRAW );
	
	//Create IBO
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexbuffersize*sizeof(GLuint), NULL  , GL_DYNAMIC_DRAW );
	
	//Create TBO
	glBindBuffer( GL_ARRAY_BUFFER, TBO );
	glBufferData( GL_ARRAY_BUFFER, instancebuffersize*sizeof(GLfloat), NULL , GL_DYNAMIC_DRAW );
	
	error = glGetError();
	if(error!=GL_NO_ERROR) 
	{
		printf("ERROR: Scene Prepare, Buffers Creation | %s | v.b.size %d |i.b.size %d |\n", gl_error_string(error), vertexbuffersize, indexbuffersize);
		throw std::exception();
	}
	//dbglog("");
	//dbglog(" Indexbuffersize (NUM_SURF*VERT_X_SURF) =", indexbuffersize);
	//dbglog(" Vertexbuffersize (VERT_COUNT*VERT_SIZE) =", vertexbuffersize);
	//dbglog("");

	uint tmpvbo=0, tmpibo=0, vertxlen=0;
	uint offsetvbo=0, offsetibo=0, offsetvbover=0;
	uint offsettbo=0, tmptbo=0, tbolen=0;

	for(int i=0; i< assets.size(); i++)
	{
	for(int j=0; j< assets[i].size(); j++)
	{
		assets[i][j]->GetBuffersInfo(tmpvbo, tmpibo, vertxlen);	
		//dbglog("Sizes: VBO =" ,tmpvbo, "| IBO  = ",tmpibo, "| VLEN  = ", vertxlen);
		glBindBuffer( GL_ARRAY_BUFFER, VBO );
		glBufferSubData( GL_ARRAY_BUFFER, offsetvbo*sizeof(GLfloat), tmpvbo*vertxlen*sizeof(GLfloat), assets[i][j]->vertex_buffer );
		
		error = glGetError();
		
		if(error!=GL_NO_ERROR) 
		{
			printf("ERROR: Scene Prepare, Vertex Buffer Filling %s asset program %d  index %d\n", gl_error_string(error), i, j);
			throw std::exception();
		}
		

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
		glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, offsetibo*sizeof(GLuint), tmpibo*sizeof(GLuint), assets[i][j]->index_buffer );

		
		offsetvbo+=tmpvbo*vertxlen;
		offsetibo+=tmpibo;
		offsetvbover+=tmpvbo;
		
		error = glGetError();
		
		if(error!=GL_NO_ERROR) 
		{
			printf("ERROR: Scene::Prepare-IBO filling: %s | AssetsProgram %d | AssetsProgramIndex %d\n", gl_error_string(error), i, j);
			throw std::exception();
		}
		
		InstancedObject* instancedTmp = dynamic_cast<InstancedObject*>(assets[i][j]);
		if(instancedTmp != nullptr)
		{
			instancedTmp->InstancedBufferInfo(tmptbo, tbolen);
			glBindBuffer( GL_ARRAY_BUFFER, TBO );
			glCheckError();
			glBufferSubData( GL_ARRAY_BUFFER, 
					  offsettbo*sizeof(GLfloat), 
					  tmptbo*tbolen*sizeof(GLfloat), 
					  instancedTmp->instance_buffer );
					  
		
			error = glGetError();
		
			if(error!=GL_NO_ERROR) 
			{
				printf("ERROR: Scene::Prepare-TBO filling: %s | AssetsProgram %d | AssetsProgramIndex %d\n",
							 gl_error_string(error), i, j);
				throw std::exception();
			}
		
		}
	
		
		
		
	}
	}
	

	//dbglog("Buffers Filled ");
	
	
	
}



void Scene::UnloadObject(VectorizedObject& obj)
{	
	
	assets[obj.sceneprog].erase(assets[obj.sceneprog].begin()+obj.sceneprogidx);
	
	uint tmpvbo, tmpibo, vtlen;
	obj.GetBuffersInfo(tmpvbo, tmpibo, vtlen);
	vertexbuffersize -= tmpvbo*vtlen;
	indexbuffersize -=tmpibo;
	if(assets[obj.sceneprog].size() == 0)
	{
		assets.erase(assets.begin()+obj.sceneprog);
		dbglog("test9");
		for (int i =obj.sceneprog; i < assets.size(); i++ )
		{
			for (int j =0; j < assets[i].size(); j++ )
			{
				assets[i][j]->sceneprog = i ;
			}
		}
	}
	for (int j =obj.sceneprogidx; j < assets[obj.sceneprog].size(); j++ )
	{
		assets[obj.sceneprog][j]->sceneprogidx = j ;
	}

	for(int i=0; i<assets.size(); i++)
	{

		for (int j =0; j < assets[i].size(); j++ )
	{
	
	}


	}
		
	//TEMP
	Prepare();
	
	//TO DO! 
	/*Get position of the removed object* =LOC 
	**Copy buffer[0:LOC] to newbuffer[0:LOC]
	**Copy buffer[LOC+SIZE: buffersize] to newbuffer[LOC+1: buffersize-SIZE]
	**Use the new buffer instead of the old one, both IBO and VBO*/

}

void Scene::DebugColliders()
{

	collidersdebug->Load("debug_colliders_vertex","debug_colliders_fragment", "debug_colliders");
	
	for (auto obj : sceneCollisionEngine.collisionSet )
	{
		obj->collider->BuildVecObj();
		this->LoadObj( *(obj->collider->colliderRep), collidersdebug->glprograms[0]);
	}
	ldbgcolliders=true;
}


Scene::~Scene()
{
	delete collidersdebug;
}


