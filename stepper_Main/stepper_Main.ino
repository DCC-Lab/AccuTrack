// CREDITS FOR FILTER: curiores

template <int order> // order is 1 or 2
class LowPass
{
  private:
    float a[order];
    float b[order+1];
    float omega0;
    float dt;
    bool adapt;
    float tn1 = 0;
    float x[order+1]; // Raw values
    float y[order+1]; // Filtered values

  public:  
    LowPass(float f0, float fs, bool adaptive){
      // f0: cutoff frequency (Hz)
      // fs: sample frequency (Hz)
      // adaptive: boolean flag, if set to 1, the code will automatically set
      // the sample frequency based on the time history.
      
      omega0 = 6.28318530718*f0;
      dt = 1.0/fs;
      adapt = adaptive;
      tn1 = -dt;
      for(int k = 0; k < order+1; k++){
        x[k] = 0;
        y[k] = 0;        
      }
      setCoef();
    }

    void setCoef(){
      if(adapt){
        float t = micros()/1.0e6;
        dt = t - tn1;
        tn1 = t;
      }
      
      float alpha = omega0*dt;
      if(order==1){
        a[0] = -(alpha - 2.0)/(alpha+2.0);
        b[0] = alpha/(alpha+2.0);
        b[1] = alpha/(alpha+2.0);        
      }
      if(order==2){
        float alphaSq = alpha*alpha;
        float beta[] = {1, sqrt(2), 1};
        float D = alphaSq*beta[0] + 2*alpha*beta[1] + 4*beta[2];
        b[0] = alphaSq/D;
        b[1] = 2*b[0];
        b[2] = b[0];
        a[0] = -(2*alphaSq*beta[0] - 8*beta[2])/D;
        a[1] = -(beta[0]*alphaSq - 2*beta[1]*alpha + 4*beta[2])/D;      
      }
    }

    float filt(float xn){
      // Provide me with the current raw value: x
      // I will give you the current filtered value: y
      if(adapt){
        setCoef(); // Update coefficients if necessary      
      }
      y[0] = 0;
      x[0] = xn;
      // Compute the filtered values
      for(int k = 0; k < order; k++){
        y[0] += a[k]*y[k+1] + b[k]*x[k];
      }
      y[0] += b[order]*x[order];

      // Save the historical values
      for(int k = order; k > 0; k--){
        y[k] = y[k-1];
        x[k] = x[k-1];
      }
  
      // Return the filtered value    
      return y[0];
    }
};

// Filter instance
LowPass<2> lp(3,1e3,true);



// CREDITS FOR MOTOR: curiousscientist 
// EDIT: T.Shooner
// Link to tutorial : https://curiousscientist.tech/blog/arduino-accelstepper-tb6600-stepper-motor

//Transforming the motor's rotary motion into linear motion by using a threaded rod:
//Threaded rod's pitch = 2 mm. This means that one revolution will move the nut 2 mm.
//Default stepping = 400 step/revolution.
// 400 step = 1 revolution = 8 mm linear motion. (4 start 2 mm pitch screw)
// 1 cm = 10 mm =>> 10/8 * 400 = 4000/8 = 500 steps are needed to move the nut by 1 cm.
 
 
//character for commands
/*
     'C' : Prints all the commands and their functions.
     'P' : Rotates the motor in positive (CW) direction, relative.
     'N' : Rotates the motor in negative (CCW) direction, relative.
     'R' : Rotates the motor to an absolute positive position (+).
     'r' : Rotates the motor to an absolute negative position (-).
     'S' : Stops the motor immediately.
     'A' : Sets an acceleration value.
     'L' : Prints the current position/location of the motor.
     'H' : Goes back to 0 position from the current position (homing).
     'U' : Updates the position current position and makes it as the new 0 position.
 */

 
#include <AccelStepper.h>
// #include <vector>
// std::vector<float> myArray; // Declare a vector to hold the array

bool state = false; // initial state to show there is no values in the array
int myIndex = 0;
// float myArray[1000];
 
//User-defined values
long receivedSteps = 0; //Number of steps
long receivedSpeed = 0; //Steps / second
long receivedAcceleration = 0; //Steps / second^2

float t2;
float t1; // to measure execution time of displacement function


int analogPin = A0;
float sensorValue = 0.0;


char receivedCommand;
//-------------------------------------------------------------------------------
int directionMultiplier = 1; // = 1: positive direction, = -1: negative direction
bool newData, runallowed = false; // booleans for new data from serial, and runallowed flag
AccelStepper stepper(1, 2, 3);// direction Digital 3 (CCW), pulses Digital 2 (CLK)
 
