// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <arduino-timer.h>  // https://www.arduino.cc/reference/en/libraries/arduino-timer/

Adafruit_MPU6050 mpu;
auto timer = timer_create_default();

#define EOT 0x04
#define HMI Serial2
#define HMI_CMD(...) \
  { \
    HMI.printf(__VA_ARGS__); \
    HMI.write(EOT); \
  }

// screen function declear
void scr1_init();
void scr1_cmd(const char *);
void scr1_update();
void scr2_init();
void scr2_cmd(const char *);
void scr2_update();
void scr3_init();
void scr3_cmd(const char *);
void scr3_update();


int scr_idx = 0;
typedef struct
{
  void (*init)();
  void (*cmd)(const char *);
  void (*update)();
} SCREEN_t;

SCREEN_t scr_table[] = {
  { scr1_init, scr1_cmd, scr1_update },
  { scr2_init, scr2_cmd, scr2_update },
  { scr3_init, scr3_cmd, scr3_update },
};

void setup(void) {
  Serial.begin(115200);
  //while (!Serial)
  //    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");


  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }

  Serial.println("");

  HMI.begin(115200);
  HMI.setTimeout(10);
  // Wait HMI bootup
  delay(100);
  // Send dummy cmd to make HMI clear cmd buffer
  HMI.printf("\x04\x04");
  // flush serial rx buffer
  char buff[256];
  int rxlen;
  do {
    rxlen = HMI.readBytes(buff, sizeof(buff));
  } while (rxlen > 0);

  scr_table[scr_idx].init();
  timer.every(100, timer_tick);
}

void loop() {

  char rxbuff[256];
  int rxlen;

  timer.tick();
  // Check Data from HMI
  rxlen = rxHmiData(rxbuff, sizeof(rxbuff));
  if (rxlen) {
    Serial.printf("HMI:%s\r\n", rxbuff);
    // Handle data from HMI
    handle_hmi_cmd(rxbuff);
  }
}

/**
  * Description: Receive Data from HMI
  * Parameters:
  *  dat: Data buffer
  *  dat_len: Data buffer length
  * Return:
  *  length of rx bytes
*/
int rxHmiData(char *dat, int dat_len) {
  static char hmiBuff[256];
  static uint8_t hmiBuffLen = 0;

  if (!HMI.available()) {
    return 0;
  }
  int rxlen = HMI.readBytes(hmiBuff + hmiBuffLen, sizeof(hmiBuff) - hmiBuffLen);
  int i;
  hmiBuffLen += rxlen;
  for (i = hmiBuffLen - 1; i >= 0; i--) {
    if (hmiBuff[i] == EOT) {
      // Got EOT Byte
      hmiBuff[i++] = 0;  // Change EOT to NULL,  string  terminate
      int hmi_len = (i < dat_len) ? i : dat_len;
      // Copy hmiBuff to dat
      memcpy(dat, hmiBuff, hmi_len);
      // Move remain data to the head of hmiBuff
      int remain_len = 0;
      while (i < hmiBuffLen) {
        hmiBuff[remain_len] = hmiBuff[i];
        remain_len++;
        i++;
      }
      hmiBuffLen = remain_len;
      return hmi_len;
    }
  }
  return 0;
}

// Set change screen of HMI and screen index
void set_scr_idx(int idx) {
  Serial.printf("set scr idx=%d", idx);
  int scr_tab_cnt = sizeof(scr_table) / sizeof(scr_table[0]);
  if (idx < scr_tab_cnt) {
    HMI_CMD("sys.scr(scr%d)", idx + 1);
    scr_idx = idx;
    scr_table[scr_idx].init();
  }
}

// Handle hmi cmd
void handle_hmi_cmd(const char *cmd) {
  if (strncmp("SCR:", cmd, 4) == 0) {
    int val = strtoul(cmd + 4, NULL, 0);
    set_scr_idx(val - 1);
    return;
  }
  scr_table[scr_idx].cmd(cmd);
}

/**
Run tick for every 100ms
*/
bool timer_tick(void *) {
  scr_table[scr_idx].update();
  return true;
}