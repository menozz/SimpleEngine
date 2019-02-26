#include "olcConsoleGameEngine.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#define PI 3.14159

using namespace std;

struct vec3d {
	float x, y, z;
};

struct vertex {
	int v1, v2, v3;
};

struct triangle {
	vec3d p[3];
};

struct mesh {
	vector<triangle> tris;
};

struct mat4x4 {
	float m[4][4] = { 0 };
};

class olcEngine3D : public olcConsoleGameEngine
{
public:
	olcEngine3D()
	{
		m_sAppName = L"Example";
	}

	void InitMesh(mesh meshData)
	{
		meshCube = meshData;
	}

private:
	mesh meshCube;
	mat4x4 mathProj;
	float fTheta = 0.0f;

	void initMeshCube() {
		meshCube.tris = {

			// SOUTH
			{0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f},

			// EAST
			{1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 1.0f},

			// NORTH
			{1.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f},
			{1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f},

			// WEST
			{0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f},

			// TOP
			{0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f},
			{0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f},

			// BOTTOM
			{1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f},
		};
	}



	void initMatrixProjection()
	{
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * PI);

		mathProj.m[0][0] = fAspectRatio * fFovRad;
		mathProj.m[1][1] = fFovRad;
		mathProj.m[2][2] = fFar / (fFar - fNear);
		mathProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		mathProj.m[2][3] = 1.0f;
		mathProj.m[3][3] = 0.0f;
	}

	void ScaleIntoView(triangle &triProjected)
	{
		triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
		triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
		triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

		triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
		triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
		triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
		triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
		triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
		triProjected.p[2].y *= 0.5f * (float)ScreenHeight();
	}

	void Draw(float &fElapsedTime)
	{
		mat4x4 matRotZ, matRotX;
		fTheta += 1.0f * fElapsedTime;

		// Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		// Rotation X
		const auto fTh = fTheta * 0.5f;
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTh);
		matRotX.m[1][2] = sinf(fTh);
		matRotX.m[2][1] = -sinf(fTh);
		matRotX.m[2][2] = cosf(fTh);
		matRotX.m[3][3] = 1;


		for (auto tri : meshCube.tris)
		{
			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			multiply_matrix_vector(tri.p[0], triRotatedZ.p[0], matRotZ);
			multiply_matrix_vector(tri.p[1], triRotatedZ.p[1], matRotZ);
			multiply_matrix_vector(tri.p[2], triRotatedZ.p[2], matRotZ);

			multiply_matrix_vector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			multiply_matrix_vector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			multiply_matrix_vector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

			multiply_matrix_vector(triTranslated.p[0], triProjected.p[0], mathProj);
			multiply_matrix_vector(triTranslated.p[1], triProjected.p[1], mathProj);
			multiply_matrix_vector(triTranslated.p[2], triProjected.p[2], mathProj);

			// Scale into view
			ScaleIntoView(triProjected);

			DrawTriangle(
				triProjected.p[0].x, triProjected.p[0].y,
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y,
				PIXEL_SOLID, FG_WHITE);
		}
	}

	static void multiply_matrix_vector(vec3d &i, vec3d &o, mat4x4 &m) {
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z* m.m[2][0] + m.m[3][0];
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z* m.m[2][1] + m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z* m.m[2][2] + m.m[3][2];
		const auto w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z* m.m[2][3] + m.m[3][3];

		if (w != 0)
		{
			o.x /= w, o.y /= w, o.z /= w;
		}
	}

public:


	bool OnUserCreate() override
	{
		//	initMeshPyr();
		initMatrixProjection();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		Draw(fElapsedTime);
		return true;
	}
};

mesh CreateMeshPyr() {

	mesh mesh_data;
	mesh_data.tris =
	{

		// SOUTH
		{0.0f, 0.0f, 0.0f,   0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f},

		// EAST
		{1.0f, 0.0f, 0.0f,   0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 1.0f},

		// NORTH
		{0.0f, 0.0f, 1.0f,   0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 1.0f},

		// WEST
		{0.0f, 0.0f, 1.0f,   0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 0.0f},

		// BOTTOM
		{0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f},
	};

	return mesh_data;
}
string ExePath();
mesh ReadFromFile()
{
	ifstream source;

	vector<vertex> vertices;
	vector<vec3d> coords;
	const string path = ExePath();
	//var file = sprintf(path, "%s ");
	source.open(path + "\\cyl.obj", ios_base::in);
	for (string line; getline(source, line); )
	{
		istringstream in(line);

		string type;
		in >> type;

		if (type == "v")
		{
			vec3d vec{};
			in >> vec.x >> vec.y >> vec.z;
			coords.push_back(vec);
		}

		else if (type == "f") {
			vertex vex{};
			in >> vex.v1 >> vex.v2 >> vex.v3;
			vertices.push_back(vex);
		}
	}

	source.close();
	mesh m{};

	for (auto item : vertices)
	{
		triangle tri{};
		tri.p[0] = coords[item.v1 - 1];
		tri.p[1] = coords[item.v2 - 1];
		tri.p[2] = coords[item.v3 - 1];
		m.tris.push_back(tri);
	}

	return m;
}

string ExePath() {
	TCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	wstring wStr = buffer;
	string s = std::string(wStr.begin(), wStr.end());
	string::size_type pos = string(s).find_last_of("\\/");
	return string(s).substr(0, pos);
}

int main()
{
	cout << "my directory is " << ExePath() << "\n";
	const auto m = ReadFromFile();
	olcEngine3D demo;

	if (demo.ConstructConsole(400, 172, 4, 4))
	{
		demo.InitMesh(m);
		demo.Start();
	}

	return 0;
}

