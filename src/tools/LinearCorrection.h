#ifndef _LINEAR_CORRECTION_H_
#define _LINEAR_CORRECTION_H_

#include <base/RzConfigurable.h>
#include "RzFiles.h"

//const float DEFAULT_REF_LOW = 0;
//const float DEFAULT_RAW_LOW = 0;
//const float DEFAULT_REF_HIGH = 100;
//const float DEFAULT_RAW_HIGH = 100;

class LinearCorrection : public RzConfigurable {
public:
    LinearCorrection(const char *id, //
                     float refLow, float refHigh, float rawLow, float rawHigh);

    virtual ~LinearCorrection();

    float fixValue(float) const;

    void displayConfig() const;

    // -=[ RzConfigurable ]=---------------------------
    String getJsonConfig() override;

    void loadConfiguration() override;

    void saveConfiguration() override;

    // -=[ RzIdentifiable ]=---------------------------

    const char *getId() override;

    const char *getDisplayName() override;

    const char *getPrefix() override;

private:
    const char *_id;
    float _refLow;
    float _refHigh;
    float _rawLow;
    float _rawHigh;
    float _refRange;
    float _rawRange;
};

#endif
