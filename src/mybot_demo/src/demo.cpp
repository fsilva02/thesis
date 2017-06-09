#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <geometry_msgs/Twist.h>
#include <math.h>

static const std::string OPENCV_WINDOW = "Image window";
static const std::string OPENCV_WINDOW2 = "Image window2";

using namespace cv;

class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;

public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/nardo/camera1/image_raw", 1,
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    namedWindow(OPENCV_WINDOW);
    namedWindow(OPENCV_WINDOW2);

  }

  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }

  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cv_ptr;
    Mat img_rgb;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
      img_rgb = cv_ptr->image;
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    // Draw an example circle on the video stream
    if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
      circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));
    //code goes here
    Mat img_gray;
    Mat img_final;
    Mat gray_out;
    Mat canny_out;
    Mat canny_out1;
    Mat canny_out2;
    Mat gray_out1;
    Mat gray_out2;


    cvtColor(img_rgb,img_gray,CV_RGB2GRAY);
   // cvSmooth(img_gray,img_gray,CV_GAUSSIAN, 11, 11);
   // cvNot(img_gray, gray_out1);
    Canny(gray_out1, canny_out,80, 125,3, false);

    //adaptiveThreshold(img_gray,img_final, 127, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 11, 12);
    threshold ( img_gray,img_final, 0, 255, THRESH_BINARY | THRESH_OTSU );

    // Update GUI Window
    imshow(OPENCV_WINDOW, img_final);
    waitKey(3);

    // Output modified video stream
    image_pub_.publish(cv_ptr->toImageMsg());


  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}
