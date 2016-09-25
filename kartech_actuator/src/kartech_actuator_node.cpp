#include <canlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float64.h>
#include <std_msgs/UInt8.h>
#include <std_msgs/UInt16.h>
#include <stdint.h>
#include "ros/ros.h"

#define POSITION_REPORT 0x80
#define MOTOR_CURRENT 0x81
#define ENHANCED_POSITION_REPORT 0x98

const long CAN_ID=0xFF0000;   // ID for motor command message

unsigned char willExit=0;
canHandle h;

void motor_off();
void set_current(double);
void enable_torque_assist();
void set_speed(double);
void set_position(double);
void set_position_with_speed(double,double);
void set_encoder_value(double);
void delay(int);
void check(char*,canStatus);
void sighand(int);
void callback_set_position(const std_msgs::Float32::ConstPtr& msg);

void check(const char* id, canStatus stat) {
  char buf[50];

  buf[0] = '\0';
  canGetErrorText(stat, buf, sizeof(buf));
  if (stat != canOK) {
    ROS_INFO("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
  } 
}

void callback_set_position(const std_msgs::Float32::ConstPtr& msg)
{
  std::cout << "Set position to: " << msg->data << " inches" << std::endl;
  uint8_t msg_buf[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
 
  float position_input;
  if(msg->data > 3.0)
    position_input = 3.0;
  else if(msg->data < 0)
    position_input = 0.0;
  else
    position_input = msg->data;


  uint16_t position_inches = static_cast<uint16_t>(position_input*1000 + 500);

  std::cout << "Position in inches: 0x" <<position_inches << std::endl;
  msg_buf[0] = 0x0F;
  msg_buf[1] = 0x4A;
  msg_buf[2] = (position_inches & 0x00FF);
  msg_buf[3] = (((position_inches >> 8) & 0x001F)  | 0xC0);
  msg_buf[4] = 0x0;
  msg_buf[5] = 0x0;
  msg_buf[6] = 0x0;
  msg_buf[7] = 0x0;

  for (int j = 0; j < 8; j++){
              std::cout << std::hex << std::setw(2) << std::setfill('0')  << (uint16_t)msg_buf[j] << " ";
            }
            std::cout << std::endl;

check("canWrite", canWrite(h, CAN_ID, msg_buf, 8, canMSG_EXT));
}

void set_position_reach_error(const uint16_t posError)
{
  uint8_t msg_buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  std::cout << "Position Reach Error set to: " << posError << " ms" << std::endl;
  msg_buf[0] = 0x7E;
  msg_buf[1] = 0x04;
  msg_buf[2] = posError & 0x00FF;
  msg_buf[3] = ((posError >> 8) & 0x00FF);
  msg_buf[4] = 0x0;
  msg_buf[5] = 0x0;
  msg_buf[6] = 0x0;
  msg_buf[7] = 0x0;

  for (int j = 0; j < 8; j++){
              std::cout << std::hex << std::setw(2) << std::setfill('0')  << (uint16_t)msg_buf[j] << " ";
            }
            std::cout << std::endl;

check("canWrite", canWrite(h, CAN_ID, msg_buf, 8, canMSG_EXT));
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
  
  int bitrate = BAUD_250K;
  int j = 0;

  ///////////////
  // ROS setup //
  ///////////////
  ros::init(argc, argv, "kartech_actuator");
  ros::NodeHandle n;

  //Get rosparams - set the defaults, too
  int channel;
  int positionReachError;
  n.param("can_channel", channel, 0);
  n.param("position_reach_error", positionReachError, 2000);

  ros::Publisher position_report_pub = n.advertise<std_msgs::Float32>("report/position_report", 1000);
  ros::Publisher motor_current_pub = n.advertise<std_msgs::Float32>("report/motor_current", 1000);
  ros::Publisher actuator_errors_pub = n.advertise<std_msgs::UInt8>("report/actuator_errors", 1000);

  //Set position in inches
  ros::Subscriber set_position_sub = n.subscribe("command/set_position", 1000, callback_set_position);
  
  //Set this ros node to run at 10 Hz    
  ros::Rate loop_rate(10);

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

  //Resquest for Position, motor current and enhanced position report;
  unsigned char messageRequests[8] = {0xF1, 0, 0x80, 0x81, 0x98, 0xFF, 0xFF, 0xFF};

  // Main loop, waiting for feedback messages
  while(!willExit) {
    do { 
      //Set the Position Reach Error time
      std::cout << "//////Postion Reach Error: " << positionReachError << std::endl;
      set_position_reach_error(static_cast<uint16_t> (positionReachError));
      //Set messageRequests
      check("canWrite", canWrite(h, CAN_ID, messageRequests, 8, canMSG_EXT)); 
      ret = canReadWait(h, &id, &msg, &dlc, &flag, &t, -1);
      switch (ret) { 
      case canOK:
        if(id==0xFF0101) {
          if(msg[0] == POSITION_REPORT) //Position report
          {
            if (dlc > 8) {
              dlc = 8;
            }
            for (j = 0; j < dlc; j++){
              std::cout << std::hex << std::setw(2) 
                        << std::setfill('0') << static_cast<unsigned int> (msg[j]) << " ";
            }
            
            double rawPosition = static_cast<double>( (msg[3] << 8) | (msg[2]));
            double position = (rawPosition - 500)/1000;

            std::cout << "Position: " << position << " inches" << std::endl;
          }
         
          if(msg[0] == MOTOR_CURRENT)
          {
            double current = static_cast<double>( (msg[3] << 8) | (msg[2]));
            std::cout << "Current: " << current << " mA" << std::endl;
          }

          if(msg[0] == ENHANCED_POSITION_REPORT)
          { 
            const double rawPosition = static_cast<double>( (msg[3] << 8) | (msg[2]));
            const double position = (rawPosition -500)/1000;
            std::cout << "Position: " << position << " inches" << std::endl;
            std_msgs::Float32 pub_position;
            pub_position.data = position;
            position_report_pub.publish(pub_position);          //publish position

            double current = static_cast<double>( (msg[6] << 8) | (msg[5]));
            std::cout << "Current: " << current << " mA" << std::endl;
            std_msgs::Float32 pub_current;
            pub_current.data = current;
            motor_current_pub.publish(pub_current);             //publish current

            std::cout << "Errors: " << std::endl;
            unsigned char errorBits = (msg[4]);
            unsigned char error  = 0;            
            for(int i = 0; i < 8; ++i)
            {
              error = errorBits << 1;
              if(error)
              {
                switch(i){
                  case 0:
                    std::cout << "\t Motor overload" << std::endl;
                    break;
                  case 1:
                    std::cout << "\t Clutch overload" << std::endl;
                    break;
                  case 2:
                    std::cout << "\t Motor open load" << std::endl;
                    break;
                  case 3:
                    std::cout << "\t Clutch open load" << std::endl;
                    break;
                  case 4:
                    std::cout << "\t Position Reach Error" << std::endl;
                    break;
                  case 5:
                    std::cout << "\t HW Warning 1" << std::endl;
                    break;
                  case 6:
                    std::cout << "\t HW Warning 2" << std::endl;
                    break;
                  default:
                    std::cout << "\t Unknown Error" << std::endl;
                    break;
                }
                error = 0;
              }
            }
            if(!errorBits)
              std::cout << "\t No Errors" << std::endl;

            std_msgs::UInt8 pub_errors;
            pub_errors.data = errorBits;
            actuator_errors_pub.publish(pub_errors);
          }
        }
        break;
      case canERR_NOMSG:
        break;
      default:
        perror("canReadBlock error");
        break;
      }
      
      ros::spinOnce();
                
    } while((ret==canOK)&&(ros::ok()));
    willExit=1;
  } 
   
  canClose(h);   
  sighand(SIGALRM);

  return 0;
}

