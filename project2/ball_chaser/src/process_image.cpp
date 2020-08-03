#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    // Done
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    if (!client.call(srv)) {
        ROS_ERROR("Failed to call service drive_bot!");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
 
   float lin_x = 0, ang_z = 0;
   bool ball_in_camera = false;
   //int region_LCR = 0;
   for (int i = 0; i<(img.height * img.step); i+=3){
        // assume the ball is the only object that could reach 255 intensity in the image taken from camera
	if ((img.data[i] + img.data[i+1] + img.data[i+2])/3 == white_pixel){
            ball_in_camera = true;
	// find out which colum the white_pixel located in
	int region_LCR = i % img.step;

	    // if it is in the first 1/3
	    if (region_LCR <= img.step/3){
               // turn left
               lin_x = 0;
	       ang_z = 0.1;
               }

            // if it is beyong the 2/3
	    else if (region_LCR >= 2*img.step/3){
		// turn right
               lin_x = 0;
	       ang_z = -0.1;
	       }

	    else {
	       // go straight
	       lin_x = 0.5;
	       ang_z = 0;
               }
	      break;
            }
       else{
            // if the ball is not in the camera's view , stop it.
            ball_in_camera = false;
            lin_x = 0;
            ang_z = 0;
          }
    // drive the robot to move as specified in lin_x and ang_z

        }

  drive_robot(lin_x, ang_z);

  // Publish message to user
  if (ball_in_camera) { 
    ROS_INFO_STREAM("Target detected, driving towards the target ...");
    std::string linx = "lin_x is " + std::to_string(lin_x);
    std::string angz = "ang_z is" + std::to_string(ang_z);
    std::cout << linx<<'\n';
    std::cout << angz<<'\n';
      }

  else {
    ROS_INFO_STREAM("Target is not found, robot is stopped.");
    std::string linx = "lin_x is " + std::to_string(lin_x);
    std::string angz = "ang_z is " + std::to_string(ang_z);
    std::cout << linx<<'\n';
    std::cout << angz<<'\n';
  }

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
