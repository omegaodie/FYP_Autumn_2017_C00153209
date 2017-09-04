#include "stdafx.h"
#include "Fluid_Sim_3D.h"

Fluid_Sim_3D::Fluid_Sim_3D(int w, int h, int b, int _scale) : width(w), height(h), breadth(b)
{

	_N_x = width - 2;
	_N_y = height - 2;
	_N_z = breadth - 2;

	scale = _scale;
	_size = width * height * breadth;

	//_volume_data.resize(width * height * breadth);
	//_volume_data_0.resize(width * height * breadth);
	//_walls.resize(width * height * breadth);
	//_divergence.resize(width * height * breadth);
	for (int i = 0; i < (width * height * breadth); i++) {
		_volume_data.push_back(VectorXf(6));
		_volume_data[i].setZero();
		_volume_data_0.push_back(VectorXf(6));
		_volume_data_0[i].setZero();
		_walls.push_back(0.0f);
		_divergence.push_back(0.0f);
	}
	std::cout << _volume_data.max_size() << std::endl;
	_pressure_added_this_frame = false;
	image.create(width, height, Color::Blue);
	texture.loadFromImage(image);
	sprite.setTexture(texture, true);
	sprite.setScale(scale, scale);
	posPressureColor.loadFromFile("pressure+.jpg");
	negPressureColor.loadFromFile("pressure-.jpg");
	temperatureColor.loadFromFile("color.jpg");
	speedColor.loadFromFile("speed.jpg");
	densityColor.loadFromFile("density.jpg");


	_diffusion_coefficient_pressure = 8;
	_diffusion_coefficient_density = 100;
	_diffusion_coefficient_velocity = 6;

	for (int i = -5; i < 5; i++)
	{
		for (int j = -5; j < 5; j++)
		{
			for (int k = -5; k < 5; k++)
			{
				set_actual((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 5, 5);
				set_actual((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 0, 5000);
				set_actual((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 1, 5000);
				set_actual((width / 2) + i, (height / 2) + j, (breadth / 2) + k, 2, 5000);
			}
		}
	}


}

void Fluid_Sim_3D::_update_stam(float _dt)
{
	vel_step(_dt);
	dens_step(_dt);
}

void Fluid_Sim_3D::diffusion_stam(float _dt, int var, float viscosity)
{
	float _a = _dt *  viscosity * _N_x * _N_y * _N_z;
	////DIFFUSION/////
	float _value;
	for (int k = 0; k < 2; k++)
	{
		for (int i = 1; i <= _N_x; i++)
		{
			for (int j = 1; j <= _N_y; j++)
			{
				for (int k = 1; k <= _N_z; k++)
				{
					set_0(i, j, k, var, (get_actual(i, j, k, var)
						+ _a
						* (get_0(i - 1, j, k, var)
							+ get_0(i + 1, j, k,var)
							+ get_0(i, j - 1, k,var)
							+ get_0(i, j + 1, k,var)
							+ get_0(i, j, k - 1, var)
							+ get_0(i, j, k + 1, var)
							))
						/ (1 + 6 * _a));
				}
			}
		}
		applyBoundary(var);
	}

}

void Fluid_Sim_3D::advection_stam(float _dt, int var)
{

	////ADVECTION/////
	int i0, j0, i1, j1, k0, k1;

	float x, y, z, s0, t0, s1, t1, r0, r1;
	float dt0X = _dt * _N_x;
	float dt0Y = _dt * _N_y;
	float dt0Z = _dt * _N_z;
	for (int i = 1; i <= _N_x; i++)
	{
		for (int j = 1; j <= _N_y; j++)
		{
			for (int k = 1; k <= _N_y; k++)
			{
				x = i - dt0X * get_actual(i, j, k, 0);
				y = j - dt0Y * get_actual(i, j, k, 1);
				z = k - dt0Z * get_actual(i, j, k, 2);

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

				set_actual(i, j, k, var,
					s0
					* ((t0 * r0) * get_0(i0, j0, k0, var)
						+ (t1 * r0)* get_0(i0, j1, k0, var)
						+ (t0*r1) *  get_0(i0, j0, k1, var))
					+ s1
					* ((t0 * r0) * get_0(i1, j0, k0, var)
						+ (t1 * r0)* get_0(i1, j1, k0, var)
						+ (t0*r1) *  get_0(i1, j0, k1, var)));
			}
		}
	}
	applyBoundary(var);
}

void Fluid_Sim_3D::projection_stam(float _dt)
{
	int i, j, k;
	float h = 1.0 / ((_N_y + _N_y +_N_z) / 3);




	for (i = 1; i <= _N_x; i++) {
		for (j = 1; j <= _N_y; j++) 
		{
			for (int k = 1; k <= _N_z; k++)
			{
				set_gradient(i, j, k, -0.5f*h*(get_actual(i + 1, j, k, 0) - get_actual(i - 1, j, k, 0) +
					get_actual(i, j + 1, k, 1) - get_actual(i, j - 1, k, 1)
					+ get_actual(i, j, k + 1, 2) - get_actual(i, j - 1, k - 1, 2)));
				set_actual(i, j, k, 3, 0);
			}
		}
	}
	applyBoundary(6);
	applyBoundary(3);
	for (k = 0; k<2; k++)
	{
		for (i = 1; i <= _N_x; i++)
		{
			for (j = 1; j <= _N_y; j++)
			{
				for (int k = 1; k <= _N_z; k++)
				{
					set_actual(i, j, k, 3, (get_gradient(i, j, k) + get_actual(i - 1, j, k, 3) + get_actual(i + 1, j, k, 3) +
						get_actual(i, j - 1, k, 3) + get_actual(i, j + 1, k, 3)
						+ get_actual(i, j, k - 1, 3) + get_actual(i, j, k +1, 3)) / 6);
				}
			}
		}
		applyBoundary(3);
	}
	for (i = 1; i <= _N_x; i++)
	{
		for (j = 1; j <= _N_y; j++)
		{
			for (int k = 1; k <= _N_z; k++)
			{
				set_actual(i, j, k, 0, 0.5*(get_actual(i + 1, j, k, 3) - get_actual(i - 1, j, k, 3)) / h);
				set_actual(i, j, k, 1, 0.5*(get_actual(i, j + 1, k, 3) - get_actual(i, j - 1, k, 3)) / h);
				set_actual(i, j, k, 1, 0.5*(get_actual(i, j , k + 1, 3) - get_actual(i, j, k - 1, 3)) / h);
			}
		}
	}
	applyBoundary(0);
	applyBoundary(1);
	applyBoundary(2);
}

void Fluid_Sim_3D::addDensity(int x, int y, int z)
{
	for (int i = (x - 20); i < (x + 20); i++) 
	{
		for (int j = (y - 20); j < (y + 20); j++) 
		{
			for (int k = (z - 20); k < (z + 20); k++) 
			{
				if (i > 0 && i < width && j > 0 && j < height && k > 0 && k < breadth)
				{
					float d = std::sqrt(std::pow(i - x, 2) + std::pow(j - y, 2) + std::pow(k - z, 2)); //this a proven thing btw
					if (d < 20 / 4) {
						set_actual(i, j, k, 5, get_actual(i, j, k, 5) + 5);
					}
				}
			}
		}
	}
}

void Fluid_Sim_3D::addPressure(int x, int y, int z)
{
	//for (int i = (x - 5); i < (x + 5); i++) {
	//	for (int j = (y - 5); j < (y + 5); j++) {
	//		if (i > 0 && i < width && j > 0 && j < height)
	//		{
	//			set_actual(i, j, 2, get_actual(i, j, 2) + 100.0f);
	//			_pressure_added_this_frame = true;
	//		}
	//	}
	//}
}

void Fluid_Sim_3D::addV(int x, int y, int z)
{
	//for (int i = (x - 20); i < (x + 20); i++) {
	//	for (int j = (y - 20); j < (y + 20); j++) {
	//		if (i > 0 && i < width && j > 0 && j < height)
	//		{
	//			float d = std::sqrt(std::pow(i - x, 2) + std::pow(j - y, 2));
	//			if (d < 20 / 4)
	//			{
	//				set_actual(i, j, 1, get_actual(i, j, 1) + 0.005f);
	//			}
	//		}
	//	}
	//}
}

void Fluid_Sim_3D::addU(int x, int y, int z)
{
	//for (int i = (x - 20); i < (x + 20); i++) {
	//	for (int j = (y - 20); j < (y + 20); j++) {
	//		if (i > 0 && i < width && j > 0 && j < height)
	//		{
	//			float d = std::sqrt(std::pow(i - x, 2) + std::pow(j - y, 2));
	//			if (d < 20 / 4)
	//			{
	//				set_actual(i, j, 0, get_actual(i, j, 0) + 0.005f);
	//			}
	//		}
	//	}
	//}
}

void Fluid_Sim_3D::addVelocity(int x, int y, int z)
{
	for (int i = (x - 8); i < (x + 8); i++) {
		for (int j = (y - 8); j < (y + 8); j++) 
		{
			for (int k = (z - 8); k < (z + 8); k++) {
				if (i > 0 && i < width && j > 0 && j < height && k > 0 && k < breadth)
				{
					float d = std::sqrt(std::pow(i - x, 2) + std::pow(j - y, 2) + std::pow(k - z, 2)); //this a proven thing btw
					if (d < 8 / 4)
					{
						set_actual(i, j, k, 0, get_actual(i, j, k, 0) + 1000000000.0f);
						set_actual(i, j, k, 1, get_actual(i, j, k, 1) + 1000000000.0f);
						set_actual(i, j, k, 1, get_actual(i, j, k, 2) + 1000000000.0f);
					}
				}
			}
		}
	}
}

void Fluid_Sim_3D::applyBoundary(int var)
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
		boundaryConditionsWVelocity();
		break;
	case 3:
		boundaryConditionsPressure();
		break;
	case 4:
		//PLACEHOLDER
		break;
	case 5:
		boundaryConditionsDensity();
		break;
	case 6:
		boundaryConditionsDivergance();
		break;
	}
}

void Fluid_Sim_3D::boundaryConditionsUVelocity()
{
	//Y X boundary, z is static
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (x == 0) 
			{
				if (y == 0) 
				{
					set_actual(x, y, 0, 0, get_actual(1, 1, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(1, 1, _N_z, 0));
				}
				else if (y == height - 1)
				{
					set_actual(x, y, 0, 0, get_actual(1, _N_y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(1, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, y, 0, 0, get_actual(1, y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(1, y, _N_z, 0));
				}
			}
			else if (x == width - 1)
			{
				if (y == 0)
				{
					set_actual(x, y, 0, 0, get_actual(_N_x, 1, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(_N_x, 1, _N_z, 0));
				}
				else if (y == height - 1)
				{
					set_actual(x, y, 0, 0, get_actual(_N_x, _N_y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(_N_x, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, y, 0, 0, get_actual(_N_x, y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(_N_x, y, _N_z, 0));
				}
			}
			else 
			{
				set_actual(x, y, 0, 0, get_actual(x, y, 1, 0));
				set_actual(x, y, _N_x + 1, 0, get_actual(x, y, _N_x, 0));
			}
		}
	}


	//Z X boundary, y is static
	for (int x = 0; x < width; x++)
	{
		for (int z = 0; z < breadth; z++)
		{
			if (x == 0)
			{
				if (z == 0)
				{
					set_actual(x, 0, z, 0, get_actual(1, 1, 1, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(1, _N_y, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(x, 0, z, 0, get_actual(1, 1, _N_z, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(1, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, 0, z, 0, get_actual(1, 0, 1, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(1, _N_y, z, 0));
				}
			}
			else if (x == width - 1)
			{
				if (z == 0)
				{
					set_actual(x, 0, z, 0, get_actual(_N_x, 1, 1, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(_N_x, _N_y, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(x, 0, z, 0, get_actual(_N_x, 1, _N_z, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(_N_x, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, 0, z, 0, get_actual(_N_x, 0, 1, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(_N_x, _N_y, z, 0));
				}
			}
			else
			{
				set_actual(x, 0, z, 0, get_actual(x, 1, z, 0));
				set_actual(x, _N_y, z, 0, get_actual(x, _N_y, z, 0));
			}
		}
	}


	//Y Z boundary, x is static
	for (int y = 0; y < height; y++)
	{
		for (int z = 0; z < breadth; z++)
		{
			if (y == 0)
			{
				if (z == 0)
				{
					set_actual(0, 0, z, 0, -get_actual(1, 1, 1, 0));
					set_actual(_N_x + 1, y, z, 0, -get_actual(_N_x, 1, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(0, 0, z, 0, -get_actual(1, 1, _N_z, 0));
					set_actual(_N_x + 1, y, z, 0, -get_actual(_N_x, 1, _N_z, 0));
				}
				else {
					set_actual(0, 0, z, 0, -get_actual(1, 0, 1, 0));
					set_actual(_N_x + 1, y, z, 0, -get_actual(_N_x, 1, z, 0));
				}
			}
			else if (y == width - 1)
			{
				if (z == 0)
				{
					set_actual(0, y, z, 0, -get_actual(1, _N_y, 1, 0));
					set_actual(_N_x + 1, y, z, 0, -get_actual(_N_x, _N_y, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(0, y, z, 0, -get_actual(1, _N_y, _N_z, 0));
					set_actual(_N_x + 1, _N_y + 1, z, 0, -get_actual(_N_x, _N_y, _N_z, 0));
				}
				else {
					set_actual(0, y, z, 0, -get_actual(1, _N_y, 1, 0));
					set_actual(_N_x + 1, _N_y + 1, z, 0, -get_actual(_N_x, _N_y, z, 0));
				}
			}
			else
			{
				set_actual(0, y, z, 0, -get_actual(1, y, z, 0));
				set_actual(_N_x + 1, y, z, 0, -get_actual(_N_x, y, z, 0));
			}
		}
	}





	//set_actual(0, 0, 0, 0.5*(get_actual(1, 0, 0) + get_actual(0, 1, 0)));

	//set_actual(0, _N_y + 1, 0, 0.5*(get_actual(1, _N_y + 1, 0) + get_actual(0, _N_y, 0)));

	//set_actual(_N_x + 1, 0, 0, 0.5*(get_actual(_N_x, 0, 0) + get_actual(_N_x + 1, 1, 0)));

	//set_actual(_N_x + 1, _N_y + 1, 0, 0.5f * (get_actual(_N_x, _N_y + 1, 0) + get_actual(_N_x + 1, _N_y, 0)));
}

void Fluid_Sim_3D::boundaryConditionsVVelocity()
{
	//Y X boundary, z is static
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (x == 0)
			{
				if (y == 0)
				{
					set_actual(x, y, 0, 0, get_actual(1, 1, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(1, 1, _N_z, 0));
				}
				else if (y == height - 1)
				{
					set_actual(x, y, 0, 0, get_actual(1, _N_y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(1, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, y, 0, 0, get_actual(1, y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(1, y, _N_z, 0));
				}
			}
			else if (x == width - 1)
			{
				if (y == 0)
				{
					set_actual(x, y, 0, 0, get_actual(_N_x, 1, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(_N_x, 1, _N_z, 0));
				}
				else if (y == height - 1)
				{
					set_actual(x, y, 0, 0, get_actual(_N_x, _N_y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(_N_x, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, y, 0, 0, get_actual(_N_x, y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, get_actual(_N_x, y, _N_z, 0));
				}
			}
			else
			{
				set_actual(x, y, 0, 0, get_actual(x, y, 1, 0));
				set_actual(x, y, _N_x + 1, 0, get_actual(x, y, _N_x, 0));
			}
		}
	}


	//Z X boundary, y is static
	for (int x = 0; x < width; x++)
	{
		for (int z = 0; z < breadth; z++)
		{
			if (x == 0)
			{
				if (z == 0)
				{
					set_actual(x, 0, z, 0, -get_actual(1, 1, 1, 0));
					set_actual(x, _N_y + 1, z, 0, -get_actual(1, _N_y, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(x, 0, z, 0, -get_actual(1, 1, _N_z, 0));
					set_actual(x, _N_y + 1, z, 0, -get_actual(1, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, 0, z, 0, -get_actual(1, 0, 1, 0));
					set_actual(x, _N_y + 1, z, 0, -get_actual(1, _N_y, z, 0));
				}
			}
			else if (x == width - 1)
			{
				if (z == 0)
				{
					set_actual(x, 0, z, 0, -get_actual(_N_x, 1, 1, 0));
					set_actual(x, _N_y + 1, z, 0, -get_actual(_N_x, _N_y, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(x, 0, z, 0, -get_actual(_N_x, 1, _N_z, 0));
					set_actual(x, _N_y + 1, z, 0, -get_actual(_N_x, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, 0, z, 0, -get_actual(_N_x, 0, 1, 0));
					set_actual(x, _N_y + 1, z, 0, -get_actual(_N_x, _N_y, z, 0));
				}
			}
			else
			{
				set_actual(x, 0, z, 0, -get_actual(x, 1, z, 0));
				set_actual(x, _N_y, z, 0, -get_actual(x, _N_y, z, 0));
			}
		}
	}


	//Y Z boundary, x is static
	for (int y = 0; y < height; y++)
	{
		for (int z = 0; z < breadth; z++)
		{
			if (y == 0)
			{
				if (z == 0)
				{
					set_actual(0, 0, z, 0, get_actual(1, 1, 1, 0));
					set_actual(_N_x + 1, y, z, 0, get_actual(_N_x, 1, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(0, 0, z, 0, get_actual(1, 1, _N_z, 0));
					set_actual(_N_x + 1, y, z, 0, get_actual(_N_x, 1, _N_z, 0));
				}
				else {
					set_actual(0, 0, z, 0, get_actual(1, 0, 1, 0));
					set_actual(_N_x + 1, y, z, 0, get_actual(_N_x, 1, z, 0));
				}
			}
			else if (y == width - 1)
			{
				if (z == 0)
				{
					set_actual(0, y, z, 0, get_actual(1, _N_y, 1, 0));
					set_actual(_N_x + 1, y, z, 0, get_actual(_N_x, _N_y, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(0, y, z, 0, get_actual(1, _N_y, _N_z, 0));
					set_actual(_N_x + 1, _N_y + 1, z, 0, get_actual(_N_x, _N_y, _N_z, 0));
				}
				else {
					set_actual(0, y, z, 0, get_actual(1, _N_y, 1, 0));
					set_actual(_N_x + 1, _N_y + 1, z, 0, get_actual(_N_x, _N_y, z, 0));
				}
			}
			else
			{
				set_actual(0, y, z, 0, get_actual(1, y, z, 0));
				set_actual(_N_x + 1, y, z, 0, get_actual(_N_x, y, z, 0));
			}
		}
	}


}

void Fluid_Sim_3D::boundaryConditionsWVelocity()
{
	//Y X boundary, z is static
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			if (x == 0)
			{
				if (y == 0)
				{
					set_actual(x, y, 0, 0, -get_actual(1, 1, 1, 0));
					set_actual(x, y, _N_z + 1, 0, -get_actual(1, 1, _N_z, 0));
				}
				else if (y == height - 1)
				{
					set_actual(x, y, 0, 0, -get_actual(1, _N_y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, -get_actual(1, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, y, 0, 0, -get_actual(1, y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, -get_actual(1, y, _N_z, 0));
				}
			}
			else if (x == width - 1)
			{
				if (y == 0)
				{
					set_actual(x, y, 0, 0, -get_actual(_N_x, 1, 1, 0));
					set_actual(x, y, _N_z + 1, 0, -get_actual(_N_x, 1, _N_z, 0));
				}
				else if (y == height - 1)
				{
					set_actual(x, y, 0, 0, -get_actual(_N_x, _N_y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, -get_actual(_N_x, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, y, 0, 0, -get_actual(_N_x, y, 1, 0));
					set_actual(x, y, _N_z + 1, 0, -get_actual(_N_x, y, _N_z, 0));
				}
			}
			else
			{
				set_actual(x, y, 0, 0, -get_actual(x, y, 1, 0));
				set_actual(x, y, _N_x + 1, 0, -get_actual(x, y, _N_x, 0));
			}
		}
	}


	//Z X boundary, y is static
	for (int x = 0; x < width; x++)
	{
		for (int z = 0; z < breadth; z++)
		{
			if (x == 0)
			{
				if (z == 0)
				{
					set_actual(x, 0, z, 0, get_actual(1, 1, 1, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(1, _N_y, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(x, 0, z, 0, get_actual(1, 1, _N_z, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(1, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, 0, z, 0, get_actual(1, 0, 1, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(1, _N_y, z, 0));
				}
			}
			else if (x == width - 1)
			{
				if (z == 0)
				{
					set_actual(x, 0, z, 0, get_actual(_N_x, 1, 1, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(_N_x, _N_y, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(x, 0, z, 0, get_actual(_N_x, 1, _N_z, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(_N_x, _N_y, _N_z, 0));
				}
				else {
					set_actual(x, 0, z, 0, get_actual(_N_x, 0, 1, 0));
					set_actual(x, _N_y + 1, z, 0, get_actual(_N_x, _N_y, z, 0));
				}
			}
			else
			{
				set_actual(x, 0, z, 0, get_actual(x, 1, z, 0));
				set_actual(x, _N_y, z, 0, get_actual(x, _N_y, z, 0));
			}
		}
	}


	//Y Z boundary, x is static
	for (int y = 0; y < height; y++)
	{
		for (int z = 0; z < breadth; z++)
		{
			if (y == 0)
			{
				if (z == 0)
				{
					set_actual(0, 0, z, 0, get_actual(1, 1, 1, 0));
					set_actual(_N_x + 1, y, z, 0, get_actual(_N_x, 1, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(0, 0, z, 0, get_actual(1, 1, _N_z, 0));
					set_actual(_N_x + 1, y, z, 0, get_actual(_N_x, 1, _N_z, 0));
				}
				else {
					set_actual(0, 0, z, 0, get_actual(1, 0, 1, 0));
					set_actual(_N_x + 1, y, z, 0, get_actual(_N_x, 1, z, 0));
				}
			}
			else if (y == width - 1)
			{
				if (z == 0)
				{
					set_actual(0, y, z, 0, get_actual(1, _N_y, 1, 0));
					set_actual(_N_x + 1, y, z, 0, get_actual(_N_x, _N_y, 1, 0));
				}
				else if (z == breadth - 1)
				{
					set_actual(0, y, z, 0, get_actual(1, _N_y, _N_z, 0));
					set_actual(_N_x + 1, _N_y + 1, z, 0, get_actual(_N_x, _N_y, _N_z, 0));
				}
				else {
					set_actual(0, y, z, 0, get_actual(1, _N_y, 1, 0));
					set_actual(_N_x + 1, _N_y + 1, z, 0, get_actual(_N_x, _N_y, z, 0));
				}
			}
			else
			{
				set_actual(0, y, z, 0, get_actual(1, y, z, 0));
				set_actual(_N_x + 1, y, z, 0, get_actual(_N_x, y, z, 0));
			}
		}
	}

}

//PlaceHolder For temperature
void Fluid_Sim_3D::boundaryConditionsPressure()
{
}

void Fluid_Sim_3D::boundaryConditionsDensity()
{
}

void Fluid_Sim_3D::boundaryConditionsDivergance()
{
}

void Fluid_Sim_3D::addSourceData(float _dt, int var)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < height; k++)
			{
				set_0(i, j, k, var, get_actual(i, j, k, var));
			}
		}
	}

}

float Fluid_Sim_3D::get_gradient(int const & i, int const & j, int const& k ) const
{
	if (i > 0 && i < width && j>0 && j < width && k > 0 && k < breadth)
	{
		return _divergence[i + width*j + width*height*k];
	}
	else return 0;
}

void Fluid_Sim_3D::set_gradient(int const & i, int const & j, int const& k, float const & n)
{
	if (i > 0 && i < width && j>0 && j < width && k > 0 && k < breadth)
	{
		_divergence[i + width*j + width*height*k] = n;
	}
}

float Fluid_Sim_3D::get_wall(int const & i, int const & j, int const& k,  int const & v) const
{
	return 0.0f;
}

void Fluid_Sim_3D::set_wall(int const & i, int const & j, int const& k,int const & v, float const & n)
{
}

void Fluid_Sim_3D::vel_step(float _dt)
{
	addSourceData(_dt, 0);
	addSourceData(_dt, 1);
	addSourceData(_dt, 2);
	diffusion_stam(_dt, 0, _diffusion_coefficient_velocity);
	diffusion_stam(_dt, 1, _diffusion_coefficient_velocity);
	diffusion_stam(_dt, 2, _diffusion_coefficient_velocity);
	projection_stam(_dt);
	advection_stam(_dt, 0);
	advection_stam(_dt, 1);
	advection_stam(_dt, 2);
	projection_stam(_dt);
}

void Fluid_Sim_3D::dens_step(float _dt)
{
	addSourceData(_dt, 5);
	diffusion_stam(_dt, 5, _diffusion_coefficient_density);
	advection_stam(_dt, 5);
}

sf::Color Fluid_Sim_3D::getColour_general(int var, int x, int y, int z)
{
	//sf::Color _c;
	float _rho;
	switch (var)
	{
	default:
		break;
	case 0:
		return getSpeedColour(x,y,z);
		break;
	case 1:
		return getSpeedColour(x, y, z);
		break;
	case 2:
		return getPressureColour(x, y, z);
		break;
	case 3:
		//PlaceHOLDER
		return getTemperatureColour(x, y, z);
		break;
	case 4:

		//if (x == (width / 2) && y == (height / 2) && z == (breadth / 2)) 
		//{
		//	std::cout << get_actual(x, y, z, 5) << std::endl;
		//	getDensityColour(x, y, z);
		//}
		return getDensityColour(x, y, z);
		break;
	case 5:
		return getDensityColour(x, y, z);
		break;
	}
}

//Credit Muzkaw
sf::Color Fluid_Sim_3D::colorGradient(Image &color, float x)
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

// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
//Modified by Odon Flanagan
sf::Color Fluid_Sim_3D::getPressureColour(int i, int j, int k)
{
			if (get_actual(i, j, k, 3) > 0)
			{
				return colorGradient(posPressureColor, get_actual(i, j, k, 3) / scale);
			}
			else
			{
				return colorGradient(negPressureColor, get_actual(i, j, k, 3) / scale);
			}
}
//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
sf::Color Fluid_Sim_3D::getTemperatureColour(int i, int j, int k)
{
	return colorGradient(temperatureColor, get_actual(i, j, k, 4) / scale);
}
//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
sf::Color Fluid_Sim_3D::getSpeedColour(int i, int j, int k)
{
	return colorGradient(speedColor, sqrt(pow(get_actual(i, j, k, 0), 2) + pow(get_actual(i, j, k, 1), 2) + pow(get_actual(i, j, k, 3), 2)) / scale);
}
//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
sf::Color Fluid_Sim_3D::getDensityColour(int i, int j, int k)
{
	return colorGradient(densityColor, get_actual(i, j, k, 5) / scale);
}


float Fluid_Sim_3D::get_actual(int const& i, int const& j, int const& k, int const& v) const
{
	if (i > 0 && i < width && j>0 && j < width && k > 0 && k < breadth)
	{
		return _volume_data[i + width*j + width*height*k](v);
	}
	else return 0;
}


void Fluid_Sim_3D::set_actual(int const& i, int const& j, int const& k, int const& v, float const& n)
{
	if (i > 0 && i < width && j>0 && j < width && k > 0 && k < breadth)
	{
		_volume_data[i + width*j + width*height*k](v) = n;
	}
}
//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
float Fluid_Sim_3D::get_0(int const& i, int const& j, int const& k, int const& v) const
{
	if (i > 0 && i < width && j>0 && j < width && k > 0 && k < breadth)
	{
		return _volume_data_0[i + width*j + width*height*k](v);
	}
	else return 0;
}

//Credit Muzkaw
// https://drive.google.com/file/d/0B2voedb-erQsUnFQeU5mMWVGOEU/view?usp=sharing
void Fluid_Sim_3D::set_0(int const& i, int const& j, int const& k, int const& v, float const& n)
{
	if (i > 0 && i < width && j>0 && j < width && k > 0 && k < breadth)
	{
		_volume_data_0[i + width*j + width*height*k](v) = n;
	}
}