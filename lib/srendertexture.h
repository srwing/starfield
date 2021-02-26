///////////////////////////////////////////////////////////////////////
// SRenderTexture.h
//
// Copyright by André Stein
// E-Mail: stone@steinsoft.net, andre_stein@web.de
// http://www.steinsoft.net
/*-----------------------------------------------
Copyright Andre Stein
E-Mail: stone@steinsoft.net, andre_stein@web.de
Internet: http://www.steinsoft.net
-----------------------------------------------

SRenderTexture Class
---------------

Description:
-----------

A very useful OpenGL wrapper class that allows you
to render an OpenGL scene/OpenGL commands to a texture
of any size. After rendering to it, you can use it like any
other texture and map it to say a cube or any other
object.

Quick Snippet:
--------------

Don't forget to add SRenderTexture.cpp to your project!!

#include "SRenderTexture.h"

SRenderTexture texture;

//create a texture with size 256x256
// use texture.create(256,256,true) to render
// alpha values too (RGBA)
texture.create(256,256);

//everything what is between beginRender()
//and finishRender() will be drawn to this texture
texture.beginRender();

glLoadIdentity();

glTranslatef(0.0f,0.0f,-5.0f);
DrawCube();

texture.finishTexture();

//texture is now a snapshot of the scene between
//begin and finishRender()

//...

//use it!
texture.select();

DrawTextureMappedCube();

*/

//////////////////////////////////////////////////////////////////////

#ifndef S_RENDER_TEXTURE_H
#define S_RENDER_TEXTURE_H

class SRenderTexture  
{
public:
	SRenderTexture();
	virtual ~SRenderTexture();
	
	void finishRender();
	void beginRender();
	void select();
	void create(int xSize, int ySize, bool alpha = false, bool depth = false);
protected:
	bool withAlpha;
	bool withDepth;
	
	unsigned int size[2];
	unsigned int viewport[4];
	unsigned int texture;
};

#endif
