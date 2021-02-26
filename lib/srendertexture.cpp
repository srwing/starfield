///////////////////////////////////////////////////////////////////////
// SRenderTexture.cpp
//
// Copyright by André Stein
// E-Mail: stone@steinsoft.net, andre_stein@web.de
// http://www.steinsoft.net
//////////////////////////////////////////////////////////////////////

#include <GL/gl.h>
#include <GL/glu.h>
#include "srendertexture.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

SRenderTexture::SRenderTexture()
{
	size[0] = 0;
	size[1] = 0;
	withAlpha = false;
	withDepth = false;
	texture = 0;
	viewport[0] = viewport[1] = viewport[2] = viewport[3] = 0;
}

SRenderTexture::~SRenderTexture()
{
	if (texture)
		glDeleteTextures(1,&texture);
}

void SRenderTexture::create(int xSize, int ySize, bool alpha, bool depth)
{
	//delete old texture for security reasons
	if (texture)
		glDeleteTextures(1, &texture);

	//save data for further use
	size[0] = xSize;
	size[1] = ySize;
	withAlpha = alpha;
	withDepth = depth;

	//texture
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	
	//create first texture
	glTexImage2D( GL_TEXTURE_2D, 0, ( withAlpha ? GL_RGBA : 
		( withDepth ? GL_DEPTH_COMPONENT : GL_RGB ) ),
		xSize, ySize, 0,
		( withAlpha ? GL_RGBA : 
		( withDepth ? GL_DEPTH_COMPONENT : GL_RGB ) ),
		GL_UNSIGNED_BYTE, 0);

	//set parameters
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void SRenderTexture::select()
{
	//select texture for further use
	glBindTexture(GL_TEXTURE_2D,texture);
}

void SRenderTexture::beginRender()
{
	//save viewport and set new one
	//that fits to the actual texture size
	glGetIntegerv(GL_VIEWPORT,(int*)viewport);

	glViewport(0,0,size[0],size[1]);
}

void SRenderTexture::finishRender()
{
	//select texture, copy screen data to texture
	//and restore old, original viewport
	glBindTexture(GL_TEXTURE_2D,texture);

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
		size[0], size[1]);

	glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
}

