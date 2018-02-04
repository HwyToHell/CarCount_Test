#include "stdafx.h"
#include "../../videoprocessing/include/tracker.h"

using namespace std;
using namespace cv;

ostream& operator<<(ostream& out, vector<vector<double>> mat);
ostream& operator<<(ostream& out, cv::Rect r);


/******************************************************************************
/*** TrackEntry ***************************************************************
/******************************************************************************/

TEST_CASE( "Centroid, positive bbox", "[TrackEntry]" ) {
    TrackEntry te(100, 100, 100, 100);
	REQUIRE(te.centroid().x == 150);
	REQUIRE(te.centroid().y == 150);
}

TEST_CASE( "Centroid, negative bbox", "[TrackEntry]" ) 
{
    TrackEntry te(-100, -100, -100, -100);
	REQUIRE(te.centroid().x == 150);
	REQUIRE(te.centroid().y == 150);
}

TEST_CASE( "use Rect contstructor", "[TrackEntry]" ) 
{
    TrackEntry te(Rect(100, 100, 100, 100));
	REQUIRE(te.centroid().x == 150);
	REQUIRE(te.centroid().y == 150);
}

TEST_CASE( "is size similar", "[TrackEntry]" ) 
{ 
	TrackEntry te(0, 0, 100, 100);

	SECTION( "similar -> 10% larger is OK" ) {
		REQUIRE(true == te.isSizeSimilar(TrackEntry(0, 0, 110, 110), 10));
		REQUIRE(true == te.isSizeSimilar(TrackEntry(0, 0, 90, 90), 10));
	}
	SECTION( "not similar -> more than 10% larger is not OK" ) {
		REQUIRE(false == te.isSizeSimilar(TrackEntry(0, 0, 111, 111), 10));
		REQUIRE(false == te.isSizeSimilar(TrackEntry(0, 0, 89, 89), 10));
	}
}

TEST_CASE( "is close", "[TrackEntry]" ) 
{ 
	TrackEntry te(100, 100, 100, 100);

	SECTION( "close -> distance smaller than 30" ) {
		REQUIRE(true == te.isClose(TrackEntry(129, 129, 100, 100), 30));
		REQUIRE(true == te.isClose(TrackEntry(71, 71, 100, 100), 30));

	}
	SECTION( "not close -> distance larger than 30" ) {
		REQUIRE(false == te.isClose(TrackEntry(131, 131, 100, 100), 30));
		REQUIRE(false == te.isClose(TrackEntry(69, 69, 100, 100), 30));
	}
}

/******************************************************************************
/*** Track ********************************************************************
/******************************************************************************/

TEST_CASE( "ID", "[Track]" ) 
{
	Track track(TrackEntry(0, 0, 100, 100), 1);
	REQUIRE(track.getId() == 1);
}

TEST_CASE( "velocity", "[Track]" ) 
{
	Track track(TrackEntry(0, 0, 100, 100), 1);
	REQUIRE(track.getActualEntry().rect() == cv::Rect(0, 0, 100, 100));
	REQUIRE(track.getVelocity() == cv::Point2d(0, 0));

	track.addTrackEntry(TrackEntry(50, 0, 100, 100));
	REQUIRE(track.getActualEntry().rect() == cv::Rect(50, 0, 100, 100));
	REQUIRE(track.getVelocity() == cv::Point2d(50, 0));
	
	track.addTrackEntry(TrackEntry(150, 0, 100, 100));
	REQUIRE(track.getActualEntry().rect() == cv::Rect(150, 0, 100, 100));
	REQUIRE(track.getVelocity() == cv::Point2d(75, 0));

	SECTION("substitute value")
	{
		track.addSubstitute();
		REQUIRE(track.getActualEntry().rect() == cv::Rect(150+75, 0, 100, 100));	
		REQUIRE(track.getVelocity() == cv::Point2d(75, 0)); 		// average velocity remains unchanged
	}
	// clip rect out of roi

}

