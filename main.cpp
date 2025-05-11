#include <iostream>
#include <cstdint>

typedef uint64_t Bitboard;

static constexpr void set_1(Bitboard &bb, uint8_t square) 
{
    bb = bb | (1ull << square);
}
static constexpr void set_0(Bitboard &bb, uint8_t square) 
{
    bb = bb & (~(1ull << square));
}

static constexpr bool get_bit(Bitboard bb, uint8_t square) 
{
    return (bb & (1ull << square));
}

int main()
{


    return 0;
}