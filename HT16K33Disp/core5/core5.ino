//----------------------------------------
// Count form 00 to 99 on HT16K33 Display
// using HT16K33Disp library (Ver 1.0)
// by: Anas Kuzechie (May 03, 2022)
//----------------------------------------
#include <Wire.h>
#include <HT16K33Disp.h>
//-----------------------
HT16K33Disp disp1(0x70, 1);
HT16K33Disp disp2(0x71, 1);
HT16K33Disp disp3(0x72, 1);
//------------------------------------------------
void setup()
{
  byte brightness1[1] = {1};
  byte brightness2[1] = {8};
  byte brightness3[1] = {15};

  Wire.begin();

  disp1.Init(brightness1); 
  disp2.Init(brightness2); 
  disp3.Init(brightness3); 

  disp1.clear();
  disp2.clear();
  disp3.clear();

  delay(1000);
}
//------------------------------------------------

bool running1 = false, running2 = false, running3 = false;
void loop()
{
  unsigned long time = millis();
  char buffer[13] = "            ";
  if(!running1)
    disp1.begin_scroll_string(buffer, 300, 50);
  if(!running2)
    disp2.begin_scroll_string(buffer, 200, 50);
  if(!running3){
    disp3.begin_scroll_string(buffer, 100, 50);
  } 
  
  sprintf(buffer, "   %8lu", time / 1000000);
  disp1.update_scroll_string(buffer);

  sprintf(buffer, "    %8lu", time / 1000);
  disp2.update_scroll_string(buffer);

  sprintf(buffer, "    %8lu", time);
  disp3.update_scroll_string(buffer);

  running1 = disp1.step_scroll_string();
  running2 = disp2.step_scroll_string();
  running3 = disp3.step_scroll_string();
  
  // int count = disp1.string_length("This is a test.");

  // disp1.segments_test();
  // delay(1000);

  // disp1.patterns_test();
  // delay(1000);

  // unsigned long count;
  // char buffer[13];
  // while(true){
  //   //++count;
  //   count = micros();
  //   long seconds = count / 1000000;
  //   long remainder = count - (seconds * 1000000);
  //   sprintf(buffer, "%lu.%06lu00", seconds, remainder);  
  //   disp1.show_string(buffer, false, true);
    // disp1.show_string(buffer, false, false);
  // }
  
  // disp1.scroll_string(buffer);
  // delay(1000); 
  
  // disp1.show_string("$ .25");

  // delay(10000);
  // disp1.clear();

//   delay(1000);
//   disp1.Text("Coun");
//   delay(1000);
  // disp1.Text("t=");
//   //------------------------------
//   for(byte MSD=0; MSD<=9; MSD++)
//   {
//     disp1.Num(2, MSD);
//     for(byte LSD=0; LSD<=9; LSD++)
//     {
//       disp1.Num(3, LSD);
//       delay(10);
//     }
//   }
//   //------------------------------
//   delay(1000);
//   disp1.clear();
//   delay(1000);
}