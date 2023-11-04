#ifndef oled_h
#define oled_h

void set_oled_text(const char *text);
void set_team_scored(const char *text);
void update_oled_score();
void setup_ui();
void initialize_oled(void);

#endif