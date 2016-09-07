#include "RosalilaGraphics.h"

void RosalilaGraphics::init()
{
    this->shake_magnitude = 0;
    this->shake_time = 0;
    this->shake_original_x = 0;
    this->shake_original_y = 0;
    this->current_screen_shake_x = 0;
    this->current_screen_shake_y = 0;
    this->current_grayscale_percentage = 1.0;
    this->target_grayscale_percentage = this->current_grayscale_percentage;
    this->grayscale_change_speed = 0;

    //XML Initializations
    Node* root_node = Rosalila()->Parser->getNodes(assets_directory+"config.xml");

    Node* resolution_node = root_node->getNodeByName("Resolution");
    screen_width=atoi(resolution_node->attributes["x"].c_str());
    screen_height=atoi(resolution_node->attributes["y"].c_str());

    Node* screen_size_node = root_node->getNodeByName("ScreenSize");
    int screen_resized_width=atoi(screen_size_node->attributes["x"].c_str());
    int screen_resized_height=atoi(screen_size_node->attributes["y"].c_str());

    Node* fullscreen_node = root_node->getNodeByName("Fullscreen");
    fullscreen=fullscreen_node->attributes["enabled"]=="yes";

    Node* font_node = root_node->getNodeByName("Font");

    int font_size=10;
    if(font_node->hasAttribute("size"))
    {
        font_size=atoi(font_node->attributes["size"].c_str());
    }
    int font_red=0;
    if(font_node->hasAttribute("red"))
    {
        font_red=atoi(font_node->attributes["red"].c_str());
    }
    int font_green=0;
    if(font_node->hasAttribute("green"))
    {
        font_green=atoi(font_node->attributes["green"].c_str());
    }
    int font_blue=0;
    if(font_node->hasAttribute("blue"))
    {
        font_blue=atoi(font_node->attributes["blue"].c_str());
    }
    //Internal initializations
    joystick_1 = NULL;
    joystick_2 = NULL;

    screen_bpp = 32;
    camera_x=camera_y=0;

    //Initialize all SDL subsystems
    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 )
    {
        //Rosalila()->Utility->writeLogLine(SDL_GetError());
        return;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        //Rosalila()->Utility->writeLogLine(SDL_GetError());
        return;
    }

    font = NULL;
    textColor.r = font_red;
    textColor.g = font_green;
    textColor.b = font_blue;

    string font_path=assets_directory+"misc/font.ttf";
    font = TTF_OpenFont( font_path.c_str(), font_size );

    if(font==NULL)
    {
        //Rosalila()->Utility->writeLogLine("Could not init font. Place it on /misc/font.ttf .");
    }

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ); // *new*

    window = SDL_CreateWindow( "Rosalila Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               screen_resized_width, screen_resized_height,
                               SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
    if(!window)
    {
        //Rosalila()->Utility->writeLogLine("Could not init window");
    }

    if(fullscreen)
        SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);

    renderer = SDL_CreateRenderer(window, -1, 0);

    if(!renderer)
    {
        //Rosalila()->Utility->writeLogLine("Could not init render");
    }

    //Set the openGL state?
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    glViewport( 0, 0, screen_resized_width, screen_resized_height );

    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho(0.0f, screen_width, screen_height, 0.0f, -1.0f, 1.0f);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Fps cap
    frames_per_seccond = 60;
    frame = 0;
    last_tick=SDL_GetTicks();

    //Init joysticks
    if( SDL_NumJoysticks() == 1 )
    {
//        Rosalila()->Utility->writeLogLine("1 joystick was found.");
        joystick_1 = SDL_JoystickOpen( 0 );
//        if(joystick_1 != NULL)
//            Rosalila()->Utility->writeLogLine("Joystick for player 1 initialized succesfully.");
//        else
//            Rosalila()->Utility->writeLogLine("Error initializing joystick for player 1.");
    }
    if( SDL_NumJoysticks() == 2 )
    {
//        Rosalila()->Utility->writeLogLine("2 joysticks were found.");
        joystick_1 = SDL_JoystickOpen( 0 );
//        if(joystick_1 != NULL)
//            Rosalila()->Utility->writeLogLine("Joystick for player 1 initialized succesfully.");
//        else
//            Rosalila()->Utility->writeLogLine("Error initializing joystick for player 1.");
        joystick_2 = SDL_JoystickOpen( 1 );
//        if(joystick_2 != NULL)
//            Rosalila()->Utility->writeLogLine("Joystick for player 2 initialized succesfully.");
//        else
//            Rosalila()->Utility->writeLogLine("Error initializing joystick for player 2.");
    }

    //If everything initialized fine
