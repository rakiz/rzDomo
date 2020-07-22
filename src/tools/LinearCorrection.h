#ifndef _LINEAR_CORRECTION_H_
#define _LINEAR_CORRECTION_H_

class LinearCorrection {
public:
    LinearCorrection(float _refLow, float _refHigh, float _rawLow, float _rawHigh);

    float fixValue(float) const;

    void displayConfig();

private:
    float refHigh;
    float rawHigh;
    float refLow;
    float rawLow;
    float rawRange;
    float refRange;
};

#endif
