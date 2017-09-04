#include "stdafx.h"
#include "Fluid_Sim_3D_Muzkaw.h"
//Credit Muzkaw
// https://www.youtube.com/user/Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
//Modified and extended by Odon Flanagan

Fluid_Sim_3D_Muzkaw::Fluid_Sim_3D_Muzkaw()
{
	width = 100;
	height = 150;
	breadth = 100;
	rho = 20;
	k = rho * 30;
	L = 1;
	buoyancyFactor = 0.001;
	ambientTemperature = 0;
	scale = 1;

	_N_x = width - 2;
	_N_y = height - 2;
	_N_z = breadth - 2;

	u.resize(width* height* breadth);
	v.resize(width* height* breadth);
	w.resize(width* height* breadth);
	u0.resize(width* height* breadth);
	v0.resize(width* height* breadth);
	w0.resize(width* height* breadth);
	p.resize(width* height* breadth);
	wall.resize(width* height* breadth);
	T.resize(width* height* breadth);
	T0.resize(width* height* breadth);
	density.resize(width* height* breadth);
	density0.resize(width* height* breadth);

	for (int i(0); i < width; i++)
	{
		for (int j(0); j < height; j++)
		{
			for (int k(0); k < height; k++)
			{
				for (int l = 0; l < 12; l++) {
					_set(i, j, k, l, 0);
				}
				_set(i, j, k, 8, ambientTemperature);
				_set(i, j, k, 9, ambientTemperature);
			}
		}
	}

	posPressureColor.loadFromFile("pressure+.jpg");
	negPressureColor.loadFromFile("pressure-.jpg");
	temperatureColor.loadFromFile("color.jpg");
	speedColor.loadFromFile("speed.jpg");
	densityColor.loadFromFile("density.jpg");
}
/////////////////////////////////////Fluid_Sim_3D_Muzkaw(width / scale, height / scale, 1.239, 101e3, 800, 1, scale);
Fluid_Sim_3D_Muzkaw::Fluid_Sim_3D_Muzkaw(float const& _width, float const& _height, float const& _breadth, float const& _rho, float const& _k, float const& _buoyancyFactor, float const& _L, float const& _scale)
{
	width = 100;
	height = 150;
	breadth = 100;
	rho = 20;
	k = rho * 30;
	L = 3 * _L / (width + height + breadth);
	buoyancyFactor = _buoyancyFactor;
	ambientTemperature = 0;
	scale = _scale;
	width = _width;
	height = _height;
	rho = _rho;
	k = _k;

	_N_x = width - 2;
	_N_y = height - 2;
	_N_z = breadth - 2;


	u.resize(width* height* breadth);
	v.resize(width* height* breadth);
	w.resize(width* height* breadth);
	u0.resize(width* height* breadth);
	v0.resize(width* height* breadth);
	w0.resize(width* height* breadth);
	p.resize(width* height* breadth);
	wall.resize(width* height* breadth);
	T.resize(width* height* breadth);
	T0.resize(width* height* breadth);
	density.resize(width* height* breadth);
	density0.resize(width* height* breadth);


	for (int i(0); i < width; i++)
	{
		for (int j(0); j < height; j++)
		{
			for (int k(0); k < breadth; k++)
			{
				for (int l = 0; l < 12; l++) {
					_set(i, j, k, l, 0);
				}
				//_set(i, j, k, 10, 0.1);
				_set(i, j, k, 8, ambientTemperature);
				_set(i, j, k, 9, ambientTemperature);

				if (i < 5)
				{
					_set(i, j, k, 7, 2);
				}
				else if (i >width - 5)
				{
					_set(i, j, k, 7, 3);
				}
				if (j < 5)
				{
					_set(i, j, k, 7, 4);
				}
				else if (j > width - 5)
				{
					_set(i, j, k, 7, 5);
				}
				if (k < 5)
				{
					_set(i, j, k, 7, 6);
				}
				else if (k > breadth - 5)
				{
					_set(i, j, k, 7, 7);
				}
			}
		}
	}


	posPressureColor.loadFromFile("pressure+.jpg");
	negPressureColor.loadFromFile("pressure-.jpg");
	temperatureColor.loadFromFile("color.jpg");
	speedColor.loadFromFile("speed.jpg");
	densityColor.loadFromFile("density.jpg");

	pScale = 3 * 60000;
	vScale = 3 * 200;
	tScale = 3 * 350;

	scale = _scale;
}

