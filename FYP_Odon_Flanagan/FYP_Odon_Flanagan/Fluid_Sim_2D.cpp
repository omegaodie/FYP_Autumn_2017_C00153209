#include "stdafx.h"
#include "Fluid_Sim_2D.h"

using Eigen::VectorXf;

Fluid_Sim_2D::Fluid_Sim_2D(int w, int h, int size ,int _scale) : width(w), height(h)
{

	_N_x = width - 2; //Provide buffer
	_N_y = height - 2;  //Provide buffer
	scale = _scale;
	_size = width * height;
	for (int i = 0; i < _size; i++)
	{
		_volume_data.push_back(VectorXf(5));
		_volume_data[i].setZero();
		_volume_data_0.push_back(VectorXf(5));
		_volume_data_0[i].setZero();
		_walls.push_back(0.0f);
		_divergence.push_back(0);
		//_volume_data[i](0) = 0.2;
		//_volume_data[i](1) = 2000;
	}
	_pressure_added_this_frame = false;
	image.create(width, height , Color::Blue);
	texture.loadFromImage(image);
	sprite.setTexture(texture, true);
	sprite.setScale(scale, scale);
	posPressureColor.loadFromFile("pressure+.jpg");
	negPressureColor.loadFromFile("pressure-.jpg");
	temperatureColor.loadFromFile("color.jpg");
	speedColor.loadFromFile("speed.jpg");
	densityColor.loadFromFile("density.jpg");


	_diffusion_coefficient_pressure = 8;
	_diffusion_coefficient_density = 1;
	_diffusion_coefficient_velocity  = 6;
}

void Fluid_Sim_2D::_update(float _dt)
{
	vel_step(_dt);
	dens_step(_dt);
}

void Fluid_Sim_2D::diffusion_stam(float _dt, int var, float viscosity)
{
	float _a = _dt *  viscosity * _N_x * _N_y;
	////DIFFUSION/////
		float _value;
		for (int k = 0; k < 10; k++) 
		{
			for (int i = 1; i <= _N_x; i++)
			{
				for (int j = 1; j <= _N_y; j++)
				{
					set_0(i, j, var, (get_actual(i, j, var)
						+ _a
						* (get_0(i - 1, j, var)
							+ get_0(i + 1, j, var)
							+ get_0(i, j - 1, var)
							+ get_0(i, j + 1, var)))
						/ (1+4*_a));
				}
			}
			applyBoundary(var);
		}

}

void Fluid_Sim_2D::advection_stam(float _dt, int var)
{

	////ADVECTION/////
	int i0, j0, i1, j1;

	float x, y, z, s0, t0, s1, t1;
	float dt0X = _dt * _N_x;
	float dt0Y = _dt * _N_y;
	for (int i = 1; i <= _N_x; i++)
	{
		for (int j = 1; j <= _N_y; j++)
		{
			x = i - dt0X * get_actual(i, j, 0);
			y = j - dt0Y * get_actual(i, j, 1);

			if (x < 0.5) {
				x = 0.5;
			}
			else if (x > _N_x + 0.5) {
				x = _N_x + 0.5;
			}

			if (y < 0.5) {
				y = 0.5;
			}
			else if (y > _N_y + 0.5) {
				y = _N_y + 0.5;
			}
			i0 = (int)x; //cell our subsection occupied last frame
			j0 = (int)y;  //cell our subsection occupied last frame


			i1 = i0 + 1; 
			j1 = j0 + 1; 

			s1 = x - i0; 
			s0 = 1 - s1; 

			t1 = y - j0;
			t0 = 1 - t1;

			set_actual(i, j, var,
				s0
				* (t0 * get_0(i0, j0, var)
					+ t1* get_0(i0, j1, var))
				+ s1
				* (t0 * get_0(i1, j0, var)
					+ t1* get_0(i1, j1, var)));
		}
	}
	applyBoundary(var);
}

void Fluid_Sim_2D::projection_stam(float _dt)
{
	int i, j, k;
	float h = 1.0 / ((_N_y + _N_y) / 2);

	


	for (i = 1; i <= _N_x; i++) {
		for (j = 1; j <= _N_y; j++) {
			set_gradient(i, j, -0.5f*h*(get_actual(i + 1, j, 0) - get_actual(i - 1, j, 0) +
				get_actual(i, j + 1, 1) - get_actual(i, j - 1, 1)));
			//if(!_pressure_added_this_frame)
			set_actual(i,j, 2, 0);
		}
	}
	_pressure_added_this_frame = false; // why waste an if.
	applyBoundary(5);
	applyBoundary(2);
	for (k = 0; k<5; k++) 
	{
		for (i = 1; i <= _N_x; i++) 
		{
			for (j = 1; j <= _N_y; j++) 
			{
				set_actual(i, j, 2, (get_gradient(i, j) + get_actual(i -1, j, 2) + get_actual(i + 1, j, 2) +
					get_actual(i, j - 1, 2) + get_actual(i, j + 1, 2)) / 4);
			}
		}
	applyBoundary(2);
	}
	for (i = 1; i <= _N_x; i++) 
	{
		for (j = 1; j <= _N_y; j++) 
		{
			set_actual(i, j, 0, 0.5*(get_actual(i + 1, j, 2) - get_actual(i - 1, j, 2)) / h);
			set_actual(i, j, 1, 0.5*(get_actual(i, j + 1, 2) - get_actual(i, j - 1, 2)) / h);
		}
	}
	applyBoundary(0);
	applyBoundary(1);
}

