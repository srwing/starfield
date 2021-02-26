#include <stdio.h>
#include "sdlimage.h"



SDL_Surface *load_image( char *filename ) { 
//Temporary storage for the image that's loaded 
	SDL_Surface* loadedImage = NULL; 
//The optimized image that will be used 
	SDL_Surface* optimizedImage = NULL;

//Load the image 
//	loadedImage = SDL_LoadBMP( filename);
loadedImage=IMG_Load(filename);
//If nothing went wrong in loading the image 
	if( loadedImage != NULL ) { 
//Create an optimized image 
		optimizedImage = SDL_DisplayFormat( loadedImage ); 
//Free the old image 
		SDL_FreeSurface( loadedImage ); 
	}
 //Return the optimized image 
	return optimizedImage; 
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination ) { 
//Make a temporary rectangle to hold the offsets 
	SDL_Rect offset; 
//Give the offsets to the rectangle 
	offset.x = x; offset.y = y;
	offset.w=source->w; offset.h=source->h;
	SDL_Rect sourcerect;
sourcerect.x=0;
sourcerect.y=0;
sourcerect.w = source->w;
sourcerect.h = source->h;

//Blit the surface 
//	SDL_BlitSurface( source, &sourcerect, destination, &offset ); 
	SDL_BlitSurface( source, NULL, destination, &offset ); 
} 
