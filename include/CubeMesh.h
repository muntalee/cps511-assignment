class CubeMesh
{
private:
	Vector3 center;
	float tx, ty, tz;			// Translatation Deltas
	float sfx, sfy, sfz;		// Scale Factors
	float angle;				// Angle around y-axis of cube coordinate system

	GLfloat (*vertices)[3];
	GLfloat (*quadNormals)[3];
	GLubyte (*quads)[4];

	// Material properties for drawing
	float mat_ambient[4];
	float mat_specular[4];
	float mat_diffuse[4];
	float mat_shininess[1];
private:
	

public:

	typedef std::pair<int, int> MaxMeshDim;
	
	CubeMesh();
	void CubeMesh::drawCubeMesh();
	void CubeMesh::setMaterial(Vector3 ambient, Vector3 diffuse, Vector3 specular, double shininess);
};