//    Rosalila()->Utility->writeLogLine("Success! SDL initialized.");

     SDL_GL_CreateContext(window);
    GLenum error = GL_NO_ERROR;
    error = glGetError();
    if( error != GL_NO_ERROR ) {
/* Died as glu did
         printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
*/
         exit(12);
    }

    return;
}

RosalilaGraphics::~RosalilaGraphics()
{
    //Quit SDL
    SDL_Quit();
}


Image* RosalilaGraphics::getTexture(std::string filename)
{
    SDL_Surface *surface;
    GLenum texture_format;
    GLint  nOfColors;
    GLuint texture;

    if ( (surface = IMG_Load(filename.c_str())) ) {

            // get the number of channels in the SDL surface
            nOfColors = surface->format->BytesPerPixel;
            if (nOfColors == 4)     // contains an alpha channel
            {
                    if (surface->format->Rmask == 0x000000ff)
                            texture_format = GL_RGBA;
                    //else
                            //texture_format = GL_BGRA;
            } else if (nOfColors == 3)     // no alpha channel
            {
                    if (surface->format->Rmask == 0x000000ff)
                            texture_format = GL_RGB;
                    //else
                            //texture_format = GL_BGR;
            } else {
                Rosalila()->Utility->writeLogLine("Warning: "+ filename+ " is not truecolor. This will probably break.");
                    // this error should not go unhandled
            }

        // Have OpenGL generate a texture object handle for us
        glGenTextures( 1, &texture );

        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, texture );

        // Set the texture's stretching properties
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

//for(int x=0;x<surface->w;x++)
//for(int y=0;y<surface->h;y++)
//((unsigned int*)surface->pixels)[y*(surface->pitch/sizeof(unsigned int)) + x]+=1;
        // Edit the texture object's image data using the information SDL_Surface gives us
#ifdef OSX
        glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
                          GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels );
#else
        glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
                          texture_format, GL_UNSIGNED_BYTE, surface->pixels );
#endif
    }
    else {
        std::string sdl_error=SDL_GetError();
        Rosalila()->Utility->writeLogLine("SDL could not load "+filename+": "+sdl_error);
        SDL_Quit();
        return NULL;
    }


    Image*image=new Image();
    image->setTexture(texture);
    image->setWidth(surface->w);
    image->setHeight(surface->h);

    // Free the SDL_Surface only if it was successfully created
    if ( surface ) {
        SDL_FreeSurface( surface );
    }

    Rosalila()->Utility->writeLogLine(filename+" loaded");

    return image;
}