float Fluid_Sim_3D_Muzkaw::_get(int const& i, int const& j, int const& k, int const& var) const
{
	if (i > 0 && i < width && j>0 && j < width && k > 0 && k < breadth)
	{
		switch (var) {
			case 0:
				return u[i + width*j + width*height*k];
				// U actual
				break;
			case 1:
				return v[i + width*j + width*height*k];
				// V actual
				break;
			case 2:
				return w[i + width*j + width*height*k];
				// W actual
				break;
			case 3:
				return u0[i + width*j + width*height*k];
				// U 0
				break;
			case 4:
				return v0[i + width*j + width*height*k];
				// V 0
				break;
			case 5:
				return w0[i + width*j + width*height*k];
				// W 0
				break;
			case 6:
				return p[i + width*j + width*height*k];
				// P
				break;
			case 7:
				return wall[i + width*j + width*height*k];
				//WALL
				break;
			case 8:
				return T[i + width*j + width*height*k];
				// T ACTUAL
				break;
			case 9:
				return T0[i + width*j + width*height*k];
				//T 0
				break;
			case 10:
				return density[i + width*j + width*height*k];
				//DENSITY ACTUAL
				break;
			case 11:
				return density0[i + width*j + width*height*k];
				//DENSITY 0
				break;
		}
	}
	else return 0;
}

void Fluid_Sim_3D_Muzkaw::_set(int const& i, int const& j, int const& k, int const& var, float const& n)
{
	if (i > 0 && i < width && j>0 && j < width && k > 0 && k < breadth)
	{
		switch (var) {
		case 0:
			 u[i + width*j + width*height*k] = n;
			// U actual
			break;
		case 1:
			 v[i + width*j + width*height*k] = n;
			// V actual
			break;
		case 2:
			 w[i + width*j + width*height*k] = n;
			// W actual
			break;
		case 3:
			 u0[i + width*j + width*height*k] = n;
			// U 0
			break;
		case 4:
			 v0[i + width*j + width*height*k] = n;
			// V 0
			break;
		case 5:
			 w0[i + width*j + width*height*k] = n;
			// W 0
			break;
		case 6:
			 p[i + width*j + width*height*k] = n;
			// P
			break;
		case 7:
			 wall[i + width*j + width*height*k] = n;
			//WALL
			break;
		case 8:
			 T[i + width*j + width*height*k] = n;
			// T ACTUAL
			break;
		case 9:
			 T0[i + width*j + width*height*k] = n;
			//T 0
			break;
		case 10:
			 density[i + width*j + width*height*k] = n;
			//DENSITY ACTUAL
			break;
		case 11:
			 density0[i + width*j + width*height*k] = n;
			//DENSITY 0
			break;
		}
	}
}

