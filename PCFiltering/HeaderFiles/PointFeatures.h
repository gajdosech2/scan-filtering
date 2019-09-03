struct PointFeatures
{
public:
	PointFeatures(float normal_x, float normal_y, float normal_z, float intensity, float depth)
	{
		this->normal_x = normal_x;
		this->normal_y = normal_y;
		this->normal_z = normal_z;
		this->intensity = intensity;
		this->depth = depth;
	}

	float normal_x;
	float normal_y;
	float normal_z;
	float intensity;
	float depth;
};