void RosalilaGraphics::draw2DImage	(
             Image* texture,
             int size_x,int size_y,
             int position_x,int position_y,
             float scale,
             float rotation,
             bool flipHorizontally,
             int depth_effect_x,
             int depth_effect_y,
             Color color_effects,
             int shadow_x,int shadow_y,
             bool camera_align,
             FlatShadow flat_shadow)
{
    double grey_scale = (color_effects.red+color_effects.green+color_effects.blue)/3;

    double red_difference = color_effects.red-grey_scale;
    double green_difference = color_effects.green-grey_scale;
    double blue_difference = color_effects.blue-grey_scale;

    color_effects.red = grey_scale + red_difference*current_grayscale_percentage;
    color_effects.green = grey_scale + green_difference*current_grayscale_percentage;
    color_effects.blue = grey_scale + blue_difference*current_grayscale_percentage;


    glEnable( GL_TEXTURE_2D );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0.0f, screen_width, screen_height, 0.0f, -1.0f, 1.0f);
    glMatrixMode( GL_MODELVIEW );

    glDisable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glDisable (GL_DEPTH_TEST);

    //Camera and depth effect
    if(depth_effect_x>0)
    {
        position_x-=camera_x/depth_effect_x;
    }else if(depth_effect_x<0)
    {
        position_x-=camera_x*-depth_effect_x;
    }else if(camera_align)
    {
        position_x-=camera_x;
    }

    if(depth_effect_y>0)
    {
        position_y+=camera_y/depth_effect_y;
    }else if(depth_effect_y<0)
    {
        position_y+=camera_y*-depth_effect_y;
    }else if(camera_align)
    {
        position_y+=camera_y;
    }

    //Screen shake
    position_x += current_screen_shake_x;
    position_y += current_screen_shake_y;

    GLfloat x1=0.f+position_x;
    GLfloat y1=0.f+position_y;
    GLfloat x2=0.f+position_x+(float)size_x*scale;
    GLfloat y2=0.f+position_y+(float)size_y*scale;

    //Flip
    if(flipHorizontally)
    {
        GLfloat temp=x1;
        x1=x2;
        x2=temp;
    }

    //OpenGL draw
    //Save the current matrix.
    glPushMatrix();
    //Change the current matrix.
    float translate_x=(x2-x1)/2+position_x;
    float translate_y=(y2-y1)/2+position_y;
    glTranslatef(translate_x,translate_y, 1.0);
    glRotatef(-rotation, 0, 0, 1.0);

    glColor4ub(color_effects.red, color_effects.green, color_effects.blue,color_effects.alpha);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    if((flat_shadow.shadow_x!=0 || flat_shadow.shadow_y!=0) && flat_shadow.points_left.size()>0)
    {
        glBindTexture( GL_TEXTURE_2D, flat_shadow.image->getTexture() );

    glColor4ub(color_effects.red, color_effects.green, color_effects.blue,color_effects.alpha);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glColor4ub(100,100,100,100);

        if(x1>=flat_shadow.shadow_x)// Right shadow
        {
            float gifiti1=(x2-flat_shadow.shadow_x)*flat_shadow.shadow_lenght;
            float gifiti2=(y1-flat_shadow.shadow_y)*flat_shadow.shadow_lenght;
            float gifiti1b=(x1-flat_shadow.shadow_x)*flat_shadow.shadow_lenght;
            float gifiti2b=(y2-flat_shadow.shadow_y)*flat_shadow.shadow_lenght;
            glBegin( GL_POLYGON );
                //Top-left vertex (corner)
                //glColor4ub(100,100,100,255);
                glColor4ub(255,255,255,flat_shadow.alpha_init);
                glTexCoord2i( 0, 0 );
                glVertex3f( x2-translate_x+flat_shadow.points_right[1]->x,
                           y1-translate_y+flat_shadow.points_right[1]->y,
                           0.0f );

                //Bottom-right vertex (corner)
                glColor4ub(255,255,255,flat_shadow.alpha_end);
                //glColor4ub(0,0,0,255);
                glTexCoord2i( 1, 0 );
                glVertex3f( x2-translate_x+gifiti1, y1-translate_y+gifiti2, 0.f );


                //Down-right vertex (corner)
                //glColor4ub(0,0,0,255);
                glColor4ub(255,255,255,flat_shadow.alpha_end);
                glTexCoord2i( 1, 1 );
                glVertex3f( x1-translate_x+gifiti1b, y2-translate_y+gifiti2b, 0.f );



                //Top-left vertex (corner)
                //glColor4ub(100,100,100,255);
                glColor4ub(255,255,255,flat_shadow.alpha_init);
                glTexCoord2i( 0, 1 );
                glVertex3f( x1-translate_x+flat_shadow.points_right[0]->x,
                            y2-translate_y+flat_shadow.points_right[0]->y,
                            0.f );


                for(int i=0;i<(int)flat_shadow.inbetween_points_right.size();i++)
                glVertex3f( x1-translate_x+flat_shadow.points_right[0]->x+flat_shadow.inbetween_points_right[i]->x,
                            y2-translate_y+flat_shadow.points_right[0]->y+flat_shadow.inbetween_points_right[i]->y,
                            0.f );

            glEnd();
        }else if(x2<=flat_shadow.shadow_x)// Left shadow
        {
            float gifiti1=(flat_shadow.shadow_x-x1)*flat_shadow.shadow_lenght;
            float gifiti2=(flat_shadow.shadow_y-y1)*flat_shadow.shadow_lenght;
            float gifiti1b=(flat_shadow.shadow_x-x2)*flat_shadow.shadow_lenght;
            float gifiti2b=(flat_shadow.shadow_y-y2)*flat_shadow.shadow_lenght;
            glBegin( GL_POLYGON );
                //Bottom-left vertex (corner)
                //glColor4ub(0,0,0,255);
                glColor4ub(255,255,255,flat_shadow.alpha_end);
                glTexCoord2i( 0, 0 );
                glVertex3f( x1-translate_x-gifiti1, y1-translate_y-gifiti2, 0.0f );

                //Bottom-right vertex (corner)
                //glColor4ub(100,100,100,255);
                glColor4ub(255,255,255,flat_shadow.alpha_init);
                glTexCoord2i( 1, 0 );
                glVertex3f( x1-translate_x+flat_shadow.points_left[0]->x,
                           y1-translate_y+flat_shadow.points_left[0]->y,
                           0.f );

                for(int i=0;i<(int)flat_shadow.inbetween_points_left.size();i++)
                glVertex3f( x1-translate_x+flat_shadow.points_left[0]->x+flat_shadow.inbetween_points_left[i]->x,
                           y1-translate_y+flat_shadow.points_left[0]->y+flat_shadow.inbetween_points_left[i]->y,
                           0.f );

                //Top-right vertex (corner)
                //glColor4ub(100,100,100,255);
                glColor4ub(255,255,255,flat_shadow.alpha_init);
                glTexCoord2i( 1, 1 );
                glVertex3f( x2-translate_x+flat_shadow.points_left[1]->x,
                           y2-translate_y+flat_shadow.points_left[1]->y,
                           0.f );

                //Top-left vertex (corner)
                //glColor4ub(0,0,0,255);
                glColor4ub(255,255,255,flat_shadow.alpha_end);
                glTexCoord2i( 0, 1 );
                glVertex3f( x2-translate_x-gifiti1b, y2-translate_y-gifiti2b, 0.f );

            glEnd();
        }else// Top shadow
        {
            float gifiti1=(flat_shadow.shadow_x-x1)*flat_shadow.shadow_lenght;
            float gifiti2=(flat_shadow.shadow_y-y1)*flat_shadow.shadow_lenght;

            float gifiti1x=(x2-flat_shadow.shadow_x)*flat_shadow.shadow_lenght;
            float gifiti2x=(y1-flat_shadow.shadow_y)*flat_shadow.shadow_lenght;

            glBegin( GL_QUADS );
                //Bottom-left vertex (corner)
                //glColor4ub(0,0,0,255);
                glColor4ub(255,255,255,flat_shadow.alpha_end);
                glTexCoord2i( 0, 0 );
                glVertex3f( x1-translate_x-gifiti1, y1-translate_y-gifiti2, 0.0f );

                //Bottom-right vertex (corner)
                //glColor4ub(100,100,100,255);
                glColor4ub(255,255,255,flat_shadow.alpha_init);
                glTexCoord2i( 1, 0 );
                glVertex3f( x1-translate_x+flat_shadow.points_top[0]->x,
                           y1-translate_y+flat_shadow.points_top[0]->y,
                           0.f );


                //Top-right vertex (corner)
                //glColor4ub(100,100,100,255);
                glColor4ub(255,255,255,flat_shadow.alpha_init);
                glTexCoord2i( 1, 1 );
                glVertex3f( x2-translate_x+flat_shadow.points_top[1]->x,
                           y1-translate_y+flat_shadow.points_top[1]->y,
                           0.f );

                //Top-left vertex (corner)
                glColor4ub(255,255,255,flat_shadow.alpha_end);
                glTexCoord2i( 0, 1 );
                glVertex3f( x2-translate_x+gifiti1x, y1-translate_y+gifiti2x, 0.f );

            glEnd();
        }
        glColor4ub(color_effects.red, color_effects.green, color_effects.blue,color_effects.alpha);
    }





