@echo off
REM All of the IP addresses for all the domain as below must be set a new route!
REM ping res.wx.qq.com
REM ping wx.qq.com
REM ping p21.tcdn.qq.com
REM 
REM Note:
REM 1) Adding one item for these domain names as below:
REM        res.wx.qq.com
REM        wx.qq.com
REM        p21.tcdn.qq.com
REM    respectively to:
REM        C:\Windows\System32\drivers\etc\hosts
REM    is OK. And the IPs for the last two domain can be same.
echo on

if "%1" == "" (
    set WIFI_ROUTE_ADDR=192.168.11.1
) else (
    set WIFI_ROUTE_ADDR=%1
)

route add 140.206.160.161 mask 255.255.255.255 %WIFI_ROUTE_ADDR%
route add 203.205.136.141 mask 255.255.255.255 %WIFI_ROUTE_ADDR%
route add 103.7.28.33 mask 255.255.255.255 %WIFI_ROUTE_ADDR%

REM for wx.520wawa.com
route add 60.190.236.105 mask 255.255.255.255 %WIFI_ROUTE_ADDR%

