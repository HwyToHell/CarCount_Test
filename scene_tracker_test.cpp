#include "stdafx.h"
#include "../../videoprocessing/include/config.h" // includes tracker.h

using namespace std;
using namespace cv;


SCENARIO("Get and return trackIDs", "[SceneTracker]")
{
	GIVEN("scene with no tracks")
	{
		Config config;
		Config* pConfig = &config;
		SceneTracker scene(pConfig);
		WHEN("trackID is pulled 9 times")
		{
			int id[9];
			for (int i = 0; i < 9; ++i)
				id[i] = scene.nextTrackID();
			THEN("next trackid is 0")
				REQUIRE(scene.nextTrackID() == 0);
			AND_THEN("trackID is returned 9 times")
			{
				for (int i = 1; i < 10; ++i)
				{
					REQUIRE(scene.returnTrackID(i));
					//cout << "returned id[" << i << "]: " << i << endl;
				}
				REQUIRE(scene.returnTrackID(10) == false);
				REQUIRE(scene.returnTrackID(0) == false);
			}
		}
	}
}



//////////////////////////////////////////////////////////////////////////////
// implement test for "count and classify"
//////////////////////////////////////////////////////////////////////////////

struct TwoTracks
{
	Track& track1;
	Track& track2;
	TwoTracks(Track& tr1, Track& tr2) : track1(tr1), track2(tr2) {};
	TwoTracks& operator= (const TwoTracks& tSource)
	{
		track1 = tSource.track1;
		track2 = tSource.track2;
		return *this;
	}
};

// Generate two tracks in a scene
//  origin:   start (upper left corner) of inital rectangle
//  velocity: velocity vector the rectangle will be moved at 
void multiUpdateScene(SceneTracker& scene, Point2i origin1, Point2i origin2, Point2d velocity1, Point2d velocity2)
{
	int height = 100, width = 100;
	std::list<TrackEntry> blobs;
	for(size_t i = 0; i < 10; ++i)
	{
		blobs.push_back(TrackEntry(origin1.x + (int)(i*velocity1.x), origin1.y + (int)(i*velocity1.y),  width, height));
		blobs.push_back(TrackEntry(origin2.x + (int)(i*velocity2.x), origin2.y + (int)(i*velocity2.y),  width, height));
		scene.updateTracks(blobs);
	}
	return;
}

SCENARIO("counting", "[SceneTracker]")
{
	GIVEN("SceneTracker with 2 Tracks with 6 updated history elements")
	{
		Config config;
		Config* pConfig = &config;
		SceneTracker scene(pConfig);
		Point2i org1 = Point2i(100, 100);
		Point2i org2 = Point2i(500, 500);
		Point2d vel1 = Point2d(10,1);
		Point2d vel2 = Point2d(10,-1);
		double directionDiff = 0.1;
		double normDiff = 0.5;

		WHEN("velocity vectors have same positive direction, abs cos_phi is larger than 0.9 and projected length is the same")
		{
			multiUpdateScene(scene, org1, org2, vel1, vel2); // cos phi = 0.995
			scene.countVehicles(1);
		}

	}
}


/* test cases for count and classify
 *  direction (left, right) 
 *  count position
 *  track length
 */

