export module gameboy;
import bus;
import cpu;

export class GameBoy
{
    Bus bus;
    CPU cpu;

  public:
    void step();
};
