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

  // Pitch angle
  int pitch_offset_y = map(a.acceleration.x, -5, 5, -53, 0);
  if (pitch_offset_y > 0) {
    pitch_offset_y = 0;
  }
  if (pitch_offset_y < -53) {
    pitch_offset_y = -53;
  }

  // Rotation angle
  int rotation_angle = map(a.acceleration.y, -18, 18, -800, 800);
  if (rotation_angle < -800) {
    rotation_angle = -800;
  }
  if (rotation_angle > 800) {
    rotation_angle = 800;
  }
  if(++interleave_cnt & 1){
    HMI_CMD("pitch.offset_y(%d)",  pitch_offset_y);
  }else{
    HMI_CMD("roll.angle(%d);pitch.angle(%d)", rotation_angle, rotation_angle);
  }

#undef PIVOT_X
#undef PIVOT_Y
#undef OFFSET_Y
}