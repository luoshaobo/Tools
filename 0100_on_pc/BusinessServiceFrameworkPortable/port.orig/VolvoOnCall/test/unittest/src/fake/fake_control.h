#ifndef FAKE_VC_PROXY_CONTROL
#define FAKE_VC_PROXY_CONTROL

class FakeVCProxyControl
{
public :
   static void Init()
   {
     m_request_vin_number_flag = true;
     m_request_tcam_hw_ver_flag = true;
   }

   static void Set_Request_VINNumber_Success_Or_Fail(bool flag) { m_request_vin_number_flag = flag; }
   static bool Get_Request_VINNumber_Success_Or_Fail() { return m_request_vin_number_flag; }
   static void Set_Request_GetTcamHwVersion_Success_Or_Fail(bool flag) { m_request_tcam_hw_ver_flag = flag; }
   static bool Get_Request_GetTcamHwVersion_Success_Or_Fail() { return m_request_tcam_hw_ver_flag; }

private :
   static bool m_request_vin_number_flag;
   static bool m_request_tcam_hw_ver_flag;
};

class FakeVocPersistDataControl
{
public :
   static void Init()
   {
     m_uploaded_flag = 0;
   }

   static void Set_MTA_Uploaded_Flag(unsigned int flag){ m_uploaded_flag = flag; }
   static unsigned int Get_MTA_Uploaded_Flag(){ return m_uploaded_flag; }

private :
    static unsigned int m_uploaded_flag;
};

#endif
