#include<ros/ros.h>
#include <geometry_msgs/Twist.h>
#include<nav_msgs/Odometry.h>
#include<tf/tf.h>

using namespace std;

ros::Publisher vel_pub;
ros::Subscriber pos_sub;

double x, y, theta, roll, pitch, yaw;
double goal_pose_x, goal_pose_y, goal_pose_theta;

void go_to_goal(double goal_pose_x, double goal_pose_y, double distance_error);
double get_distance(double x1, double y1, double x2, double y2);
void poseCallback(const nav_msgs::Odometry::ConstPtr & pose_message);

int main(int argc, char **argv){
	ros::init(argc, argv, "controller");
	ros::NodeHandle nh;
	vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 100);
	pos_sub = nh.subscribe("/odom", 10, poseCallback);

	cout << "Enter x co-ordinate\n";
	cin >> goal_pose_x;
	cout << "Enter y co-ordinate\n";
	cin >> goal_pose_y;

	go_to_goal(goal_pose_x, goal_pose_y, 0.04);
	ros::Rate rate(10);
	rate.sleep();
	ros::spin();
	return 0;
}

void go_to_goal(double goal_pose_x, double goal_pose_y, double distance_error)
{
	geometry_msgs::Twist vellocity_message;
	double Kp1 = 0.2;
	double Kp2 = 3;
	ros::Rate loop_rate(100);
	double distance = get_distance(x, y, goal_pose_x, goal_pose_y);
	cout << "distance = " << distance << endl;
	while (distance > distance_error)
	{
		distance = get_distance(x, y, goal_pose_x, goal_pose_y);
		cout << "distance = " << distance << endl;
		vellocity_message.linear.x = Kp1 * distance;
		vellocity_message.angular.z = Kp2 * (atan2(goal_pose_y - y, goal_pose_x - x) - theta); 
		vel_pub.publish(vellocity_message);
		cout << "X = " << x << endl;
		cout << "Y = " << y << endl;
		ros::spinOnce();
		loop_rate.sleep();
		if (distance < distance_error)
			break;
	}
	cout << "----Reached----\n";
	vellocity_message.linear.x = 0.0;
	vellocity_message.angular.z = 0.0;
	vel_pub.publish(vellocity_message);
}

double get_distance(double x1, double y1, double x2, double y2){
	return sqrt(pow((x2 - x1),2) + pow((y2 - y1), 2));
}

void poseCallback(const nav_msgs::Odometry::ConstPtr & pose_message)
{
	x = pose_message->pose.pose.position.x;
	y = pose_message->pose.pose.position.y;
	tf::Quaternion q(
        pose_message->pose.pose.orientation.x,
        pose_message->pose.pose.orientation.y,
        pose_message->pose.pose.orientation.z,
        pose_message->pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    m.getRPY(roll, pitch, theta);
}
