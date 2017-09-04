#pragma once
#pragma once
//Credit Muzkaw
//Modified and extended by
//Odon Flanagan
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <iostream>
#include <vector>
//Credit Muzkaw
// https://www.youtube.com/user/Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
//Modified  and extended by Odon Flanagan

using namespace std;
using namespace sf;
#define PI 3.14159265359
class Fluid_Sim_3D_Muzkaw
{
public:
	Fluid_Sim_3D_Muzkaw();
	float _get(int const & i, int const & j, int const & k, int const & v) const;
	void _set(int const & i, int const & j, int const & k, int const & v, float const & n);
	Fluid_Sim_3D_Muzkaw(float const& _width, float const& _height, float const& _breadth, float const& _rho, float const& _k, float const& _buoyancyFactor, float const& _L, float const& _scale);

	void Update(float const& dt);

	void setExplosion();

	void createWind();



	sf::Color  getPressureColour(int x, int y, int z);
	sf::Color  getTemperatureColour(int x, int y, int z);
	sf::Color  getSpeedColour(int x, int y, int z);
	sf::Color  getDensityColour(int x, int y, int z);

	sf::Color  getColour_general(int var, int x, int y, int z);

	sf::Color colorGradient(sf::Image &color, float x);


private:
	int width;
	int height;
	int breadth;
	float rho;
	float k;
	float buoyancyFactor;
	float L;
	float ambientTemperature;
	vector<float> u;
	vector<float> v;
	vector<float> w;
	vector<float> u0;
	vector<float> v0;
	vector<float> w0;
	vector<float> p;
	vector<float> wall;
	vector<float> T;
	vector<float> T0;
	vector<float> density;
	vector<float> density0;

	int _N_x;
	int _N_y;
	int _N_z;

	Image posPressureColor;
	Image negPressureColor;
	Image temperatureColor;
	Image speedColor;
	Image densityColor;

	float pScale;
	float vScale;
	float tScale;

	float scale;
};

