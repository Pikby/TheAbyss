inline int pack4chars(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
  return ((a << 24) | (b << 16) | (c << 8) | d);
}

inline int floatBitsToInt(float f)
{
    return *(int*)&f;
}
