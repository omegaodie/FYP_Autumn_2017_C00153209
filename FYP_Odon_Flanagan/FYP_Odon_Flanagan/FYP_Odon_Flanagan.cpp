//////////////////////////////////////////////////////////// 
// Headers 
//////////////////////////////////////////////////////////// 
#include "stdafx.h" 
#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 
#pragma comment(lib,"opengl32.lib") 
#pragma comment(lib,"glu32.lib") 


#include <iostream> 
#include "SFML/Graphics.hpp" 
#include "SFML/OpenGL.hpp" 


#include "Fluid_Sim_2D.h"
#include "Fluid_Sim_3D.h"
#include "Fluid_Sim_3D_Muzkaw.h"



int main()
{
	int sizeX = 500;
	int sizeY = 500;
	int _scale = 2;

	float _cell_width = (sizeX / (sizeX / _scale));
	float _cell_heigth = (sizeY / (sizeY /_scale));
	// Create the main window 
	sf::RenderWindow App(sf::VideoMode(sizeX, sizeY, 32), "SFML OpenGL");
	//sf::RenderWindow App(sf::VideoMode(800, 600, 32), "SFML OpenGL");

	// Create a clock for measuring time elapsed     
	sf::Clock Clock;
	//create a font
	//bool _run = false;
	int _variable_shown = 0;
	float _dt = 0.0001;

	sf::Font font;
	if (!font.loadFromFile("sansation.ttf"))
	{
		// error...
	}
	sf::Color _col, _zero_col;
	_zero_col = sf::Color(0, 0, 0, 0);
	//prepare OpenGL surface for HSR
	glClearDepth(1.f);
	glClearColor(0.3f, 0.3f, 0.3f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//// Setup a perspective projection & Camera position
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(90.f, 1.f, 1.f, 300.0f);//fov, aspect, zNear, zFar
	GLfloat zNear = 0.1f;
	GLfloat zFar = 255.0f;
	GLfloat aspect = float(800) / float(600);
	GLfloat fH = tan(float(90 / 360.0f * 3.14159f)) * zNear;
	GLfloat fW = fH * aspect;
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);



	bool rotate = true;
	float angle = 0;
	bool _run = false;

	int _3d_scale = 1;
	Fluid_Sim_2D _sim_2D = Fluid_Sim_2D(sizeX / _scale, sizeY / _scale, _cell_width, _scale);
	//Fluid_Sim_3D _sim_3D_Stam = Fluid_Sim_3D(100 / _3d_scale, 150 / _3d_scale, 100 / _3d_scale, _3d_scale);
	//Fluid_Sim_3D_Muzkaw _sim_3d_muz = Fluid_Sim_3D_Muzkaw(100 / _3d_scale, 150 / _3d_scale, 100 / _3d_scale, 1.239, 100000, 400, 1, _3d_scale);
	//Fluid_Sim_3D_Muzkaw _sim_3d_muz = Fluid_Sim_3D_Muzkaw(10, 15, 10, 1.239, 100000, 400, 1, _3d_scale);

	//_sim_3d_muz.setExplosion();
	sf::Vector2f mousePos;

	int _sim_ID = 1; //1 = 2d, 2 =  3d, 3 = 3d_muz

	int iter = 0;
	 //Start game loop 
	while (App.isOpen())
	{
		// Process events 
		sf::Event Event;
		while (App.pollEvent(Event))
		{
			// Close window : exit 
			if (Event.type == sf::Event::Closed)
				App.close();
	
			// Escape key : exit 
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
				App.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
			{
				if (_run == false) {
					_run = true;
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				if (_run == true) {
					_run = false;
				}
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				angle += 10;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				angle -= 10;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
				_sim_ID = 1;
				std::cout << "2d" << std::endl;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
				_sim_ID = 2;
				std::cout << "3d Stam" << std::endl;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
				_sim_ID = 3;
				std::cout << "3d Muz" << std::endl;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
			{
				_variable_shown = 0;
				std::cout << "0" << std::endl;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
			{
				_variable_shown = 1;
				std::cout << "1" << std::endl;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
			{
				_variable_shown = 2;
				std::cout << "2" << std::endl;
			}
			//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
			//{
			//	_variable_shown = 3;
			//	std::cout << "3" << std::endl;
			//}
		}
		if (_sim_ID == 1)
		{
			//std::cout << "true_running" << std::endl;
			mousePos = sf::Vector2f(sf::Mouse::getPosition(App).x, sf::Mouse::getPosition(App).y);
			//Prepare for drawing 
			// Clear color and depth buffer 
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
			// Apply some transformations 
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
	
	
			App.clear(sf::Color::Black);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				int X = mousePos.x / _cell_width;
				int Y = mousePos.y / _cell_heigth;
				if (X < sizeX && X > 0 && Y < sizeY && Y > 0)
				{
					_sim_2D.addDensity(X, Y);
				}
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				int X = mousePos.x / _cell_width;
				int Y = mousePos.y / _cell_heigth;
				if (X < sizeX && X > 0 && Y < sizeY && Y > 0)
				{
					_sim_2D.addVelocity(X, Y);
				}
			}
			if (_run)
			{
				//std::cout << "RUN" << std::endl;
				_sim_2D._update(_dt);
				//_run = false;
			}
	
			//App.draw(_sim_2D.getDensitySprite());
			App.draw(_sim_2D.getSprite_general(_variable_shown));
		}
		//else if(_sim_ID == 2)
		//{
		//	if (_run)
		//	{
		//		std::cout << "RUN" << std::endl;
		//		//_sim_3d_muz.createWind();
		//		_sim_3D_Stam._update_stam(_dt);
		//		_run = false;
		//	}
		//	//Prepare for drawing
		//	// Clear color and depth buffer
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		//	// Apply some transformations for the cube
		//	glMatrixMode(GL_MODELVIEW);
		//	glLoadIdentity();
		//	glTranslatef(0.f, -30.f, -150.f);
		//	glRotatef(45, 0.f, 1.f, 0.f);
		//	//if (rotate) {
		//	//	angle = Clock.getElapsedTime().asSeconds();
		//	//}
		//	//glRotatef(angle * 50, 1.f, 0.f, 0.f);
		//	//glRotatef(angle * 30, 0.f, 1.f, 0.f);
		//	//glRotatef(angle * 90, 0.f, 0.f, 1.f);
	
	
	
		//	//Draw a cube
		//	////LEFT FACE
		//	glBegin(GL_QUADS);//draw some squares
		//	glColor4f(0, 1, 1, 0.5);
		//	glVertex3f(-50.f, -50.f, -50.f);
		//	glVertex3f(-50.f, 100.f, -50.f);
		//	glVertex3f(50.f, 100.f, -50.f);
		//	glVertex3f(50.f, -50.f, -50.f);
	
		//	////Right FACE
		//	glColor4f(0, 1, 1, 0.5);
		//	glVertex3f(50.f, -50.f, -50.f);
		//	glVertex3f(50.f, 100.f, -50.f);
		//	glVertex3f(50.f, 100.f, 50.f);
		//	glVertex3f(50.f, -50.f, 50.f);
	
	
		//	//BOTTOM
		//	glColor4f(0, 1, 1, 0.1);
		//	glVertex3f(-50.f, -50.f, 50.f);
		//	glVertex3f(-50.f, -50.f, -50.f);
		//	glVertex3f(50.f, -50.f, -50.f);
		//	glVertex3f(50.f, -50.f, 50.f);
	
		//	glEnd();
	
		//	glBegin(GL_LINES);
		//	glColor4f(1, 1, 0, 1);
		//	//LEFT Face BOt
		//	glVertex3f(-50.f, -50.f, -50.f);
		//	glVertex3f(50.f, -50.f, -50.f);
		//	//LEFT Face top
		//	glVertex3f(-50.f, 100.f, -50.f);
		//	glVertex3f(50.f, 100.f, -50.f);
		//	//LEFT Face left
		//	glVertex3f(-50.f, -50.f, -50.f);
		//	glVertex3f(-50.f, 100.f, -50.f);
		//	//RIGHT FACE RIGHT
		//	glVertex3f(50.f, -50.f, 50.f);
		//	glVertex3f(50.f, 100.f, 50.f);
		//	//CENTER
		//	glVertex3f(50.f, 100.f, -50.f);
		//	glVertex3f(50.f, -50.f, -50.f);
		//	//RIGHT FACE BOTTOM
		//	glVertex3f(50.f, -50.f, -50.f);
		//	glVertex3f(50.f, -50.f, 50.f);
	
		//	////LEFT Face top
		//	glVertex3f(50.f, 100.f, -50.f);
		//	glVertex3f(50.f, 100.f, 50.f);
	
		//	//FRONTRIGHT
		//	glVertex3f(50.f, -50.f, 50.f);
		//	glVertex3f(-50.f, -50.f, 50.f);
	
		//	//FRONTLEFT
		//	glVertex3f(-50.f, -50.f, 50.f);
		//	glVertex3f(-50.f, -50.f, -50.f);
		//	glEnd();
		//	for (int i = -50, x = 0; i < 50; i++, x++) {
		//		for (int j = -50, y = 0; j < 100; j++, y++) {
		//			for (int k = -50, z = 0; k < 50; k++ , z++)
		//			{
		//				_col = _sim_3D_Stam.getColour_general(4, x, y, z);
		//				//if (_col.a != 0) {
		//				//	std::cout << _col.r << std::endl;
		//				//	//std::cout << i << j << k << std::endl;
		//				//	//std::cout << i << j << k << std::endl;
		//				//	//std::cout << i << j << k << std::endl;
		//				//}
		//				glBegin(GL_QUADS);//draw some squares
		//				glColor4f(_col.r, _col.g, _col.b, _col.a);
	
		//				glVertex3f(i *_3d_scale, j *_3d_scale, k);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale, k);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale + 1, k);
		//				glVertex3f(i *_3d_scale, j *_3d_scale + 1, k);
	
		//				//glColor4f(_col.r, _col.g, _col.b, 0.001);
		//				glVertex3f(i *_3d_scale, j *_3d_scale + 1, k);
		//				glVertex3f(i *_3d_scale, j *_3d_scale + 1, k + 1);
		//				glVertex3f(i *_3d_scale, j *_3d_scale, k + 1);
		//				glVertex3f(i *_3d_scale, j *_3d_scale, k);
	
		//				//glColor4f(_col.r, _col.g, _col.b, 0.001);
		//				glVertex3f(i *_3d_scale, j *_3d_scale + 1, k + 1);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale + 1, k + 1);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale, k + 1);
		//				glVertex3f(i *_3d_scale, j *_3d_scale, k + 1);
	
		//				//glColor4f(_col.r, _col.g, _col.b, 0.001);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale + 1, k + 1);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale + 1, k);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale, k);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale, k + 1);
	
		//				//glColor4f(_col.r, _col.g, _col.b, 0.001);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale, k + 1);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale, k);
		//				glVertex3f(i *_3d_scale, j *_3d_scale, k);
		//				glVertex3f(i *_3d_scale, j *_3d_scale, k + 1);
	
		//				//glColor4f(_col.r, _col.g, _col.b, 0.001);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale + 1, k + 1);
		//				glVertex3f(i *_3d_scale + 1, j *_3d_scale + 1, k);
		//				glVertex3f(i *_3d_scale, j *_3d_scale + 1, k);
		//				glVertex3f(i *_3d_scale, j *_3d_scale + 1, k + 1);
		//				glEnd();
		//			}
		//		}
		//	}
		//}
		//else
		//	{
		//		if (_run)
		//		{
		//			iter++;
		//			//std::cout << "RUN" << std::endl;
		//			_sim_3d_muz.Update(_dt);
		//			if (iter < 10) {
		//				_sim_3d_muz.createWind();
		//			}
		//		}
		//			//Prepare for drawing
		//			// Clear color and depth buffer
		//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//			// Apply some transformations for the cube
		//			glMatrixMode(GL_MODELVIEW);
		//			glLoadIdentity();
		//			glTranslatef(0.f, -30.f, -150.f);
		//			glRotatef(45 + angle, 0.f, 1.f, 0.f);
		//			//if (rotate) {
		//			//	angle = Clock.getElapsedTime().asSeconds();
		//			//}
		//			//glRotatef(angle * 50, 1.f, 0.f, 0.f);
		//			//glRotatef(angle * 30, 0.f, 1.f, 0.f);
		//			//glRotatef(angle * 90, 0.f, 0.f, 1.f);



		//			//Draw a cube
		//			////LEFT FACE
		//			glBegin(GL_QUADS);//draw some squares
		//			glColor4f(0, 1, 1, 0.5);
		//			glVertex3f(-50.f, -50.f, -50.f);
		//			glVertex3f(-50.f, 100.f, -50.f);
		//			glVertex3f(50.f, 100.f, -50.f);
		//			glVertex3f(50.f, -50.f, -50.f);

		//			////Right FACE
		//			glColor4f(0, 1, 1, 0.5);
		//			glVertex3f(50.f, -50.f, -50.f);
		//			glVertex3f(50.f, 100.f, -50.f);
		//			glVertex3f(50.f, 100.f, 50.f);
		//			glVertex3f(50.f, -50.f, 50.f);


		//			//BOTTOM
		//			glColor4f(0, 1, 1, 0.1);
		//			glVertex3f(-50.f, -50.f, 50.f);
		//			glVertex3f(-50.f, -50.f, -50.f);
		//			glVertex3f(50.f, -50.f, -50.f);
		//			glVertex3f(50.f, -50.f, 50.f);

		//			glEnd();

		//			glBegin(GL_LINES);
		//			glColor4f(1, 1, 0, 1);
		//			//LEFT Face BOt
		//			glVertex3f(-50.f, -50.f, -50.f);
		//			glVertex3f(50.f, -50.f, -50.f);
		//			//LEFT Face top
		//			glVertex3f(-50.f, 100.f, -50.f);
		//			glVertex3f(50.f, 100.f, -50.f);
		//			//LEFT Face left
		//			glVertex3f(-50.f, -50.f, -50.f);
		//			glVertex3f(-50.f, 100.f, -50.f);
		//			//RIGHT FACE RIGHT
		//			glVertex3f(50.f, -50.f, 50.f);
		//			glVertex3f(50.f, 100.f, 50.f);
		//			//CENTER
		//			glVertex3f(50.f, 100.f, -50.f);
		//			glVertex3f(50.f, -50.f, -50.f);
		//			//RIGHT FACE BOTTOM
		//			glVertex3f(50.f, -50.f, -50.f);
		//			glVertex3f(50.f, -50.f, 50.f);

		//			////LEFT Face top
		//			glVertex3f(50.f, 100.f, -50.f);
		//			glVertex3f(50.f, 100.f, 50.f);

		//			//FRONTRIGHT
		//			glVertex3f(50.f, -50.f, 50.f);
		//			glVertex3f(-50.f, -50.f, 50.f);

		//			//FRONTLEFT
		//			glVertex3f(-50.f, -50.f, 50.f);
		//			glVertex3f(-50.f, -50.f, -50.f);
		//			glEnd();
		//			for (int i = -50, x = 0; i < 50; i++, x++) {
		//				for (int j = -50, y = 0; j < 100; j++, y++) {
		//					for (int k = -50, z = 0; k < 50; k++, z++)
		//					{
		//						_col = _sim_3d_muz.getColour_general(_variable_shown, x, y, z);
		//						//if (_col.a != 0) {
		//						//	std::cout << _col.r << std::endl;
		//						//	//std::cout << i << j << k << std::endl;
		//						//	//std::cout << i << j << k << std::endl;
		//						//	//std::cout << i << j << k << std::endl;
		//						//}
		//						glBegin(GL_QUADS);//draw some squares
		//						glColor4f(_col.r, _col.g, _col.b, _col.a);

		//						glVertex3f(i, j, k);
		//						glVertex3f(1 + 1, j, k);
		//						glVertex3f(i + 1, j + 1, k );
		//						glVertex3f(i, j + 1, k);

		//						//glColor4f(_col.r, _col.g, _col.b, 0.001);
		//						glVertex3f(i, j + 1, k);
		//						glVertex3f(i, j + 1, k + 1);
		//						glVertex3f(i, j, k + 1);
		//						glVertex3f(i, j, k);

		//						//glColor4f(_col.r, _col.g, _col.b, 0.001);
		//						glVertex3f(i, j + 1, k + 1);
		//						glVertex3f(i + 1, j + 1, k + 1);
		//						glVertex3f(i + 1, j, k + 1);
		//						glVertex3f(i, j, k + 1);

		//						//glColor4f(_col.r, _col.g, _col.b, 0.001);
		//						glVertex3f(i + 1, j + 1, k + 1);
		//						glVertex3f(i + 1, j + 1, k);
		//						glVertex3f(i + 1, j, k);
		//						glVertex3f(i + 1, j, k + 1);

		//						//glColor4f(_col.r, _col.g, _col.b, 0.001);
		//						glVertex3f(i + 1, j, k + 1);
		//						glVertex3f(i + 1, j, k);
		//						glVertex3f(i, j, k );
		//						glVertex3f(i, j, k + 1);

		//						//glColor4f(_col.r, _col.g, _col.b, 0.001);
		//						glVertex3f(i + 1, j + 1, k + 1);
		//						glVertex3f(i + 1, j + 1, k);
		//						glVertex3f(i, j + 1, k);
		//						glVertex3f(i, j + 1, k + 1);
		//						glEnd();
		//					}
		//				}
		//			}
		//		}
	
		App.display();
	}

	return EXIT_SUCCESS;
}