sf::Sprite Fluid_Sim_2D::getSprite_general(int var)
{
	switch (var)
	{
	default:
		break;
	case 0:
		return getSpeedSprite();
		break;
	case 1:
		return getPressureSprite();
		break;
	case 2:
		//PlaceHOLDER
		return getDensitySprite();
		break;
	}
}

void Fluid_Sim_2D::addDensity(int x, int y)
{
	for (int i = (x - 20); i < (x + 20); i++) {
		for (int j = (y - 20); j < (y + 20); j++) {
			if (i > 0 && i < width && j > 0 && j < height) 
			{
				float d = std::sqrt(std::pow(i - x, 2) + std::pow(j - y, 2));
				if (d < 20 / 4) {
					set_actual(i, j, 4, get_actual(i, j, 4) + 5);
				}
			}
		}
	}
}

void Fluid_Sim_2D::addPressure(int x, int y)
{
	for (int i = (x - 5); i < (x + 5); i++) {
		for (int j = (y - 5); j < (y + 5); j++) {
			if (i > 0 && i < width && j > 0 && j < height)
			{
					set_actual(i, j, 2, get_actual(i, j, 2) + 100.0f);
					_pressure_added_this_frame = true;
			}
		}
	}
}

void Fluid_Sim_2D::addV(int x, int y)
{
	for (int i = (x - 20); i < (x + 20); i++) {
		for (int j = (y - 20); j < (y + 20); j++) {
			if (i > 0 && i < width && j > 0 && j < height)
			{
				float d = std::sqrt(std::pow(i - x, 2) + std::pow(j - y, 2));
				if (d < 20 / 4) 
				{
					set_actual(i, j, 1, get_actual(i, j, 1) + 0.005f);
				}
			}
		}
	}
}

void Fluid_Sim_2D::addU(int x, int y)
{
	for (int i = (x - 20); i < (x + 20); i++) {
		for (int j = (y - 20); j < (y + 20); j++) {
			if (i > 0 && i < width && j > 0 && j < height)
			{
				float d = std::sqrt(std::pow(i - x, 2) + std::pow(j - y, 2));
				if (d < 20 / 4) 
				{
					set_actual(i, j, 0, get_actual(i, j, 0) + 0.005f);
				}
			}
		}
	}
}

void Fluid_Sim_2D::addVelocity(int x, int y)
{
	for (int i = (x - 8); i < (x + 8); i++) {
		for (int j = (y - 8); j < (y + 8); j++) {
			if (i > 0 && i < width && j > 0 && j < height)
			{
				float d = std::sqrt(std::pow(i - x, 2) + std::pow(j - y, 2));
				if (d < 8 / 4) 
				{
					set_actual(i, j, 0, get_actual(i, j, 0) + 1000000000.0f);
					set_actual(i, j, 1, get_actual(i, j, 1) + 1000000000.0f);
				}
			}
		}
	}
}

void Fluid_Sim_2D::applyBoundary(int var)
{
	switch (var)
	{
	default:
		break;
	case 0:
		boundaryConditionsUVelocity();
		break;
	case 1:
		boundaryConditionsVVelocity();
		break;
	case 2:
		boundaryConditionsPressure();
		break;
	case 3:
		//PlaceHOLDER
		break;
	case 4:
		boundaryConditionsDensity();
		break;
	case 5:
		boundaryConditionsDivergance();
		break;
	}
}

void Fluid_Sim_2D::boundaryConditionsUVelocity()
{
	for (int i = 1; i <= _N_y; i++)
	{
		set_actual(0, i, 0, -get_actual(1, i, 0));
		set_actual(_N_x + 1, i, 0, -get_actual(_N_x, i, 0));
	}
	for (int i = 1; i < _N_x; i++) {
		set_actual(i, 0, 0, get_actual(i, 1, 0));
		set_actual(i, _N_y + 1, 0, get_actual(i, _N_y, 0));
	}


	set_actual(0, 0, 0, 0.5*(get_actual(1, 0, 0) + get_actual(0, 1, 0)));

	set_actual(0, _N_y + 1, 0, 0.5*(get_actual(1, _N_y + 1, 0) + get_actual(0, _N_y, 0)));

	set_actual(_N_x + 1, 0, 0, 0.5*(get_actual(_N_x, 0, 0) + get_actual(_N_x + 1, 1, 0)));

	set_actual(_N_x + 1, _N_y + 1, 0, 0.5f * (get_actual(_N_x, _N_y + 1, 0) + get_actual(_N_x + 1, _N_y, 0)));
}

