#include <I2Cdev.h>
#include <MPU9250_9Axis_MotionApps41.h>
#include <helper_3dmath.h>
#include <math.h>

MPU9250 mpu;

#define OUTPUT_READABLE_QUATERNION
#define INTERRUPT_PIN 2

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
float f_mag[3];   
float d_mag[4];

float rollVal, pitchVal, yawVal;

Quaternion q;           // [w, x, y, z]         quaternion container

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

void setup() {
    // put your setup code here, to run once:
    Wire.begin();
    Wire.setClock(400000);

    Serial.begin(115200);

    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    devStatus = mpu.dmpInitialize();

    if (devStatus == 0) {
        mpu.setDMPEnabled(true);
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);

        dmpReady = true;

        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        Serial.println("Error setting up device");
        exit(1);
    }

    mpu.setFullScaleGyroRange(MPU9250_GYRO_FS_500);
}

bool readDMPMeasurements() {
    if (!dmpReady){
        Serial.println("DMP not ready");
        return false;
    }

    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        // Serial.println(F("FIFO overflow!"));
        return false;
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        return true;
    }
}

void printEulerAngles() {
    float data[3] = {0};
    mpu.dmpGetEuler(data, &q);

    Serial.print("degrees: ");
    Serial.print(data[0]*180/PI);
    Serial.print(",");
    Serial.print(data[1]*180/PI);
    Serial.print(",");
    Serial.print(data[2]*180/PI);
}

void printQuaternian() {
    mpu.dmpGetQuaternion(&q, fifoBuffer);

    Serial.print("quat\t");
    Serial.print(q.w);
    Serial.print("\t");
    Serial.print(q.x);
    Serial.print("\t");
    Serial.print(q.y);
    Serial.print("\t");
    Serial.print(q.z);
}

void printGyroVel() {
    int16_t gyroData[3] = {0};
    mpu.dmpGetGyro(gyroData, fifoBuffer);

    Serial.print("gyro: ");
    Serial.print(gyroData[0]);
    Serial.print(",");
    Serial.print(gyroData[1]);
    Serial.print(",");
    Serial.print(gyroData[2]);
}

void printQuatToSpeed() {
    
}

void loop() {
    readDMPMeasurements();

    //printQuaternian();
    //Serial.println();
    
    printGyroVel();
    Serial.println();

    Serial.println();
    delay(100);
}
