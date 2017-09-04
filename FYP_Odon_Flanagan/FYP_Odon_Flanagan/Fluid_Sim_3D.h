#pragma once
#include "SFML/Graphics.hpp" 
#include "SFML/OpenGL.hpp" 
#include <Eigen/Dense>
#include <vector>
#include <iostream>

using Eigen::VectorXf;
using namespace sf;




class Fluid_Sim_3D
{

private:

	int width, height, breadth;

	float _size;

	int _N_x;
	int _N_y;
	int _N_z;

	std::vector<VectorXf> _volume_data;
	std::vector<VectorXf> _volume_data_0;

	std::vector<float> _walls;
	std::vector<float> _divergence;
	// 0 = u
	// 1 = v
	// 2 = w
	// 3 = p
	// 4 = T
	// 5 = d



	sf::Image posPressureColor;
	sf::Image negPressureColor;
	sf::Image temperatureColor;
	sf::Image speedColor;
	sf::Image densityColor;

	Image image;
	Texture texture;
	Sprite sprite;
	int scale;

	bool _pressure_added_this_frame;

	float _diffusion_coefficient_pressure;
	float _diffusion_coefficient_density;
	float _diffusion_coefficient_velocity;

public:
	Fluid_Sim_3D(int width, int height, int breadth ,  int scale);

	std::vector<float> _cells;

	void addDensity(int x, int y, int z);
	void addPressure(int x, int y, int z);
	void addV(int x, int y, int z);
	void addU(int x, int y, int z);
	void addW(int x, int y, int z);
	void addVelocity(int x, int y,int z);



	void applyBoundary(int var);// extra for divergance

	void boundaryConditionsDensity();
	void boundaryConditionsDivergance();
	void boundaryConditionsPressure();
	void boundaryConditionsUVelocity();
	void boundaryConditionsVVelocity();
	void boundaryConditionsWVelocity();

	void addSourceData(float _dt, int var);

	float get_actual(int const& i, int const& j, int const& k, int const& v) const;
	void set_actual(int const& i, int const& j, int const& k, int const& v, float const& n);

	float get_0(int const& i, int const& j, int const& k, int const& v) const;
	void set_0(int const& i, int const& j, int const& k, int const& v, float const& n);

	float get_gradient(int const& i, int const& j, int const& k) const;
	void set_gradient(int const& i, int const& j, int const& k, float const& n);

	float get_wall(int const& i, int const& j, int const& k, int const& v) const;
	void set_wall(int const& i, int const& j, int const& k, int const& v, float const& n);


	sf::Color  getPressureColour(int x, int y, int z);
	sf::Color  getTemperatureColour(int x, int y, int z);
	sf::Color  getSpeedColour(int x, int y, int z);
	sf::Color  getDensityColour(int x, int y, int z);

	sf::Color  getColour_general(int var, int x, int y, int z);

	sf::Color colorGradient(sf::Image &color, float x);



	void diffusion_stam(float _dt, int v, float viscosity);
	void advection_stam(float _dt, int v);
	void projection_stam(float _dt);
	void dens_step(float _dt);
	void vel_step(float _dt);

	void _update_stam(float _dt);
};
