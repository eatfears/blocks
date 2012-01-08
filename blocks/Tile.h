#pragma once

class Tile
{
public:
	Tile(signed short xc, signed short yc, signed short zc, char zmat)
	{ x = xc; y = yc; z = zc; mat = zmat; };
	~Tile (void);

	signed short x;
	signed short y;
	signed short z;

	char mat;
};
