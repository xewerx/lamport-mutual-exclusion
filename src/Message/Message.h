struct Message
{
    int sender;
    int clock;

    Message(int s, int c) : sender(s), clock(c) {}

    // Operator porównania, używany do sortowania w kolejce
    bool operator<(const Message &other) const
    {
        // Jesli takie same clocki to pierwszenstwo ma mniejsze id
        if (clock == other.clock)
        {
            return sender > other.sender;
        }

        // Sortowanie względem clock rosnaco
        return clock > other.clock;
    }
};