void setup()
{
    Serial.begin(9600); //define baud rate
    Serial.println("Demonstration of AccelStepper Library"); //print a messages
    Serial.println("Send 'C' for printing the commands.");
 
    //setting up some default values for maximum speed and maximum acceleration
    Serial.println("Default speed: 6400 steps/s, default acceleration: 12800 steps/s^2.");
    stepper.setMaxSpeed(32000); //SPEED = Steps / second
    stepper.setAcceleration(12800); //ACCELERATION = Steps /(second)^2
 
    stepper.disableOutputs(); //disable outputs
}


void loop()
{   
  //Setup of arduino Due to 12 bit
    analogReadResolution(12);
    // int sensorAverage = movingAverage(1000); // initialisation of moving average
    // int sensorVoltage = map(analogRead(analogPin), 0, 4095, 0, 5000); // maping of 12 bit to 5000 mV

    //Constantly looping through these 2 functions.
    //We only use non-blocking commands, so something else (should also be non-blocking) can be done during the movement of the motor
    checkSerial(); //check serial port for new commands
    RunTheMotor(); //function to handle the motor
    if (stepper.isRunning() ==  true){
      myIndex++;
      if (myIndex >= 1000){
        // int sensorAverage = movingAverage(100);
        // int sensorVoltage = map(sensorAverage, 0, 4095, 0, 5000);
        int sensorVoltage = map(analogRead(analogPin), 0, 4095, 0, 5000);
        Serial.print(getPosition(sensorVoltage));
        Serial.print(", ");
        myIndex = 0;
      }
      state = true;
    }
    if (stepper.isRunning() == false && state == true){
      t2 = millis();
      Serial.println();
      Serial.print("Time taken by the task: "); Serial.print((t2-t1)/1000); Serial.println(" seconds");
      // printArray();
      // myIndex = 0;
      state = false;
    }
    // if (stepper.isRunning() == false && state == true){
    //   t2 = millis();
    //   Serial.print("Time taken by the task: "); Serial.print((t2-t1)/1000); Serial.println(" seconds");
    //   state = false;
    // }
 
}
 
 
void RunTheMotor() //function for the motor
{
    if (runallowed == true)
    {
        stepper.enableOutputs(); //enable pins
        stepper.run(); //step the motor (this will step the motor by 1 step at each loop)  
    }
    else //program enters this part if the runallowed is FALSE, we do not do anything
    {
        stepper.disableOutputs(); //disable outputs
        return;
    }
}
 
 
 
void checkSerial() //function for receiving the commands
{  
    if (Serial.available() > 0) //if something comes from the computer
    {
        receivedCommand = Serial.read(); // pass the value to the receivedCommad variable
        newData = true; //indicate that there is a new data by setting this bool to true
 
        if (newData == true) //we only enter this long switch-case statement if there is a new command from the computer
        {
            switch (receivedCommand) //we check what is the command
            {
 
            case 'P': //P uses the move() function of the AccelStepper library, which means that it moves relatively to the current position.              
               
                receivedSteps = Serial.parseFloat(); //value for the steps
                receivedSpeed = Serial.parseFloat(); //value for the speed
                directionMultiplier = 1; //We define the direction
                Serial.println("Positive direction."); //print the action
                t1 = millis();
                RotateRelative(); //Run the function
 
                //example: P2000 400 - 2000 steps (5 revolution with 400 step/rev microstepping) and 400 steps/s speed
                //In theory, this movement should take 5 seconds
                break;         
 
            case 'N': //N uses the move() function of the AccelStepper library, which means that it moves relatively to the current position.      
               
                receivedSteps = Serial.parseFloat(); //value for the steps
                receivedSpeed = Serial.parseFloat(); //value for the speed 
                directionMultiplier = -1; //We define the direction
                Serial.println("Negative direction."); //print action
                t1 = millis();
                RotateRelative(); //Run the function

                
 
                //example: N2000 400 - 2000 steps (5 revolution with 400 step/rev microstepping) and 500 steps/s speed; will rotate in the other direction
                //In theory, this movement should take 5 seconds
                break;
 
            case 'R': //R uses the moveTo() function of the AccelStepper library, which means that it moves absolutely to the current position.            
 
                receivedSteps = Serial.parseFloat(); //value for the steps
                receivedSpeed = Serial.parseFloat(); //value for the speed     
                directionMultiplier = 1; //We define the direction
                Serial.println("Absolute position (+)."); //print the action
                RotateAbsolute(); //Run the function
 
                //example: R800 400 - It moves to the position which is located at +800 steps away from 0.
                break;
 
            case 'r': //r uses the moveTo() function of the AccelStepper library, which means that it moves absolutely to the current position.            
 
                receivedSteps = Serial.parseFloat(); //value for the steps
                receivedSpeed = Serial.parseFloat(); //value for the speed 
                directionMultiplier = -1; //We define the direction
                Serial.println("Absolute position (-)."); //print the action
                RotateAbsolute(); //Run the function
 
                //example: r800 400 - It moves to the position which is located at -800 steps away from 0.
                break;
 
            case 'S': // Stops the motor
               
                stepper.stop(); //stop motor
                stepper.disableOutputs(); //disable power
                Serial.println("Stopped."); //print action
                runallowed = false; //disable running
                break;
 
            case 'A': // Updates acceleration
 
                runallowed = false; //we still keep running disabled, since we just update a variable
                stepper.disableOutputs(); //disable power
                receivedAcceleration = Serial.parseFloat(); //receive the acceleration from serial
                stepper.setAcceleration(receivedAcceleration); //update the value of the variable
                Serial.print("New acceleration value: "); //confirm update by message
                Serial.println(receivedAcceleration); //confirm update by message
                break;
 
            case 'L': //L: Location
 
                runallowed = false; //we still keep running disabled
                stepper.disableOutputs(); //disable power
                Serial.print("Current location of the motor: ");//Print the message
                Serial.println(stepper.currentPosition()); //Printing the current position in steps.
                break;
               
            case 'H': //H: Homing
 
                runallowed = true;     
                Serial.println("Homing"); //Print the message
                GoHome();// Run the function
                break;
 
            case 'U':
 
                runallowed = false; //we still keep running disabled
                stepper.disableOutputs(); //disable power
                stepper.setCurrentPosition(0); //Reset current position. "new home"            
                Serial.print("The current position is updated to: "); //Print message
                Serial.println(stepper.currentPosition()); //Check position after reset.
                break; 
 
            case 'C':
 
                PrintCommands(); //Print the commands for controlling the motor
                break;
 
            default:  

                break;
            }
        }
        //after we went through the above tasks, newData is set to false again, so we are ready to receive new commands again.
        newData = false;       
    }
}
 
 
void GoHome()
{  
    if (stepper.currentPosition() == 0)
    {
        Serial.println("We are at the home position.");
        stepper.disableOutputs(); //disable power
    }
    else
    {
        stepper.setMaxSpeed(12800); //set speed manually to 400. In this project 400 is 400 step/sec = 1 rev/sec.
        stepper.moveTo(0); //set abolute distance to move
    }
}
 
