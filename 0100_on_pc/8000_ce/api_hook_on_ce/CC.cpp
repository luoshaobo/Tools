#include "stdafx.h"
#include "CC.h"

int FFF(int n)
{
    int ret = n + 1;
    printf("in %s(): ret=%d\n", __FUNCTION__, ret);
    return ret;
}

int VVVn = 99;
char VVVc = 'X';
short VVVs = 55;

struct {
    const char *name;
    unsigned int age;
    bool sex;
} VVVst_Girl = {
    "Marry",
    20,
    false
};

////////////////////////////////////////////////////////////////////////////////////////

class BBB {
public:
    BBB();
    BBB(int n);
    ~BBB();

    static BBB &GetInstance();

    virtual int PubF(char *p);

private:
    int PriF(char *p);

public:
    int PubD;
    static int StaticD;

private:
    int PriD;
};
int BBB::StaticD = 111;

BBB::BBB() : PubD(0), PriD(0)
{
    printf("in %s()\n", __FUNCTION__);
}

BBB::BBB(int n) : PubD(0), PriD(0)
{
    printf("in %s()\n", __FUNCTION__);
}

BBB::~BBB()
{
    printf("in %s()\n", __FUNCTION__);
}

BBB &BBB::GetInstance()
{
    printf("in %s()\n", __FUNCTION__);
    static BBB BBB;
    return BBB;
}

int BBB::PubF(char *p)
{
    printf("in %s(\"%s\")\n", __FUNCTION__, p);
    return 0;
}

int BBB::PriF(char *p)
{
    printf("in %s()\n", __FUNCTION__);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////

class CCC : public BBB {
public:
    CCC();
    CCC(int n);
    ~CCC();

    static CCC &GetInstance();
    static CCC &GetInstance(int nPlaceHolder) {             // inline function
        printf("in %s()\n", __FUNCTION__);
        static CCC ccc;
        return ccc;
    }

    virtual int PubF(char *p);

private:
    int PriF(char *p);

public:
    int PubD;

private:
    int PriD;
};

CCC::CCC() : PubD(0), PriD(0)
{
    printf("in %s()\n", __FUNCTION__);
}

CCC::CCC(int n) : PubD(0), PriD(0)
{
    printf("in %s()\n", __FUNCTION__);
}

CCC::~CCC()
{
    printf("in %s()\n", __FUNCTION__);
}

int CCC::PubF(char *p)
{
    printf("in %s(\"%s\")\n", __FUNCTION__, p);
    return 0;
}

int CCC::PriF(char *p)
{
    printf("in %s()\n", __FUNCTION__);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////

class DDD : public CCC {
public:
    DDD();
    DDD(int n);
    ~DDD();

    static DDD &GetInstance();

    virtual int PubF(char *p);

private:
    int PriF(char *p);

public:
    int PubD;

private:
    int PriD;
};

DDD::DDD() : PubD(0), PriD(0)
{
    printf("in %s()\n", __FUNCTION__);
}

DDD::DDD(int n) : PubD(0), PriD(0)
{
    printf("in %s()\n", __FUNCTION__);
}

DDD::~DDD()
{
    printf("in %s()\n", __FUNCTION__);
}

DDD &DDD::GetInstance()
{
    printf("in %s()\n", __FUNCTION__);
    static DDD DDD;
    return DDD;
}

int DDD::PubF(char *p)
{
    printf("in %s(\"%s\")\n", __FUNCTION__, p);
    return 0;
}

int DDD::PriF(char *p)
{
    printf("in %s()\n", __FUNCTION__);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
CCC &CCC::GetInstance()
{
    printf("in %s()\n", __FUNCTION__);
    static DDD ccc;
    return ccc;
}

////////////////////////////////////////////////////////////////////////////////////////

class ForceToGenericCode
{
public:
    ForceToGenericCode() {
        CCC::GetInstance();
        CCC::GetInstance(0);
        DDD::GetInstance();
        &DDD::PubF;
        &FFF;
        printf("----------------------------------------------\n");
        printf("----------------------------------------------\n");
    }
};
static ForceToGenericCode oForceToGenericCode;































