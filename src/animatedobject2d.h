class  AnimatedObject2D  : public VectorizedObject
{
	public:
	
	float snapshotcoord[2] = {0.0,0.0} ;
	float snapshotsize[2] = {1.0,1.0};
	
	//
	bool ltexture = false;
	unsigned int texture;
	
	void LoadTexture(const std::string& filename,bool flip_, int desiredchannels, GLenum colorformat );
	void LoadTexture(std::string&& filename,bool flip_, int desiredchannels, GLenum colorformat  );
	
	AnimatedObject2D(int vertex_len_,int vertex_num_,int surfaces_num_,int space_dim_,int vertxsup_);
	void SetSnapshotCoords(float x, float y);
	void SetSnapshotSize(float sizex_, float sizey_);


	//RENDER
	
	void RenderTexture();

};


void AnimatedObject2D::LoadTexture(const std::string& filename,bool flip_, int desiredchannels, GLenum colorformat )
{
	

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(int(flip_));
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, desiredchannels);
	
	if (data)
	{
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, colorformat, width, height, 0, colorformat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		ltexture = true;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data); 
	
}

void AnimatedObject2D::LoadTexture(std::string&& filename,bool flip_, int desiredchannels, GLenum colorformat)
{
	LoadTexture(filename,flip_,desiredchannels,colorformat);
}

void AnimatedObject2D::SetSnapshotSize(float sizex_, float sizey_)
{
	
	if(sizex_ > 1.0 | sizex_ > 1.0 | sizey_ > 0.0 | sizey_ > 0.0) printf("Error : OpenGL Texture sizes ranges from 0.0 to 1.0");
	snapshotsize[0] = sizex_;
	snapshotsize[1] = sizey_;

}

AnimatedObject2D::AnimatedObject2D(int vertex_len_,int vertex_num_,int surfaces_num_,int space_dim_,int vertxsup_) : VectorizedObject( vertex_len_, vertex_num_, surfaces_num_, space_dim_, vertxsup_)
{};

void AnimatedObject2D::SetSnapshotCoords(float x, float y)
{
	if( x > 1.0 | x > 1.0 | y < 0.0 | y > 0.0) printf("Error : OpenGL Texture sizes ranges from 0.0 to 1.0");
}

void AnimatedObject2D::RenderTexture()
{
	glBindTexture(GL_TEXTURE_2D, this->texture);
}
