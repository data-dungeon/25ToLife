

class CTimer
{

public:
    CTimer();
    ~CTimer();

    void Start();
    float Stop();
    float GetEllapsedTime();

private:
    int GetTickDifference(unsigned tickStart, unsigned tickStop);
    unsigned GetTickCount();

    unsigned m_nStartTick;
    char m_bStarted;
};