void RotateRelative()
{
    //We move X steps from the current position of the stepper motor in a given direction.
    //The direction is determined by the multiplier (+1 or -1)
   
    runallowed = true; //allow running - this allows entering the RunTheMotor() function.
    stepper.setMaxSpeed(receivedSpeed); //set speed

    stepper.move(directionMultiplier * receivedSteps); //set relative distance and direction
    
}
 
 
 
void RotateAbsolute()
{
    //We move to an absolute position.
    //The AccelStepper library keeps track of the position.
    //The direction is determined by the multiplier (+1 or -1)
    //Why do we need negative numbers? - If you drive a threaded rod and the zero position is in the middle of the rod...
 
    runallowed = true; //allow running - this allows entering the RunTheMotor() function.
    stepper.setMaxSpeed(receivedSpeed); //set speed
    stepper.moveTo(directionMultiplier * receivedSteps); //set relative distance

}

int movingAverage(int range){ // Function to average the analog value of the sensor
  int sum = 0;
  int i;

  for(i = 0; i < range; i++){
    int valueNow = analogRead(analogPin);
    sum = sum + valueNow;
  }

  int sensorAverage = sum/range;
  return sensorAverage;
}
// 
float getPosition(int voltage){ // Function to convert voltage to position 
  float p1 = -1*1.645;
  float p2 = 23680;
  float q1 = 193.4;
  float pos = (p1*voltage + p2)/(voltage + q1);
  return pos;
}

// void printArray(){
//   for(int i=0;i<myArray.size();i++){
//     Serial.print(myArray[i]);
//     Serial.print(" ");
//   }
//   Serial.println();
// }
 
void PrintCommands()
{  
    //Printing the commands
    Serial.println(" 'C' : Prints all the commands and their functions.");
    Serial.println(" 'P' : Rotates the motor in positive (CW) direction, relative.");
    Serial.println(" 'N' : Rotates the motor in negative (CCW) direction, relative.");
    Serial.println(" 'R' : Rotates the motor to an absolute positive position (+).");
    Serial.println(" 'r' : Rotates the motor to an absolute negative position (-).");
    Serial.println(" 'S' : Stops the motor immediately."); 
    Serial.println(" 'A' : Sets an acceleration value.");
    Serial.println(" 'L' : Prints the current position/location of the motor.");
    Serial.println(" 'H' : Goes back to 0 position from the current position (homing).");
    Serial.println(" 'U' : Updates the position current position and makes it as the new 0 position. ");  
} 