//if((flat_shadow.shadow_x!=0 || flat_shadow.shadow_y!=0) && flat_shadow.points.size()>0)
//{
//
//}else
//{
glBindTexture( GL_TEXTURE_2D, texture->getTexture() );


    glBegin( GL_QUADS);
        //Bottom-left vertex (corner)
        glTexCoord2i( 0, 0 );
        glVertex3f( x1-translate_x, y1-translate_y, 0.0f );

        //Bottom-right vertex (corner)
        glTexCoord2i( 1, 0 );
        glVertex3f( x2-translate_x, y1-translate_y, 0.f );

        //Top-right vertex (corner)
        glTexCoord2i( 1, 1 );
        glVertex3f( x2-translate_x, y2-translate_y, 0.f );

        //Top-left vertex (corner)
        glTexCoord2i( 0, 1 );
        glVertex3f( x1-translate_x, y2-translate_y, 0.f );

    glEnd();
//}


    //if(shadow_x!=0 || shadow_y!=0)
    if(false)
    {
        glColor4ub(0, 0, 0,100);
        glBegin( GL_QUADS );
            //Bottom-left vertex (corner)
            glTexCoord2i( 0, 0 );
            glVertex3f( x1-translate_x-size_x/3, y1-translate_y+((double)size_y/1.2), 0.0f );

            //Bottom-right vertex (corner)
            glTexCoord2i( 1, 0 );
            glVertex3f( x2-translate_x-size_x/3, y1-translate_y+((double)size_y/1.2), 0.f );

            //Top-right vertex (corner)
            glTexCoord2i( 1, 1 );
            glVertex3f( x2-translate_x, y2-translate_y, 0.f );

            //Top-left vertex (corner)
            glTexCoord2i( 0, 1 );
            glVertex3f( x1-translate_x, y2-translate_y, 0.f );

        glEnd();
    }

    //Reset the current matrix to the one that was saved.
    glPopMatrix();
}