TEST_CASE( "confidence increase", "[Track]" ) 
{
	Track track(TrackEntry(0, 0, 100, 100), 1);
	track.addTrackEntry(TrackEntry(50, 0, 100, 100));
	track.addTrackEntry(TrackEntry(150, 0, 100, 100));
	std::list<TrackEntry> blobs;
	SECTION ("take closest")
	{
		blobs.push_back(TrackEntry(150+10, 0, 100, 100)); // closest, maxDist = 30
		blobs.push_back(TrackEntry(150+20, 0, 100, 100));
		REQUIRE(blobs.size() == 2);
		track.updateTrack(blobs);
		REQUIRE(track.getActualEntry().rect() == cv::Rect(150+10, 0, 100, 100));
		REQUIRE(blobs.size() == 1);
		REQUIRE(track.getConfidence() == 1);
	}
	SECTION ("take similar")
	{
		blobs.push_back(TrackEntry(150, 0, 150, 100)); // shape similar, maxDeviation = 80%
		blobs.push_back(TrackEntry(150, 0, 200, 100)); 
		REQUIRE(blobs.size() == 2);
		track.updateTrack(blobs);
		REQUIRE(track.getActualEntry().rect() == cv::Rect(150, 0, 150, 100));
		REQUIRE(blobs.size() == 1);
		REQUIRE(track.getConfidence() == 1);
	}
}

SCENARIO("Track will be deleted, if confidence drops below zero", "[Track]")
{
	GIVEN("Track with 3 updated TrackEntries")
	{
		Track track(TrackEntry(0, 0, 100, 100), 1);		
		std::list<TrackEntry> blobs;
		blobs.push_back(TrackEntry(10, 0, 100, 100)); // closest, maxDist = 30
		blobs.push_back(TrackEntry(30, 0, 100, 100));
		blobs.push_back(TrackEntry(45, 0, 100, 100));
		track.updateTrack(blobs);
		track.updateTrack(blobs);
		track.updateTrack(blobs);
		REQUIRE(blobs.size() == 0);
		REQUIRE(track.getConfidence() == 3);
		REQUIRE(track.getActualEntry().rect() == cv::Rect(45, 0, 100, 100));
		REQUIRE(track.getVelocity() == cv::Point2d(15, 0));
		WHEN("Substitute calculated 3 times")
		{
			blobs.push_back(TrackEntry(130, 0, 100, 100)); // too far >> 45(bbox.x) + 3(updates) * 15(actVelocity) + 30(maxDist) = 120
			REQUIRE(track.isMarkedForDelete() == false);
			track.updateTrack(blobs);
			track.updateTrack(blobs);
			track.updateTrack(blobs);
			REQUIRE(track.getConfidence() == 0);	
			REQUIRE(track.isMarkedForDelete() == true);
		}
	}
}




ostream& operator<<(ostream& out, cv::Rect r)
{
	out << "Rect x=" << r.x << " y=" << r.y << " width=" << r.width << " height=" << r.height << endl;
	return out;
}

ostream& operator<<(ostream& out, vector<vector<double>> mat)
{
	for (unsigned int i = 0; i < mat.size(); i++)
	{
		out << "row " << i << " "; 
		for (unsigned int n = 0; n < mat[i].size(); n++)
			out << mat[i][n] << ", ";
		out << endl;
	}
	return out; 
}

void PrintShapes(list<TrackEntry>& Shapes)
{
	// print bbox of new shapes
	cout << endl << "List of new shapes" << endl;
	list<TrackEntry>::iterator ns = Shapes.begin();
	int i = 0;
	while (ns != Shapes.end()) 
	{
		i++;
		cout << i << "  [" << ns->rect().x << ", " << ns->rect().y << ", " << ns->rect().width << ", " << ns->rect().height << "]" << endl;
		++ns;
	}// end for all new shapes
	return;
}