void Fluid_Sim_3D_Muzkaw::Update(float const& dt)
{
	for (int i(2); i < width - 2; i++)
	{
		for (int j(2); j < height - 2; j++)
		{
			for (int k(2); k < breadth - 2; k++)
			{
				_set(i, j, k, 0, 
					_get(i, j, k, 0) - ((1 / rho)*(_get(i + 1, j , k, 6) - _get(i - 1, j, k, 6)) / (2.f*L))*dt);
				
				_set(i, j, k, 1,
					_get(i, j, k, 1) - ((1 / rho)*(_get(i, j + 1, k, 6) - _get(i, j - 1, k, 6)) / (2.f*L))*dt);
				
				_set(i, j, k, 2,
					_get(i, j, k, 2) - ((1 / rho)*(_get(i, j, k + 1, 6) - _get(i, j, k - 1, 6)) / (2.f*L))*dt);
				

				_set(i, j, k, 1,  _get(i, j, k , 1) - buoyancyFactor*(_get(i, j, k, 8) - ambientTemperature)*dt);

				if (_get(i, j, k, 0) != 0) {
					int t = 0;
				}

				if (_get(i, j, k, 7) == 1)
				{
					_set(i, j, k, 0 , 0);
					_set(i, j, k, 1, 0);
					_set(i, j, k, 2, 0);
					_set(i, j, k, 3, 0);
					_set(i, j, k, 4, 0);
					_set(i, j, k, 5, 0);
					_set(i, j, k, 8, ambientTemperature);
					_set(i, j, k, 9, ambientTemperature);
					_set(i, j, k, 10, 0);
					_set(i, j, k, 11, 0);
				}
				if (_get(i, j, k, 7) == 2)
				{
					_set(i, j, k, 0, _get(i + 1, j, k, 0));
					_set(i, j, k, 1, _get(i + 1, j, k, 1));
					_set(i, j, k, 2, _get(i + 1, j, k, 2));
					_set(i, j, k, 3, _get(i + 1, j, k, 3));
					_set(i, j, k, 4, _get(i + 1, j, k, 4));
					_set(i, j, k, 5, _get(i + 1, j, k, 5));
				}
				if (_get(i, j, k, 7) == 3)
				{
					_set(i, j, k, 0, _get(i - 1, j, k, 0));
					_set(i, j, k, 1, _get(i - 1, j, k, 1));
					_set(i, j, k, 2, _get(i - 1, j, k, 2));
					_set(i, j, k, 3, _get(i - 1, j, k, 3));
					_set(i, j, k, 4, _get(i - 1, j, k, 4));
					_set(i, j, k, 5, _get(i - 1, j, k, 5));
				}
				if (_get(i, j, k, 7) == 4)
				{
					_set(i, j, k, 0, _get(i, j + 1, k, 0));
					_set(i, j, k, 1, _get(i, j + 1, k, 1));
					_set(i, j, k, 2, _get(i, j + 1, k, 2));
					_set(i, j, k, 3, _get(i, j + 1, k, 3));
					_set(i, j, k, 4, _get(i, j + 1, k, 4));
					_set(i, j, k, 5, _get(i, j + 1, k, 5));
				}
				if (_get(i, j, k, 7) == 5)
				{
					_set(i, j, k, 0, _get(i, j - 1, k, 0));
					_set(i, j, k, 1, _get(i, j - 1, k, 1));
					_set(i, j, k, 2, _get(i, j - 1, k, 2));
					_set(i, j, k, 3, _get(i, j - 1, k, 3));
					_set(i, j, k, 4, _get(i, j - 1, k, 4));
					_set(i, j, k, 5, _get(i, j - 1, k, 5));
				}
				if (_get(i, j, k, 7) == 6)
				{
					_set(i, j, k, 0, _get(i, j, k + 1, 0));
					_set(i, j, k, 1, _get(i, j, k + 1, 1));
					_set(i, j, k, 2, _get(i, j, k + 1, 2));
					_set(i, j, k, 3, _get(i, j, k + 1, 3));
					_set(i, j, k, 4, _get(i, j, k + 1, 4));
					_set(i, j, k, 5, _get(i, j, k + 1, 5));
				}
				if (_get(i, j, k, 7) == 7)
				{
					_set(i, j, k, 0, _get(i, j, k - 1, 0));
					_set(i, j, k, 1, _get(i, j, k - 1, 1));
					_set(i, j, k, 2, _get(i, j, k - 1, 2));
					_set(i, j, k, 3, _get(i, j, k - 1, 3));
					_set(i, j, k, 4, _get(i, j, k - 1, 4));
					_set(i, j, k, 5, _get(i, j, k - 1, 5));
				}
			}
		}
	}
	////ADVECTION/////
	int i0, j0, i1, j1, k0, k1;
	float x, y, z, s0, t0, s1, t1, r0, r1;
	float dt0X = dt * _N_x;
	float dt0Y = dt * _N_y;
	float dt0Z = dt * _N_z;
	for (int i(1); i < width - 1; i++)
	{
		for (int j(1); j < height - 1; j++)
		{
			for (int k = 1; k < breadth - 1; k++)
			{
				_set(i, j, k, 6, _get(i, j , k , 6) - k * ((_get(i + 1, j, k, 0) - _get(i - 1, j, k,0)) / (2.f*L) + (_get(i, j + 1, k, 1) - _get(i, j - 1, k, 1)) / (2.f*L) + (_get(i, j, k - 1, 2) - _get(i, j, k -1, 2))  / (2.0f*L))*dt);

				if (_get(i, j, k, 7) != 1)
				{
					x = i - dt0X * _get(i, j, k, 0);
					y = j - dt0Y * _get(i, j, k, 1);
					z = k - dt0Z * _get(i, j, k, 2);

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

					if (z < 0.5) {
						z = 0.5;
					}
					else if (z > _N_z + 0.5) {
						z = _N_z + 0.5;
					}

					i0 = (int)x; //cell our subsection occupied last frame
					j0 = (int)y;  //cell our subsection occupied last frame
					k0 = (int)z;  //cell our subsection occupied last frame


					i1 = i0 + 1;
					j1 = j0 + 1;
					k1 = k0 + 1;

					s1 = x - i0;
					s0 = 1 - s1;

					t1 = y - j0;
					t0 = 1 - t1;

					r1 = z - k0;
					r0 = 1 - r1;

					//if (_get(i, j, k, 10) != 0) {
					//	int lm = 0;
					//	float _v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8, a, b, Xx;
					//	_v1 = s0 * t0 * r0 * _get(i0, j0, k0, 10);
					//	_v2 = s0 * t0 * r1 * _get(i0, j0, k1, 10);
					//	_v3 = s0 * t1 * r1 * _get(i0, j1, k1, 10);
					//	_v4 = s0 * t1 * r0 * _get(i0, j1, k0, 10);

					//	_v5 = s1 * t0 * r0 * _get(i1, j0, k0, 10);
					//	_v6 = s1 * t0 * r1 * _get(i1, j0, k1, 10);
					//	_v7 = s1 * t1 * r1 * _get(i1, j1, k1, 10);
					//	_v8 = s1 * t1 * r0 * _get(i1, j1, k0, 10);
					//	a = _v1 + _v2 + _v3 + _v4;
					//	if (a != 0) {
					//		b = _v5 + _v6 + _v7 + _v8;
					//		Xx = a + b;
					//	}
					//}

					_set(i, j, k, 3,
						s0
						* ((t0 * r0) * _get(i0, j0, k0, 3)
							+ (t1 * r0)* _get(i0, j1, k0, 3)
							+ (t0*r1) *  _get(i0, j0, k1, 3))
						+ s1
						* ((t0 * r0) * _get(i1, j0, k0, 3)
							+ (t1 * r0)* _get(i1, j1, k0, 3)
							+ (t0*r1) *  _get(i1, j0, k1, 3)));

					_set(i, j, k, 4,
						s0
						* ((t0 * r0) * _get(i0, j0, k0, 4)
							+ (t1 * r0)* _get(i0, j1, k0, 4)
							+ (t0*r1) *  _get(i0, j0, k1, 4))
						+ s1
						* ((t0 * r0) * _get(i1, j0, k0, 4)
							+ (t1 * r0)* _get(i1, j1, k0, 4)
							+ (t0*r1) *  _get(i1, j0, k1, 4)));

					_set(i, j, k, 5,
						s0
						* ((t0 * r0) * _get(i0, j0, k0, 5)
							+ (t1 * r0)* _get(i0, j1, k0, 5)
							+ (t0*r1) *  _get(i0, j0, k1, 5))
						+ s1
						* ((t0 * r0) * _get(i1, j0, k0, 5)
							+ (t1 * r0)* _get(i1, j1, k0, 5)
							+ (t0*r1) *  _get(i1, j0, k1, 5)));


					_set(i, j, k, 9,
						s0
						* ((t0 * r0) * _get(i0, j0, k0, 8)
							+ (t1 * r0)* _get(i0, j1, k0, 8)
							+ (t0*r1) *  _get(i0, j0, k1, 8))
						+ s1
						* ((t0 * r0) * _get(i1, j0, k0, 8)
							+ (t1 * r0)* _get(i1, j1, k0, 8)
							+ (t0*r1) *  _get(i1, j0, k1, 8)));

					_set(i, j, k, 11,
						s0
						* ((t0 * r0) * _get(i0, j0, k0, 10)
							+ (t1 * r0)* _get(i0, j1, k0, 10)
							+ (t0*r1) *  _get(i0, j0, k1, 10))
						+ s1
						* ((t0 * r0) * _get(i1, j0, k0, 10)
							+ (t1 * r0)* _get(i1, j1, k0, 10)
							+ (t0*r1) *  _get(i1, j0, k1, 10)));

				}
			}
		}
	}
	u = u0;
	v = v0;
	w = w0;
	T = T0;
	density = density0;
	for (int i = 0; i < (width* height* breadth); i++) {
		if (density[i] != 0) {
			bool b = false;
		}
	}
}

