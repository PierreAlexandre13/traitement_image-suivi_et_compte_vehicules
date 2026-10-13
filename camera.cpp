#include "camera.h"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h> // pour usleep
#include <opencv2/opencv.hpp> // à inclure si pas déjà dans camera.h

Camera::Camera()
{
    m_fps = 30;
}

bool Camera::open(std::string filename)
{
    m_fileName = filename;

    // Convert filename to device number if possible
    std::istringstream iss(filename);
    int devid;
    bool isOpen;

    if (!(iss >> devid))
    {
        isOpen = m_cap.open(filename);
    }
    else
    {
        isOpen = m_cap.open(devid);
    }

    if (!isOpen)
    {
        std::cerr << "Unable to open video file or camera: " << filename << std::endl;
        return false;
    }

    // Try to get the FPS
    m_fps = static_cast<int>(m_cap.get(cv::CAP_PROP_FPS));
    if (m_fps <= 0)
        m_fps = 30;

    return true;
}

void Camera::play()
{
    // OpenCV crée automatiquement la fenêtre avec imshow
    bool isReading = true;
	bool firstFrame = true;
	int tenFrame = 0;
	float erode_size = 4; //4
	float dilate_size = 2; //3 
	Mat elementErode = getStructuringElement(MORPH_ELLIPSE, Size(2*erode_size+1, 2*erode_size+1), Point(erode_size, erode_size));
	Mat elementDilate = getStructuringElement(MORPH_ELLIPSE, Size(2*dilate_size+1, 2*dilate_size+1), Point(dilate_size, dilate_size));

	Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorKNN();

    int timeToWait = 1000 / m_fps;
    Point pt1, pt2;
    vector<Vec2f> lines; 

    int compteurGauche = 0;
    int compteurDroit = 0;

    while (isReading)
    {
        // Read a frame
        isReading = m_cap.read(m_frame);


        if (isReading && !m_frame.empty())
        {	

			//cv::cvtColor (m_frame, m_frame_gray, cv::COLOR_RGBA2GRAY);
			//cv::GaussianBlur (m_frame_gray, m_frame_blur, cv::Size(5, 5), 1.5);
			cv::Canny (m_frame, m_frame_contours, 100, 500, 3); //blur

            // recuperation des contours de la 10eme frame pour tracer les bordures des voies
			if (firstFrame == true){
				if(tenFrame == 10){
					
					HoughLines(m_frame_contours, lines, 1, CV_PI/180, 130, 0, 0 );                                                          
					firstFrame = false;
				}
                
				tenFrame++;
			}

            

			pBackSub->apply(m_frame, m_frame_SB); // retire le fond par soustraction avec la première frame

            cv::medianBlur(m_frame_SB, m_frame_SB, 11); // filtrage du bruit de type poivre et sel
			cv::threshold(m_frame_SB, m_frame_SB, 150, 200, cv::THRESH_BINARY); // seuil de binarisation

			//cv::dilate(m_frame_SB, m_frame_SB, elementDilate);
			//cv::erode(m_frame_SB, m_frame_SB, elementErode);
			cv::erode(m_frame_SB, m_frame_SB, elementDilate); //erosion
			cv::dilate(m_frame_SB, m_frame_SB, elementDilate); //dilatation


			std::vector<std::vector<cv::Point>> contours;
			cv::Mat hierarchy;
			cv::findContours(m_frame_SB, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
			for(int i=0; i<contours.size(); i++){ // on parcours chacun des contours trouve par findContours
				const std::vector<cv::Point>& cnt = contours[i]; // on recupere un contour
				cv::Rect rect = cv::boundingRect(cnt);  // on calcule un rectangle qui englobe ce contour
				cv::Scalar rectangleColor(255, 0, 0); // couleur du rectangle a dessiner
				cv::Point point1(rect.x, rect.y); // on recupere les deux points du rectangle 
				cv::Point point2(rect.x + rect.width, rect.y + rect.height);
				cv::rectangle(m_frame, point1, point2, rectangleColor, 2, LINE_AA, 0); // on dessine le rectangle 

                //increment du compteur si le point en haut a gauche de la detection de vehicules si trouve dans une zone de detection
                //point1 dans rectGauche
                if (rect.x <= 300 && rect.y >= 50 && rect.y <= 52)
                {
                    //17
                    compteurGauche++;
                }
                //increment du compteur si le point en bas a droite de la detection de vehicules si trouve dans une zone de detection
                //point1 dans rectDroite
                if (rect.x+rect.width >= 500 && rect.y+rect.height >= 80 && rect.y+rect.height <= 82) 
                {
                    //9
                    compteurDroit++;
                }
                
			}

            // Dessine les lignes
            for( size_t i = 0; i < (lines.size()); i++ )
            {   
                float rho = lines[i][0], theta = lines[i][1];
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));
                line(m_frame, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);

            }

            // Dessine les rectangle de détection de véhicules
            cv::Scalar rectangleColor2(0, 255, 0);
            cv::Point pRect1(0, 50);
            cv::Point pRect2(350, 55);
            cv::Point pRect3(500, 80);
            cv::Point pRect4(800, 85);
            cv::rectangle (m_frame, pRect1, pRect2, rectangleColor2, 1, LINE_AA);
            cv::rectangle (m_frame, pRect3, pRect4, rectangleColor2, 1, LINE_AA);
            
            //affiche des compteurs de véhicules
            cv::putText (m_frame, to_string(compteurGauche), Point(300, 150), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(255, 0, 0));
            cv::putText (m_frame, to_string(compteurDroit), Point(450, 150), cv::FONT_HERSHEY_DUPLEX, 0.8, Scalar(255, 0, 0));
			
			cv::imshow("Video", m_frame); 

        }
        else
        {
            std::cerr << "Unable to read frame from device" << std::endl;
            break;
        }

        // Wait for key press (ESC = 27)
        int key = cv::waitKey(timeToWait);
        if (key == 27)
        {
            std::cerr << "Stopped by user" << std::endl;
            break;
        }
    }
}

bool Camera::close()
{
    // Release the stream
    if (m_cap.isOpened())
        m_cap.release();

    // Close all OpenCV windows
    cv::destroyAllWindows();

    // Small delay to ensure resources are released
    usleep(100000);

    return true;
}
