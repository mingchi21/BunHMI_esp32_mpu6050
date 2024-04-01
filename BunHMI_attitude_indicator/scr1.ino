// Screen1
void scr1_init() {
  // TODO: Screen init
}

void scr1_cmd(const char* cmd) {
  // TODO: Screen init
}
void scr1_update() {
  char buff[256];
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

// acceleration range
#define MAX_ABS_VAL 18
  int x_val = map(a.acceleration.x, -MAX_ABS_VAL, MAX_ABS_VAL, 0, 100);
  int y_val = map(a.acceleration.y, -MAX_ABS_VAL, MAX_ABS_VAL, 0, 100);
  int z_val = map(a.acceleration.z, -MAX_ABS_VAL, MAX_ABS_VAL, 0, 100);
  HMI_CMD("bar_x.val(%d,1);bar_y.val(%d,1);bar_z.val(%d,1);", x_val, y_val, z_val);
}