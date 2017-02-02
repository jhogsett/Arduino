void draw(rgb_color color, int pos, int id){
  colors[pos] = ColorMath::add_color(colors[pos], color);

  int mirror = MAX_LED - pos;
  colors[mirror] = ColorMath::add_color(colors[mirror], color);
  
  existence[pos] |= bitmask[id];
}

void undraw(rgb_color color, int pos, int id){
  existence[pos] &= ~bitmask[id];
}

