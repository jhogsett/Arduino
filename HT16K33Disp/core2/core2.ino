//----------------------------------------
// Count form 00 to 99 on HT16K33 Display
// using HT16K33Disp library (Ver 1.0)
// by: Anas Kuzechie (May 03, 2022)
//----------------------------------------
#include <Wire.h>
#include <HT16K33Disp.h>
//-----------------------
HT16K33Disp disp(0x70);
//------------------------------------------------
void setup()
{
  Wire.begin();
  disp.Init(15); //8
  disp.clear();
  delay(1000);
}
//------------------------------------------------
void loop()
{
  disp.scroll_string("    **** ENJOY CANDY **** ONLY 25 CENTS ****    ");
  delay(1000);

  // int count = disp.string_length("This is a test.");

  // disp.segments_test();
  // delay(1000);

  // disp.patterns_test();
  // delay(1000);

  // char buffer[40];
  // sprintf(buffer, "The String Length Is %d !!", count);  

  // disp.scroll_string(buffer);
  // delay(1000);
  
  disp.show_string("$ .25");

  delay(10000);
  disp.clear();

//   delay(1000);
//   disp.Text("Coun");
//   delay(1000);
  // disp.Text("t=");
//   //------------------------------
//   for(byte MSD=0; MSD<=9; MSD++)
//   {
//     disp.Num(2, MSD);
//     for(byte LSD=0; LSD<=9; LSD++)
//     {
//       disp.Num(3, LSD);
//       delay(10);
//     }
//   }
//   //------------------------------
//   delay(1000);
//   disp.clear();
//   delay(1000);
}