// Screen3
void scr3_init() {
  // TODO: Screen init
}

void scr3_cmd(const char* cmd) {
  // TODO: handle cmd of screen
}
void scr3_update() {

  char buff[256];
  static uint8_t interleave_cnt = 0;

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

#define PIVOT_X 140
#define PIVOT_Y 115
#define OFFSET_Y -25

  // Pitch angle
  int x_val = map(a.acceleration.x, -5, 5, -53, 0);
  if (x_val > 0) {
    x_val = 0;
  }
  if (x_val < -53) {
    x_val = -53;
  }
  // Due to offset_y of pitch is changed, the pivot_y also need adjust to get correct rotation angle.
  int pivot_y = PIVOT_Y - (x_val - (OFFSET_Y));

  // Rotation angle
  int y_val = map(a.acceleration.y, -18, 18, -800, 800);
  if (y_val < -800) {
    y_val = -800;
  }
  if (y_val > 800) {
    y_val = 800;
  }

// Due to image rotation take a heavy time, we need to interleave the BunTalk cmd
switch(++interleave_cnt&0x07)
{
  case 0:
  HMI_CMD("pitch.pivot(%d,%d);pitch.offset_y(%d)", PIVOT_X, pivot_y, x_val);
  break;
  case 3:
  HMI_CMD("pitch.angle(%d)", y_val);
  break;
  case 5:
  HMI_CMD("roll.angle(%d)", y_val);
  break;
  default:
  break;
}
#undef PIVOT_X
#undef PIVOT_Y
#undef OFFSET_Y
}