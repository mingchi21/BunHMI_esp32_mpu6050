// Screen2
void scr2_init()
  {
    // TODO: Screen init
  }
  
  void scr2_cmd(const char * cmd)
  {
// TODO: handle cmd of screen
  }
  void scr2_update()
  {
     char buff[256];
    /* Get new sensor events with the readings */
   sensors_event_t a, g, temp;
   mpu.getEvent(&a, &g, &temp);

// acceleration range
#define MAX_ABS_VAL 18
int x_val = map(a.acceleration.x, -MAX_ABS_VAL, MAX_ABS_VAL, 0, 100);
int y_val = map(a.acceleration.y, -MAX_ABS_VAL, MAX_ABS_VAL, 0, 100);
int z_val = map(a.acceleration.z, -MAX_ABS_VAL, MAX_ABS_VAL, 0, 100);
HMI_CMD("chart.mnext(%d,%d,%d)", x_val, y_val, z_val);
}