void Fluid_Sim_3D_Muzkaw::setExplosion()
{
	for (int i = -3; i < 3; i++)
	{
		for (int j = -3; j < 3; j++)
		{
			for (int k = -3; k < 3; k++)
			{

				_set((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 10, 1);
				//_set((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 8, 10000);
				//_set((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 1, 10000);
				//_set((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 0, 10000);
				//_set((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 2, 10000);
			}
		}
	}
}

void Fluid_Sim_3D_Muzkaw::createWind()
{
	for (int i = -6; i < 6; i++)
	{
		for (int j = -6; j < 6; j++)
		{
			for (int k = -6; k < 6; k++)
			{
				_set((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 10, 1);
				_set((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 1, 1000);
				_set((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 0, 1000);
				_set((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 2, 1000);
			}
		}
	}

}

sf::Color Fluid_Sim_3D_Muzkaw::getColour_general(int var, int x, int y, int z)
{
	switch (var)
	{
	default:
		break;
	case 0:
		return getSpeedColour(x, y, z);
		break;
	case 1:
		return getPressureColour(x, y, z);
		break;
	case 2:
		return getTemperatureColour(x, y, z);
		break;
	case 3:
		return getDensityColour(x, y, z);
		break;
	}
}

sf::Color Fluid_Sim_3D_Muzkaw::colorGradient(Image &color, float x)
{
	if (x>0.999) x = 0.999;
	if (x < 0.001)
	{
		return sf::Color(0, 0, 0, 0);
	}
	else {
		return color.getPixel((int)(x*color.getSize().x), 0);
	}

}

sf::Color Fluid_Sim_3D_Muzkaw::getPressureColour(int i, int j, int k)
{
	if (_get(i, j, k, 3) > 0)
	{
		return colorGradient(posPressureColor, _get(i, j, k, 6) / scale);
	}
	else
	{
		return colorGradient(negPressureColor, _get(i, j, k, 6) / scale);
	}
}

sf::Color Fluid_Sim_3D_Muzkaw::getTemperatureColour(int i, int j, int k)
{
	return colorGradient(temperatureColor, _get(i, j, k, 8) / scale);
}

sf::Color Fluid_Sim_3D_Muzkaw::getSpeedColour(int i, int j, int k)
{
	return colorGradient(speedColor, sqrt(pow(_get(i, j, k, 0), 2) + pow(_get(i, j, k, 1), 2) + pow(_get(i, j, k, 3), 2)) / scale);
}

sf::Color Fluid_Sim_3D_Muzkaw::getDensityColour(int i, int j, int k)
{
	return colorGradient(densityColor, _get(i, j, k, 10) / scale);
}

