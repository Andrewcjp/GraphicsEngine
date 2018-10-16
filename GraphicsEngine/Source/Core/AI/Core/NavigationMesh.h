#pragma once
struct NavTriangle
{
	glm::vec3 Positons[3];
	int Cost = 1;
	bool Traverable = true;
	std::vector<NavTriangle*> NearTriangles;
	int Id = 0;
	glm::vec3 avgcentre = glm::vec3();
	bool IsPointInsideTri(glm::vec3 point);
};
struct NavigationPath
{
	std::vector<glm::vec3> Positions;
};
struct NavPoint
{
	glm::vec3 pos;
	NavPoint(glm::vec3 ipos)
	{
		pos = ipos;
	}
	float GetNavCost() {
		return gcost + fcost;
	}
	float gcost = 0;
	float fcost = 0;
	NavTriangle* owner = nullptr;
	bool operator==(const NavPoint& rhs)
	{
		return rhs.pos == pos;
	}
	NavPoint* Parent = nullptr;
};
namespace ENavRequestStatus
{
	enum Type
	{
		Failed,
		FailedPointOffNavMesh,
		Complete,
	};
}
class NavigationMesh
{
public:
	NavigationMesh();
	~NavigationMesh();
	void GenTestMesh();
	void DrawNavMeshLines(class DebugLineDrawer * drawer);
	void PopulateNearLists();
	NavTriangle * FindTriangleFromWorldPos(glm::vec3 worldpos);
	ENavRequestStatus::Type CalculatePath(glm::vec3 Startpoint, glm::vec3 EndPos, NavigationPath** outputPath);
private:
	std::vector<NavTriangle*> Triangles;
};

