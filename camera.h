#include <opencv2/opencv.hpp>
#include <string.h>

using namespace cv;
using namespace std;

class Camera
{
public:
	Camera();
	
	bool open(std::string name);
	void play();
	bool close();
	
private:
	std::string m_fileName;
	VideoCapture m_cap;
	int m_fps;
		
	Mat m_frame;
	Mat m_frame_blur;
	Mat m_frame_SB;
	Mat m_frame_gray;
	Mat m_frame_binary;
	Mat m_frame_contours;
	Mat m_frame_init;
	Mat m_frame_diff;
	Mat m_frame_erode;
	Mat m_frame_dilate;
	Mat m_frame_finale;
};