void Fluid_Sim_2D::boundaryConditionsVVelocity()
{
		int i;
		for (i = 1; i <= _N_y; i++)
		{
			set_actual(0, i, 1, get_actual(1, i, 1));
			set_actual(_N_x + 1, i, 1,  get_actual(_N_x, i, 1));
		}
		for (int i = 1; i < _N_x; i++) 
		{
			set_actual(i, 0, 1,  -get_actual(i, 1, 1));
			set_actual(i, _N_y + 1, 1,  -get_actual(i, _N_y, 1));
		}


		set_actual(0, 0, 1, 0.5*(get_actual(1, 0, 1) + get_actual(0, 1, 1)));

		set_actual(0, _N_y + 1, 1, 0.5*(get_actual(1, _N_y + 1, 1) + get_actual(0, _N_y, 1)));

		set_actual(_N_x + 1, 0, 1, 0.5*(get_actual(_N_x, 0, 1) + get_actual(_N_x + 1, 1, 1)));

		set_actual(_N_x + 1, _N_y + 1, 1, 0.5f * (get_actual(_N_x, _N_y + 1, 1) + get_actual(_N_x + 1, _N_y, 1)));
}

//PlaceHolder For temperature

void Fluid_Sim_2D::boundaryConditionsPressure()
{
	for (int i = 1; i <= _N_y; i++)
	{
		set_actual(0, i, 2, -get_actual(1, i, 2));
		set_actual(_N_x + 1, i, 2, -get_actual(_N_x, i, 2));
	}
	for (int i = 1; i < _N_x; i++) {
		set_actual(i, 0, 2, get_actual(i, 1, 2));
		set_actual(i, _N_y + 1, 2, get_actual(i, _N_y, 2));
	}


	set_actual(0, 0, 2, 0.5*(get_actual(1, 0, 2) + get_actual(0, 1, 2)));

	set_actual(0, _N_y + 1, 2, 0.5*(get_actual(1, _N_y + 1, 2) + get_actual(0, _N_y, 2)));

	set_actual(_N_x + 1, 0, 2, 0.5*(get_actual(_N_x, 0, 2) + get_actual(_N_x + 1, 1, 2)));

	set_actual(_N_x + 1, _N_y + 1, 2, 0.5f * (get_actual(_N_x, _N_y + 1, 2) + get_actual(_N_x + 1, _N_y, 2)));
}

void Fluid_Sim_2D::boundaryConditionsDensity()
{
		for (int i = 1; i <= _N_y; i++)
		{
			set_actual(0, i, 4, get_actual(1, i, 4));
			set_actual(_N_x + 1, i, 4, get_actual(_N_x, i, 4));
		}
		for (int i = 1; i < _N_x; i++) {
			set_actual(i, 0, 0, get_actual(i, 1, 4));
			set_actual(i, _N_y + 1, 4, get_actual(i, _N_y, 4));
		}


		set_actual(0, 0, 4, 0.5*(get_actual(1, 0, 4) + get_actual(0, 1, 4)));

		set_actual(0, _N_y + 1, 4, 0.5*(get_actual(1, _N_y + 1, 4) + get_actual(0, _N_y, 4)));

		set_actual(_N_x + 1, 0, 4, 0.5*(get_actual(_N_x, 0, 4) + get_actual(_N_x + 1, 1, 4)));

		set_actual(_N_x + 1, _N_y + 1, 4, 0.5f * (get_actual(_N_x, _N_y + 1, 4) + get_actual(_N_x + 1, _N_y, 4)));
}

void Fluid_Sim_2D::boundaryConditionsDivergance()
{
	for (int i = 1; i <= _N_y; i++)
	{
		set_gradient(0, i, get_gradient(1, i));
		set_gradient(_N_x + 1, i, get_gradient(_N_x, i));
	}
	for (int i = 1; i < _N_x; i++) {
		set_gradient(i, 0, get_gradient(i, 1));
		set_gradient(i, _N_y + 1, get_gradient(i, _N_y));
	}


	set_gradient(0, 0,  0.5*(get_gradient(1, 0) + get_gradient(0, 1)));

	set_gradient(0, _N_y + 1, 0.5*(get_gradient(1, _N_y + 1) + get_gradient(0, _N_y)));

	set_gradient(_N_x + 1, 0, 0.5*(get_gradient(_N_x, 0) + get_gradient(_N_x + 1, 1)));

	set_gradient(_N_x + 1, _N_y + 1,  0.5f * (get_gradient(_N_x, _N_y + 1) + get_gradient(_N_x + 1, _N_y)));
}

