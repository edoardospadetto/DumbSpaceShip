
//VectorizedObject

/* 
* This is the most basic type of object appearing on screen.
* 
* Buffers
* std::vector<std::string>  buffernames
* Names given to each buffer,
*
*/



class VectorizedObject
{

	void GetBuffersInfo(uint& sizevbo,uint& sizeibo);
	void RenderProgramUniforms();
		
	public:

	// Physics
	float                          angle = 0.0;
	float*				position = NULL; 
	float*				velocity = NULL; 
	float*                         lastPosition=NULL;
	
	//Buffers 
	float*				vertex_buffer = NULL;
	int* 				index_buffer = NULL;
	
	std::vector<std::string>	buffernames = {};
	std::vector<int> 		buffersizes = {};
	std::vector<int> 		bufferformat = {};
	
	// Uniforms
	std::vector<float> 		uniformattributes = {}; 
	std::vector<std::string> 	uniformnames ={};
	std::vector<int> 		uniformlocationsprogram ={};
	std::vector<int> 		uniformsizes ={};
	
	//IBO & VBO vars
	std::vector<std::string> attributenames={}; 
	std::vector<int> attributelocationsprogram={};
	GLenum representation = -1;
	std::vector<int> attributesizes={};
	
	//Geometry Info
	int vertex_len = 0;
	int vertex_num = 0;
	int surfaces_num = 0;
	int space_dim = 0; 
	
	//Temp
	int sceneprog=0; 
	int sceneprogidx =0;

	void Render(GLuint VBO, GLuint IBO, GLuint& offsetvbo, GLuint& offsetibo);


	int vertexxsurf = 0;
	
	
	VectorizedObject(int vertex_len_,int vertex_num_,int surfaces_num_,int space_dim_,GLenum representation_);
	~VectorizedObject();
	
	
	void GetBuffersInfo(uint& sizevbo,uint& sizeibo, uint& vtlen);


	void SetToOrigin(int vblocation);
	void Translate(int vblocation,float* val);
	void Rescale(int vblocation, float factor);

	
	int LinkUniformToVariable(std::string&& uniformname, int uniformsize );
	int LinkUniformToVariable(const std::string& uniformname, int uniformsize);
	
	int SpecifyBuffersAttributes(std::string&& name, int size);
	int SpecifyBuffersAttributes(const std::string& name, int size);

	int SetUniform(const std::string& uniformname,int idx, float value);
	int SetUniform(std::string&& uniformname,int idx, float value);
	void SetUniform(int uniformidx,int idx, float value);
	
	virtual void RenderTexture() {};
	virtual void UnbindTexture() {};

};



VectorizedObject::VectorizedObject(int vertex_len_,int vertex_num_, int surfaces_num_, int space_dim_,GLenum representation_):
vertex_len( vertex_len_), vertex_num(vertex_num_),  surfaces_num(surfaces_num_), space_dim(space_dim_), representation(representation_)
{

	switch(representation_)
	{
		case GL_LINES:	
	 		vertexxsurf = 2;
	 		break;
	 	case GL_TRIANGLES:
	 		vertexxsurf = 3;
	 		break;
	 	case GL_QUADS:
	 		vertexxsurf = 4;
	 		break;
	}
	position      = new float[space_dim];
	velocity      = new float[space_dim];
	lastPosition  = new float[space_dim];
	
	vertex_buffer = new float[vertex_len_*vertex_num_];
	index_buffer  = new int[surfaces_num*vertexxsurf];
	
	uniformsizes.push_back(0);
	attributesizes.push_back(0);
}

void VectorizedObject::GetBuffersInfo(uint &VBOsize , uint& IBOsize, uint& vertexlen_ )
{
	VBOsize    = vertex_num;
	IBOsize    = vertexxsurf*surfaces_num;
	vertexlen_ =vertex_len;	
}

void VectorizedObject::GetBuffersInfo(uint &VBOsize , uint& IBOsize )
{
	VBOsize = vertex_num;
	IBOsize= vertexxsurf*surfaces_num;
}

// Routine to rescale object, suggested to use after resetting to origin
void  VectorizedObject::Rescale(int vblocation,float factor)
{
for (int i=0; i<vertex_num; i++){for(int j=0; j<space_dim; j++)
{
		vertex_buffer[ i*vertex_len + vblocation +j ] *= factor; 
}}


}

// Set center of mass of object to center of coordinate system
// vb location is the offster of the coordinates on the buffer.
// coordinates are supposed subsequential.
void VectorizedObject::SetToOrigin(int vblocation)
{
	if(vertex_buffer == nullptr) printf("ERROR: nullptr vertex buffer\n");
	
	for (int i=0; i<space_dim; i++){position[i] = 0.0;} 
	
	for (int i=0; i<vertex_num; i++){for(int j=0; j<space_dim; j++)
	{
			position[j] += vertex_buffer[ i*vertex_len + vblocation +j ];
	}}	
	
	for (int i=0; i<space_dim; i++) {position[i] = position[i] / float(vertex_len);}
	
	
	for (int i=0; i<vertex_num; i++){for(int j=0; j<space_dim; j++)
	{
			vertex_buffer[ i*vertex_len + vblocation +j ] -= position[j]; 
	}}

}

