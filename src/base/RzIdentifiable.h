#ifndef _RZ_IDENTIFIABLE_H_
#define _RZ_IDENTIFIABLE_H_

class RzIdentifiable {
public:

    virtual const char *getId() = 0;

    virtual const char *getDisplayName() = 0;

    virtual const char *getPrefix() = 0;
};

#endif
