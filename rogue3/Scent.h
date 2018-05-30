#pragma once

static const float MAX_STINK = 256.0f;

// TODO: Vary the stink of different actors.
static const float DEFAULT_STINK = MAX_STINK;

class Scent : public Persistent
{
public:
	int smellsLikeId;
	int x;
	int y;
	float intensity;
	int radius;

	Scent(int smellsLikeId, int x, int y, float intensity = DEFAULT_STINK);
	void load(TCODZip &zip);
	void save(TCODZip &zip);

	void update();
#ifdef RENDER_SCENTS
	void render(int offsetX, int offsetY) const;
#endif

	// As the scent grows older, it expands and reduces in intensity.  Once it drops below a threshold, it is culled from the map.
	bool isAlive() const;
};

