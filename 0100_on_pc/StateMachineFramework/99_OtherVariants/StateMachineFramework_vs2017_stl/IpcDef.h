#ifndef IPC_DEF_H__8959406036898328572589347689209534967834589289589
#define IPC_DEF_H__8959406036898328572589347689209534967834589289589

enum {
    PUB_ID_PUB1 = SMF_BasePostOffice::PUB_ID_BASE,
    PUB_ID_PUB2,
};

enum {
    SUB_ID_SUB1 = PUB_ID_PUB1,
    SUB_ID_SUB2 = PUB_ID_PUB2,
};

enum {
    MAIL_ID_PUB1_MAIL001 = SMF_BasePostOffice::MAIL_ID_BASE,
    MAIL_ID_PUB1_MAIL002,
    MAIL_ID_PUB1_MAIL003,
};

enum {
    MAIL_ID_PUB2_MAIL001 = SMF_BasePostOffice::MAIL_ID_BASE,
    MAIL_ID_PUB2_MAIL002,
    MAIL_ID_PUB2_MAIL003,
};

struct MAIL_ID_PUB1_MAIL001_data {
    int nData[1];
};
struct MAIL_ID_PUB1_MAIL002_data {
    int nData[2];
};
struct MAIL_ID_PUB1_MAIL003_data {
    int nData[3];
};

struct MAIL_ID_PUB2_MAIL001_data {
    int nData[11];
};
struct MAIL_ID_PUB2_MAIL002_data {
    int nData[12];
};
struct MAIL_ID_PUB2_MAIL003_data {
    int nData[13];
};

#endif // #ifndef IPC_DEF_H__8959406036898328572589347689209534967834589289589
