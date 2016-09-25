#include <canlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "ros/ros.h"
#include "std_msgs/UInt8.h"
#include "std_msgs/Float64.h"
#include "globe_epas/globe_epas_cmd.h"
#include "globe_epas/globe_epas_position_with_speed.h"

int command_mode;

const long CAN_ID=0x18FF00F9;   // ID for motor command message
const double gear_ratio=16.5;
unsigned char willExit=0;
canHandle h;

void motor_off();
void set_current(double);
void enable_torque_assist();
void set_speed(double);
void set_position(double);
void set_position_with_speed(double,double);
void set_int_encoder_value(double);
void delay(int);
void check(char*,canStatus);
void globeEPASCallback(const globe_epas::globe_epas_cmd::ConstPtr&);
void sighand(int);

void delay(int milliseconds) {
  long pause;
  clock_t now,then;

  pause = milliseconds*(CLOCKS_PER_SEC/1000);
  now = then = clock();
  while( (now-then) < pause )
    now = clock();
}

void check(const char* id, canStatus stat) {
  char buf[50];

  buf[0] = '\0';
  canGetErrorText(stat, buf, sizeof(buf));
  if (stat != canOK) {
    ROS_INFO("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
  } else {
    //ROS_INFO("%s: OK\n", id);
  }
}

void motor_off() {
  // Turn off the motor's drive to stop the motor. Can still be backdriven.
  char msg_buf[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  
  msg_buf[0]=0x00;
  msg_buf[2]=0x00;
  msg_buf[3]=0x00;
  msg_buf[4]=0x00;
  msg_buf[5]=0x00;
  
  check("canWrite", canWrite(h, CAN_ID, (void*)msg_buf, 8, canMSG_EXT));       
  check("canWriteSync", canWriteSync(h, 1000));
}

void set_current(double current) {
  // Current control mode for the Globe EPAS motor. Specified current in amps,
  // internally limited to +/- 60A.
  char msg_buf[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  int Q20_current = (int)(current*pow(2,20));
  
  msg_buf[0]=0x01;
  msg_buf[2]=(Q20_current&0x000000FF);
  msg_buf[3]=(Q20_current&0x0000FF00)>>8;
  msg_buf[4]=(Q20_current&0x00FF0000)>>16;
  msg_buf[5]=(Q20_current&0xFF000000)>>24;
  
  check("canWrite", canWrite(h, CAN_ID, (void*)msg_buf, 8, canMSG_EXT));       
  check("canWriteSync", canWriteSync(h, 1000));
}

void enable_torque_assist() {
  // Unit responds to input torque, provides output torque according to
  // programmed assist curve.
  char msg_buf[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  
  msg_buf[0]=0x02;
  
  check("canWrite", canWrite(h, CAN_ID, (void*)msg_buf, 8, canMSG_EXT));       
  check("canWriteSync", canWriteSync(h, 1000));
}

void set_speed(double angular_speed) {
    // Speed control mode for the Globe EPAS motor. Specified speed in
    // degrees/second, internally limited to +/- 20 rev/second.
    char msg_buf[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    int Q20_angular_speed = (int)(angular_speed/360.0*gear_ratio*pow(2,20));

    msg_buf[0]=0x03;
    msg_buf[2]=(Q20_angular_speed&0x000000FF);
    msg_buf[3]=(Q20_angular_speed&0x0000FF00)>>8;
    msg_buf[4]=(Q20_angular_speed&0x00FF0000)>>16;
    msg_buf[5]=(Q20_angular_speed&0xFF000000)>>24;

    check("canWrite", canWrite(h, CAN_ID, (void *) msg_buf, 8, canMSG_EXT));
    check("canWriteSync", canWriteSync(h, 1000));
}

void set_position(double angular_position) {
  // Position control mode for the Globe EPAS motor. Specified current in degrees.
  char msg_buf[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  int Q20_angular_position = (int)(angular_position/360.0*pow(2,20));
   
  msg_buf[0]=0x04;
  msg_buf[2]=(Q20_angular_position&0x000000FF);
  msg_buf[3]=(Q20_angular_position&0x0000FF00)>>8;
  msg_buf[4]=(Q20_angular_position&0x00FF0000)>>16;
  msg_buf[5]=(Q20_angular_position&0xFF000000)>>24;
  
  check("canWrite", canWrite(h, CAN_ID, (void*)msg_buf, 8, canMSG_EXT));       
  check("canWriteSync", canWriteSync(h, 1000));
}

void set_position_with_speed_limit(double angular_position, double angular_speed) {
  // Position control mode for the Globe EPAS motor, with speed limit.
  // Specified position in degrees, speed in degrees/second.
  char msg_buf[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  int Q20_angular_position = (int)(angular_position/360.0*pow(2,20));
  int Q8_angular_speed = (int)(angular_speed/360.0*pow(2,8));
  
  msg_buf[0]=0x05;
  msg_buf[2]=(Q20_angular_position&0x000000FF);
  msg_buf[3]=(Q20_angular_position&0x0000FF00)>>8;
  msg_buf[4]=(Q20_angular_position&0x00FF0000)>>16;
  msg_buf[5]=(Q20_angular_position&0xFF000000)>>24;
  msg_buf[6]=(Q8_angular_speed&0x00FF);
  msg_buf[7]=(Q8_angular_speed&0xFF00)>>8;
  
  check("canWrite", canWrite(h, CAN_ID, (void*)msg_buf, 8, canMSG_EXT));       
  check("canWriteSync", canWriteSync(h, 1000));
}

void set_inc_encoder_value(double angular_position) {
  // Sets the value of the internal encoder to the specified position (in
  // degrees). Doesn't move the motor.
  char msg_buf[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  int Q20_angular_position = (int)(angular_position/360.0*pow(2,20));
  
  msg_buf[0]=0x10;
  msg_buf[2]=(Q20_angular_position&0x000000FF);
  msg_buf[3]=(Q20_angular_position&0x0000FF00)>>8;
  msg_buf[4]=(Q20_angular_position&0x00FF0000)>>16;
  msg_buf[5]=(Q20_angular_position&0xFF000000)>>24;
  
  check("canWrite", canWrite(h, CAN_ID, (void*)msg_buf, 8, canMSG_EXT));       
  check("canWriteSync", canWriteSync(h, 1000));
}

void callback_set_command_mode(const globe_epas::globe_epas_cmd::ConstPtr& msg) {
  command_mode = msg->command_mode;
  switch(msg->command_mode) {
  case globe_epas::globe_epas_cmd::OFF:  
    motor_off();
    break;
  case globe_epas::globe_epas_cmd::CURRENT_CONTROL:  
    break;
  case globe_epas::globe_epas_cmd::TORQUE_ASSIST:  
    enable_torque_assist();
    break;
  case globe_epas::globe_epas_cmd::SPEED_CONTROL:  
    break;
  case globe_epas::globe_epas_cmd::POSITION_CONTROL:  
    break;
  case globe_epas::globe_epas_cmd::POSITION_CONTROL_WITH_SPEED_LIMIT:
    break;
  case globe_epas::globe_epas_cmd::SET_INC_ENCODER_VALUE:
    break;
  default:
    ROS_INFO("Invalid Globe EPAS command mode specified: %d", msg->command_mode);
  }        
}

void callback_set_current(const std_msgs::Float64::ConstPtr& msg) {
  if(command_mode!=globe_epas::globe_epas_cmd::CURRENT_CONTROL) {
    ROS_INFO("Ignoring Globe EPAS command due to incorrect command mode");
    return;
  }
  set_current(msg->data);
}  

void callback_set_speed(const std_msgs::Float64::ConstPtr& msg) {
  if(command_mode!=globe_epas::globe_epas_cmd::SPEED_CONTROL) {
    ROS_INFO("Ignoring Globe EPAS command due to incorrect command mode");
    return;
  }
  set_speed(msg->data);
}  

void callback_set_position(const std_msgs::Float64::ConstPtr& msg) {
  printf("Setting position\n");
  if(command_mode!=globe_epas::globe_epas_cmd::POSITION_CONTROL) {
    ROS_INFO("Ignoring Globe EPAS command due to incorrect command mode");
    return;
  }
  set_position(msg->data);
}  

void callback_set_position_with_speed_limit(const globe_epas::globe_epas_position_with_speed::ConstPtr& msg) {
  if(command_mode!=globe_epas::globe_epas_cmd::POSITION_CONTROL_WITH_SPEED_LIMIT) {
    ROS_INFO("Ignoring Globe EPAS command due to incorrect command mode");
    return;
  }
  set_position_with_speed_limit(msg->angular_position, msg->speed_limit);
}  

void callback_set_inc_encoder_value(const std_msgs::Float64::ConstPtr& msg) {
  if(command_mode!=globe_epas::globe_epas_cmd::SET_INC_ENCODER_VALUE) {
    ROS_INFO("Ignoring Globe EPAS command due to incorrect command mode");
    return;
  }
  set_inc_encoder_value(msg->data);
}  
    
void sighand(int sig) {
  switch(sig) {
  case SIGINT:
    willExit=1;
    alarm(0);
    break;
  }
}
 
int main(int argc, char *argv[]) {
  // Setup CAN and ROS. Then loop forever processing received CAN messages, and publishing
  // info on ROS topics as needed. Also subcribed to ROS topic for commands.

  int ret = -1;
  long id;
  unsigned char msg[8];
  unsigned int dlc;
  unsigned int flag;
  unsigned long t;
  int channel = 0;
  int bitrate = BAUD_250K;
  
  ///////////////
  // ROS setup //
  ///////////////
  ros::init(argc, argv, "globe_epas");
  ros::NodeHandle n;

  ros::Publisher globe_epas_command_mode_pub = n.advertise<globe_epas::globe_epas_cmd>("report/command_mode", 1000);
  ros::Publisher globe_epas_inc_position_pub = n.advertise<std_msgs::Float64>("report/inc_position", 1000);
  ros::Publisher globe_epas_current_pub = n.advertise<std_msgs::Float64>("report/current", 1000);
  ros::Publisher globe_epas_velocity_pub = n.advertise<std_msgs::Float64>("report/velocity", 1000);
  ros::Publisher globe_epas_unit_temp_pub = n.advertise<std_msgs::Float64>("report/unit_temp", 1000);
  ros::Publisher globe_epas_encoder_temp_pub = n.advertise<std_msgs::Float64>("report/encoder_temp", 1000);
  ros::Publisher globe_epas_torque_input_pub = n.advertise<std_msgs::Float64>("report/torque_input", 1000);
  ros::Publisher globe_epas_torque_output_pub = n.advertise<std_msgs::Float64>("report/torque_output", 1000);
  
  ros::Subscriber globe_epas_set_command_mode_sub = n.subscribe("command/set_command_mode", 1000, callback_set_command_mode);
  ros::Subscriber globe_epas_set_current_sub = n.subscribe("command/set_current", 1000, callback_set_current);
  ros::Subscriber globe_epas_set_speed_sub = n.subscribe("command/set_speed", 1000, callback_set_speed);
  ros::Subscriber globe_epas_set_position_sub = n.subscribe("command/set_position", 1000, callback_set_position);
  ros::Subscriber globe_epas_set_position_with_speed_limit_sub = n.subscribe("command/set_position_with_speed_limit", 1000, callback_set_position_with_speed_limit);
  ros::Subscriber globe_epas_set_inc_encoder_value_sub = n.subscribe("command/set_inc_encoder_value", 1000, callback_set_inc_encoder_value);
  
//  ros::Rate loop_rate(10);

  ///////////////
  // CAN setup //
  ///////////////

  // Use sighand as our signal handler
  signal(SIGALRM, sighand);
  signal(SIGINT, sighand);
  alarm(1);

  // Allow signals to interrupt syscalls (in canReadBlock)
  siginterrupt(SIGINT, 1);
  
  // Open channels, set parameters and go on bus
  h = canOpenChannel(channel, canOPEN_REQUIRE_EXTENDED);
  if(h < 0) {
    ROS_INFO("canOpenChannel %d failed\n", channel);
    return -1;
  }    
  canSetBusParams(h, bitrate, 4, 3, 1, 1, 0);
  canSetBusOutputControl(h, canDRIVER_NORMAL);
  canBusOn(h);

  // Main loop, waiting for one of the three Globe EPAS feedback messages
  while(!willExit) {
    do { 
      ret = canReadWait(h, &id, &msg, &dlc, &flag, &t, -1);
      switch (ret) { 
      case 0:
        if(id==0x18ff0113) {    
          double inc_position = ((msg[3]<<24) + (msg[2]<<16) + (msg[1]<<8) + msg[0])*360.0/pow(2,20);
          double current = ((msg[7]<<24) + (msg[6]<<16) + (msg[5]<<8) + msg[4])/pow(2,20);
          
          std_msgs::Float64 pub_msg;
          pub_msg.data=inc_position;
          globe_epas_inc_position_pub.publish(pub_msg);

          pub_msg.data=current;
          globe_epas_current_pub.publish(pub_msg);
        } else if(id==0x18ff0213) {
          double velocity = ((msg[3]<<24) + (msg[2]<<16) + (msg[1]<<8) + msg[0])*360.0/pow(2,20)/gear_ratio;
          double unit_temp = msg[4]-40.0;
          double encoder_temp = msg[5]-40.0;  
                  
          std_msgs::Float64 pub_msg;
          pub_msg.data=velocity;
          globe_epas_velocity_pub.publish(pub_msg);

          pub_msg.data=unit_temp;
          globe_epas_unit_temp_pub.publish(pub_msg);

          pub_msg.data=encoder_temp;
          globe_epas_encoder_temp_pub.publish(pub_msg);
        } else if(id==0x18ff0313) {
          double torque_input = ((msg[3]<<24) + (msg[2]<<16) + (msg[1]<<8) + msg[0])/pow(2,20);
          double torque_output = ((msg[7]<<24) + (msg[6]<<16) + (msg[5]<<8) + msg[4])/pow(2,20);
          std_msgs::Float64 pub_msg;

          pub_msg.data=torque_input;
          globe_epas_torque_input_pub.publish(pub_msg);

          pub_msg.data=torque_output;
          globe_epas_torque_output_pub.publish(pub_msg);
        }
        break;
      case canERR_NOMSG:
        break;
      default:
        perror("canReadBlock error");
        break;
      }

      globe_epas::globe_epas_cmd pub_msg;
      pub_msg.command_mode = command_mode;
      globe_epas_command_mode_pub.publish(pub_msg);
      ros::spinOnce();
                
    } while((ret==canOK)&&(ros::ok()));
    willExit=1;
  } 
   
  canClose(h);   
  sighand(SIGALRM);

  return 0;
}