void RosalilaGraphics::draw2DImageBatch(
             Image* texture,
             int size_x,int size_y,
             vector<int> position_x,vector<int>position_y,
             float scale,
             vector<float>rotation,
             bool flipHorizontally,
             int depth_effect_x,
             int depth_effect_y,
             Color color_effects,
             int shadow_x, int shadow_y,
             bool camera_align,
             FlatShadow flat_shadow)
{

    //Camera and depth effect

    glEnable( GL_TEXTURE_2D );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0.0f, screen_width, screen_height, 0.0f, -1.0f, 1.0f);
    glMatrixMode( GL_MODELVIEW );

    glDisable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glDisable (GL_DEPTH_TEST);

    glColor4ub(color_effects.red, color_effects.green, color_effects.blue,color_effects.alpha);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glBindTexture( GL_TEXTURE_2D, texture->getTexture() );



    glPushMatrix();
    glTranslatef(1.0,1.0, 1.0);

    glBegin( GL_QUADS);
    for(int i=0;i<(int)position_x.size();i++)
    {

        if(depth_effect_x>0)
        {
            position_x[i]-=camera_x/depth_effect_x;
        }else if(depth_effect_x<0)
        {
            position_x[i]-=camera_x*-depth_effect_x;
        }else if(camera_align)
        {
            position_x[i]-=camera_x;
        }

        if(depth_effect_y>0)
        {
            position_y[i]+=camera_y/depth_effect_y;
        }else if(depth_effect_y<0)
        {
            position_y[i]+=camera_y*-depth_effect_y;
        }else if(camera_align)
        {
            position_y[i]+=camera_y;
        }

        //Screen shake
        position_x[i] += current_screen_shake_x;
        position_y[i] += current_screen_shake_y;

        GLfloat x1=0.f+position_x[i];
        GLfloat y1=0.f+position_y[i];
        GLfloat x2=0.f+position_x[i]+(float)size_x*scale;
        GLfloat y2=0.f+position_y[i]+(float)size_y*scale;

        //Flip
        if(flipHorizontally)
        {
            GLfloat temp=x1;
            x1=x2;
            x2=temp;
        }

        //OpenGL draw
        //Save the current matrix.

        //Change the current matrix.
        //glTranslatef(translate_x,translate_y, 1.0);

        //glRotatef(-rotation[i], 0, 0, 1.0);


            //Bottom-left vertex (corner)
            glTexCoord2i( 0, 0 );
            glVertex3f( x1, y1, 0.0f );

            //Bottom-right vertex (corner)
            glTexCoord2i( 1, 0 );
            glVertex3f( x2, y1, 0.f );

            //Top-right vertex (corner)
            glTexCoord2i( 1, 1 );
            glVertex3f( x2, y2, 0.f );

            //Top-left vertex (corner)
            glTexCoord2i( 0, 1 );
            glVertex3f( x1, y2, 0.f );



    }
    glEnd();
    glPopMatrix();

    //Reset the current matrix to the one that was saved.
