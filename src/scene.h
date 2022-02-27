class Scene
{
	GLuint VBO = 0;
	GLuint IBO = 0;
	uint vertexbuffersize=0, indexbuffersize=0;
	

	std::vector<std::vector<VectorizedObject*>> assets;
	std::vector<GLuint> programs;
	public:
	Scene();
	 
	void Render();
	void LoadObj(VectorizedObject& obj, GLuint designatedprogram);
	void Prepare();
	void ProgramUniforms(VectorizedObject* obj);
	void UnloadObject(VectorizedObject& obj);
};

Scene::Scene()
{
	glGenBuffers( 1, &VBO );
	glGenBuffers( 1, &IBO );
}
void Scene::LoadObj(VectorizedObject& obj, GLuint designatedprogram)
{
	bool lfound = false;
	//Set right program to render
	dbglog("designated program=  ", designatedprogram );
	for(int i=0; i<programs.size();i++)
	{
		if(programs[i] == designatedprogram)
		{
			assets[i].push_back(&obj);
			dbglog("	FOUND! ", programs[i], designatedprogram);
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
		dbglog("	NEW!");
	}
	//Set uniform
	for(int i=0; i<obj.uniformnames.size(); i++)
	{
		GLint tmp = glGetUniformLocation(designatedprogram, obj.uniformnames[i].c_str());
		GLenum error = glGetError();
		if(error!=GL_NO_ERROR | tmp==-1) printf("ERROR: uniform %s \n", gl_error_string(error));
		obj.uniformlocationsprogram[i] = tmp;
		dbglog("	link uniform = ",obj.uniformnames[i], tmp) ;
	}
	//Set VBO IBO
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
	
}

void Scene::Render()
{
	
		//glClear( GL_COLOR_BUFFER_BIT );
		GLenum error = glGetError();
		GLuint offsetibo=0 , offsetvbo=0, tmpvbo=0, tmpibo=0, vertxlen=0;
		
		//if(error!=GL_NO_ERROR) printf("ERROR: render %s \n", gl_error_string(error));
	
		for(int i=0; i< assets.size(); i++)
		{
			
		glUseProgram( programs[i] );
		glBindBuffer( GL_ARRAY_BUFFER, VBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
		
		//Enable vertex position
		for (int j =0; j< assets[i].size(); j++)
		{
		
			assets[i][j]->Render(VBO,IBO,offsetvbo,offsetibo);
		}
		//dbglog("");
		
			
		}
	
		//if(error!=GL_NO_ERROR) printf("ERROR: render %s \n", gl_error_string(error));

		
		

		//Set index data and render

		

		//Disable vertex position
		


	

}

void Scene::Prepare()
{
	dbglog("SUMMARY");
	for(int i=0; i< assets.size(); i++)
	{
		dbglog("program", programs[i], "#assets", assets[i].size());
	}
	
	//Create VBO
	
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, vertexbuffersize*sizeof(GLfloat), NULL , GL_DYNAMIC_DRAW );
	//Create IBO
	
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexbuffersize*sizeof(GLuint), NULL  , GL_DYNAMIC_DRAW );
	GLenum error = glGetError();
	if(error!=GL_NO_ERROR) printf("ERROR: Scene Prepare, Buffers Creation %s vbs %d ibs %d \n", gl_error_string(error), vertexbuffersize, indexbuffersize);
	
	dbglog("");
	dbglog(" Indexbuffersize (NUM_SURF*VERT_X_SURF) =", indexbuffersize);
	dbglog(" Vertexbuffersize (VERT_COUNT*VERT_SIZE) =", vertexbuffersize);
	dbglog("");

	uint tmpvbo=0, tmpibo=0, vertxlen=0;
	uint offsetvbo=0, offsetibo=0, offsetvbover=0;

	for(int i=0; i< assets.size(); i++)
	{
	for(int j=0; j< assets[i].size(); j++)
	{
		assets[i][j]->GetBuffersInfo(tmpvbo, tmpibo, vertxlen);	
		dbglog("Sizes: VBO =" ,tmpvbo, "| IBO  = ",tmpibo, "| VLEN  = ", vertxlen);
		glBufferSubData( GL_ARRAY_BUFFER, offsetvbo*sizeof(GLfloat), tmpvbo*vertxlen*sizeof(GLfloat), assets[i][j]->vertex_buffer );
		
		error = glGetError();
		if(error!=GL_NO_ERROR) printf("ERROR: Scene Prepare, Vertex Buffer Filling %s asset program %d  index %d\n", gl_error_string(error), i, j);
		printf("%d, %d, -> %d \n", offsetvbo,  tmpvbo*vertxlen, vertexbuffersize );

		//for(auto x = assets[i][j]->index_buffer; x != &assets[i][j]->index_buffer[assets[i][j]->surfaces_num*assets[i][j]->vertexxsurf]; ++x) {*x+=offsetvbover;}
		glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, offsetibo*sizeof(GLuint), tmpibo*sizeof(GLuint), assets[i][j]->index_buffer );
		//for(auto x = assets[i][j]->index_buffer; x != &assets[i][j]->index_buffer[assets[i][j]->surfaces_num*assets[i][j]->vertexxsurf]; ++x) {*x-=offsetvbover;}
		
		offsetvbo+=tmpvbo*vertxlen;
		offsetibo+=tmpibo;
		offsetvbover+=tmpvbo;
	
		error = glGetError();
		if(error!=GL_NO_ERROR) printf("ERROR: Scene::Prepare-IBO filling: %s | AssetsProgram %d | AssetsProgramIndex %d\n", gl_error_string(error), i, j);
	}
	}
	

	dbglog("Buffers Filled ");
}



void Scene::UnloadObject(VectorizedObject& obj)
{	
	dbglog("test A1", assets[obj.sceneprog].size());
	assets[obj.sceneprog].erase(assets[obj.sceneprog].begin()+obj.sceneprogidx);
	dbglog("test A2", assets[obj.sceneprog].size());
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
	printf("%d  -- ", i);
		for (int j =0; j < assets[i].size(); j++ )
	{
		printf(" %d ", j);
	}
	printf("\n");

	}
		
	//TEMP
	Prepare();

}