void Fluid_Sim_2D::addSourceData(float _dt, int var)
{
	for (int i = 0; i < width; i++) 
	{
		for (int j = 0; j < height; j++) 
		{
			set_0(i, j, var,  get_actual(i, j, var));
		}
	}

}



float Fluid_Sim_2D::get_gradient(int const & i, int const & j) const
{
	if (i > 0 && i < width && j>0 && j < width)
	{
		return _divergence[i + (j - 1)*width];
	}
	else return 0;
}

void Fluid_Sim_2D::set_gradient(int const & i, int const & j, float const & n)
{
	if (i > 0 && i < width && j>0 && j < width)
	{
		_divergence[i + (j - 1)*width] = n;
	}
}

float Fluid_Sim_2D::get_wall(int const & i, int const & j, int const & v) const
{
	return 0.0f;
}

void Fluid_Sim_2D::set_wall(int const & i, int const & j, int const & v, float const & n)
{
}


void Fluid_Sim_2D::vel_step(float _dt)
{
	addSourceData(_dt, 0);
	addSourceData(_dt, 1);
	diffusion_stam(_dt, 0, _diffusion_coefficient_velocity);
	diffusion_stam(_dt, 1, _diffusion_coefficient_velocity);
	projection_stam(_dt);
	advection_stam(_dt, 0);
	advection_stam(_dt, 1);
	projection_stam(_dt);
}


void Fluid_Sim_2D::dens_step(float _dt)
{
	addSourceData(_dt, 4);
	diffusion_stam(_dt, 4, _diffusion_coefficient_density);
	advection_stam(_dt, 4);
}

//Credit Muzkaw
sf::Color Fluid_Sim_2D::colorGradient(Image &color, float x)
{
	if (x>0.999) x = 0.999;
	if (x<0.001) x = 0.001;
	return color.getPixel((int)(x*color.getSize().x), 0);
}

// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
sf::Sprite Fluid_Sim_2D::getPressureSprite()
{
	for (int i = 1; i < width - 1; i++)
	{
		for (int j = 1; j < height - 1; j++)
		{
			if (get_actual(i, j, 2) > 0)
			{
				image.setPixel(i, j, colorGradient(posPressureColor, get_actual(i, j, 2) / scale));
			}
			else
			{
				image.setPixel(i, j, colorGradient(negPressureColor, -get_actual(i, j, 2) / scale));
			}
		}
	}
	texture.loadFromImage(image);
	return sprite;

}
//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
sf::Sprite Fluid_Sim_2D::getTemperatureSprite()
{
	for (int i(1); i < width - 1; i++)
		for (int j(1); j < height - 1; j++)
			image.setPixel(i, j, colorGradient(temperatureColor, get_actual(i, j, 3) / scale));

	texture.loadFromImage(image);
	return sprite;
}
//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
sf::Sprite Fluid_Sim_2D::getSpeedSprite()
{
	for (int i(1); i < width - 1; i++)
		for (int j(1); j < height - 1; j++)
			image.setPixel(i, j, colorGradient(speedColor, sqrt(pow(get_actual(i, j, 0), 2) + pow(get_actual(i, j, 1), 2)) / scale));

	texture.loadFromImage(image);
	return sprite;
}
//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
sf::Sprite Fluid_Sim_2D::getDensitySprite()
{
	for (int i(1); i < width - 1; i++)
		for (int j(1); j < height - 1; j++)
			image.setPixel(i, j, colorGradient(densityColor, get_actual(i, j, 4)*1.0f));

	texture.loadFromImage(image);
	return sprite;
}



//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
float Fluid_Sim_2D::get_actual(int const& i, int const& j, int const& v) const
{
	if (i > 0 && i < width && j>0 && j < width)
	{
		return _volume_data[i + (j - 1)*width](v);
	}
	else return 0;
}

//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
void Fluid_Sim_2D::set_actual(int const& i, int const& j, int const& v, float const& n)
{
	if (i > 0 && i < width && j>0 && j < width)
	{
		_volume_data[i + (j - 1)*width](v) = n;
	}
}
//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
float Fluid_Sim_2D::get_0(int const& i, int const& j, int const& v) const
{
	if (i > 0 && i < width && j>0 && j < width)
	{
		return _volume_data_0[i + (j - 1)*width](v);
	}
	else return 0;
}

//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
void Fluid_Sim_2D::set_0(int const& i, int const& j, int const& v, float const& n)
{
	if (i > 0 && i < width && j>0 && j < width)
	{
		_volume_data_0[i + (j - 1)*width](v) = n;
	}
}