//    glPopMatrix();
}


void RosalilaGraphics::drawRectangle(int x,int y,int width,int height,float rotation,int red,int green,int blue,int alpha,bool camera_align)
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0.0f, screen_width, screen_height, 0.0f, -1.0f, 1.0f);
    glMatrixMode( GL_MODELVIEW );

    glDisable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glDisable (GL_DEPTH_TEST);

    glDisable(GL_TEXTURE_2D);
    GLubyte r=red;
    GLubyte g=green;
    GLubyte b=blue;
    GLubyte a=alpha;
    glColor4ub(r,g,b,a);

    if(camera_align)
    {
        y+=camera_y;
        x-=camera_x;
    }

    //OpenGL draw
    //Save the current matrix.
    glPushMatrix();
    //Change the current matrix.
    float translate_x=x;
    float translate_y=y;
    glTranslatef(translate_x,translate_y, 1.0);
    glRotatef(-rotation, 0, 0, 1.0);
    x-=translate_x;
    y-=translate_y;

    glRecti(x, y, width+x, height+y);
    glFlush();

    //Reset the current matrix to the one that was saved.
    glPopMatrix();
}

void RosalilaGraphics::frameCap()
{
    frame++;

    //If a second has passed since the caption was last updated

    //If we want to cap the frame rate
    int ticks_difference = SDL_GetTicks() - last_tick;
    if(ticks_difference < 1000 / frames_per_seccond)
    {
        //Sleep the remaining frame time
        SDL_Delay( ( 1000 / frames_per_seccond ) - ticks_difference );
    }

    last_tick = SDL_GetTicks();
}