void VectorizedObject::Translate(int vblocation,float* val)
{
	if(vertex_buffer == nullptr) printf("ERROR: nullptr vertex buffer\n");

	
	for (int i=0; i<vertex_num; i++){for(int j=0; j<space_dim; j++)
	{
			vertex_buffer[ i*vertex_len + vblocation +j ] += val[j]; 
	}}

}

VectorizedObject::~VectorizedObject()
{

	delete [] index_buffer;
	delete [] vertex_buffer;
	if ( position != nullptr ) delete[] position; 
	if ( velocity != nullptr ) delete[] velocity; 
	if ( velocity != nullptr ) delete[] lastPosition; 


}
int VectorizedObject::LinkUniformToVariable(std::string&& uniformname, int uniformsize )
{
	return LinkUniformToVariable( uniformname, uniformsize );
}

int VectorizedObject::LinkUniformToVariable(const std::string& uniformname, int uniformsize )
{
	
	for(int i=0; i<uniformsize; i++){ uniformattributes.push_back(0.0);}	
	uniformnames.push_back(uniformname);
	uniformlocationsprogram.push_back(-1);
	dbglog(uniformsizes[uniformsizes.size()-1], "test");
	uniformsizes.push_back(uniformsize+uniformsizes[uniformsizes.size()-1]);
	return(uniformnames.size()-1);

}
int VectorizedObject::SetUniform(const std::string& uniformname,int idx, float value)
{
	
	for (int i=0; i<uniformnames.size(); i++)
	{
		
		if(uniformname == uniformnames[i]){
			 if(idx<uniformsizes[i+1]-uniformsizes[i] & idx >= 0){uniformattributes[uniformsizes[i]+idx] = value;	return(i);}
			 else{printf("ERROR, invalid index for the requested uniform %d %s \n", uniformsizes[i+1]-uniformsizes[i], uniformnames[i].c_str());}
			
		}
	}
	printf("ERROR, uniform not found\n");
	return(-1);
}







int VectorizedObject::SetUniform(std::string&& uniformname,int idx, float value) {return SetUniform(uniformname,idx,value);}
void VectorizedObject::SetUniform(int uniformidx,int idx, float value)
{
	uniformattributes[uniformsizes[uniformidx]+idx] = value;
}







int VectorizedObject::SpecifyBuffersAttributes(std::string&& name, int size){return SpecifyBuffersAttributes(name, size);}





int VectorizedObject::SpecifyBuffersAttributes(const std::string& name, int attributesize)
{
	attributenames.push_back(name);
	attributesizes.push_back(attributesizes[attributesizes.size()-1]+attributesize);
	attributelocationsprogram.push_back(-1);

	return(attributenames.size()-1);
}


void VectorizedObject::Render(GLuint VBO, GLuint IBO, GLuint& offsetvbo, GLuint& offsetibo)
{
	
		
	
		uint tmpibo = 0, tmpvbo = 0; 
		this->RenderProgramUniforms();
		this->GetBuffersInfo(tmpvbo, tmpibo);	
		
		

		for(int k=0; k<this->attributenames.size(); k++ )
		{
			
			glVertexAttribPointer(  this->attributelocationsprogram[k], 
						(this->attributesizes[k+1] - this->attributesizes[k]), 
						GL_FLOAT, 
						GL_FALSE, 
						(this->attributesizes[this->attributesizes.size()-1])*sizeof(GLfloat), 
						(void*) ((offsetvbo+this->attributesizes[k])*sizeof(GLfloat)) );
						glEnableVertexAttribArray(this->attributelocationsprogram[k]);
							
		

		}	
		
		
		
			
	  	this->RenderTexture();
			
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
		glDrawElements( this->representation , 
				this->surfaces_num*this->vertexxsurf, 
				GL_UNSIGNED_INT, 
				(void*) (offsetibo*sizeof(GLuint)) );
			
		int nbuffersize, vbsi; 
		this->UnbindTexture();
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &nbuffersize);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vbsi);	

		offsetibo +=this->surfaces_num*this->vertexxsurf;
		offsetvbo += tmpvbo*this->vertex_len;
				
		for(int k=0; k<this->attributenames.size(); k++ ){glDisableVertexAttribArray( this->attributelocationsprogram[k]);}	
		

}

void VectorizedObject::RenderProgramUniforms()
{

	for(int k=0; k< this->uniformnames.size(); k++)
	{
		switch(this->uniformsizes[k+1]-this->uniformsizes[k])
		{
			case 2: 
				glUniform2f(this->uniformlocationsprogram[k],
				this->uniformattributes[this->uniformsizes[k]],
				this->uniformattributes[this->uniformsizes[k]+1]);
			case 4: 
				glUniform4f(this->uniformlocationsprogram[k],
				this->uniformattributes[this->uniformsizes[k]],
				this->uniformattributes[this->uniformsizes[k]+1],
				this->uniformattributes[this->uniformsizes[k]+2],
				this->uniformattributes[this->uniformsizes[k]+3]);
				//dbglog("unif" , obj->uniformattributes[obj->uniformsizes[k]],obj->uniformattributes[obj->uniformsizes[k]+1 ]);
		}
	}


}

