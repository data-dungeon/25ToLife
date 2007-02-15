#ifndef __CLOTHMODDATA_H__
#define __CLOTHMODDATA_H__


class ClothModData;

enum PointState
{
	POINTSTATE_INACTIVE,
	POINTSTATE_FRONT,
	POINTSTATE_ATTACHED,
	POINTSTATE_BACK
};



class ClothPoint
{
public:
	ClothPoint()
	{
		state = POINTSTATE_FRONT;
	}

	PointState state;
};


enum ConnectionType
{
	CT_FRONTFRONT,
	CT_FRONTBACK,
	CT_BAD
};


class ClothConnection
{
public:
	int v1, v2;
	bool selected;

	ConnectionType GetConnectionType(ClothModData* pModData);
};


class ClothModData : public LocalModData
{
public:
	// exported data
	Tab<ClothConnection> connections;
	Tab<ClothPoint> points;

	// display dataZ
	Tab<int> selectedPoints;

	// other internal data
	Tab<Point3> pointNormals;
	Tab<int> hilightedPoints;
	Tab<int> hilightedConnections;

	int id;

	Point3 hitPoint;
	BOOL isHit;
	BOOL isDragging;

	ClothModData();
	ClothModData(int i);
	~ClothModData();

	LocalModData*	Clone();
};

#endif // __CLOTHMODDATA_H__