void RosalilaGraphics::drawText(std::string text,int position_x,int position_y)
{
    if(text=="")
        text=" ";
    GLuint texture;

    SDL_Surface *message = NULL;
    message = TTF_RenderUTF8_Blended( font, text.c_str(), textColor );


    // Prepare the texture for the font
    GLenum textFormat;
    if(message->format->BytesPerPixel == 4)
    {
        // alpha
        if(message->format->Rmask == 0x000000ff)
            textFormat = GL_RGBA;
        else
            textFormat = GL_BGRA_EXT;
    }

    // Create the font's texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, message->format->BytesPerPixel, message->w, message->h, 0, textFormat, GL_UNSIGNED_BYTE, message->pixels);

    GLfloat x1=0.f+position_x;
    GLfloat y1=0.f+position_y;
    GLfloat x2=0.f+position_x+message->w;
    GLfloat y2=0.f+position_y+message->h;
    SDL_FreeSurface(message);


    //OpenGL draw
    glBindTexture( GL_TEXTURE_2D, texture );
    glColor3ub(255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glBegin( GL_QUADS );

        //Bottom-left vertex (corner)
        glTexCoord2i( 0, 0 );
        glVertex3f( x1, y1, 0.0f );

        //Bottom-right vertex (corner)
        glTexCoord2i( 1, 0 );
        glVertex3f( x2, y1, 0.f );

        //Top-right vertex (corner)
        glTexCoord2i( 1, 1 );
        glVertex3f( x2, y2, 0.f );

        //Top-left vertex (corner)
        glTexCoord2i( 0, 1 );
        glVertex3f( x1, y2, 0.f );

    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures( 1, &texture );
}

void RosalilaGraphics::updateScreen()
{
    std::string error= ">>>";
    error+=SDL_GetError();
    if(error!=">>>")
        Rosalila()->Utility->writeLogLine(error);
    frameCap();

    if(shake_time>0)
    {
        shake_time--;
        if(shake_time==0)
        {
            current_screen_shake_x = 0;
            current_screen_shake_y = 0;
        }else
        {
            current_screen_shake_x = (rand()*10000)%shake_magnitude;
            current_screen_shake_y = (rand()*10000)%shake_magnitude;
        }
    }

    updateGrayscale();

    SDL_GL_SwapWindow(window);
}

void RosalilaGraphics::shakeScreen(int shake_magnitude, int shake_time)
{
    this->shake_magnitude = shake_magnitude;
    this->shake_time = shake_time;
    this->shake_original_x = camera_x;
    this->shake_original_y = camera_y;
}

void RosalilaGraphics::setGrayscale(double target_grayscale_percentage, double grayscale_change_speed)
{
    this->target_grayscale_percentage = target_grayscale_percentage;
    this->grayscale_change_speed = grayscale_change_speed;
    updateGrayscale();
}

void RosalilaGraphics::updateGrayscale()
{
    if(current_grayscale_percentage<target_grayscale_percentage)
    {
       current_grayscale_percentage+=grayscale_change_speed;
       if(current_grayscale_percentage>target_grayscale_percentage)
       {
           current_grayscale_percentage=target_grayscale_percentage;
       }
    }
    if(current_grayscale_percentage>target_grayscale_percentage)
    {
       current_grayscale_percentage-=grayscale_change_speed;
       if(current_grayscale_percentage<target_grayscale_percentage)
       {
           current_grayscale_percentage=target_grayscale_percentage;
       }
    }
}

void RosalilaGraphics::screenshot(int x, int y, int w, int h, string filename)
{
    unsigned char * pixels = new unsigned char[w*h*4]; // 4 bytes for RGBA
    glReadPixels(x,y,w, h, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

    SDL_Surface * surf = SDL_CreateRGBSurfaceFrom(pixels, w, h, 8*4, w*4, 0,0,0,0);
    SDL_SaveBMP(surf, filename.c_str());

    SDL_FreeSurface(surf);
    delete [] pixels;
}
