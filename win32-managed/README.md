# Network Settings Manager client library for .NET



## Description
Library allows to manage Windows network adapter settings.


## Supported Frameworks

    .NET Framework 4.5.2+
    
    
## Dependencies

System.Management
System.Net.Http


## Developer Documentation

*1. Get information about available network interfaces (NI)*

```cs
new NetworkConnectionManager().GetAdaptersInfo();  
```
    
**Response example:**
```javascript
[
    {   "UUID":"{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}",
        "IpAddress":"1.1.1.1",
        "IsDhcpEnabled":true,
        "NetMask":"255.255.255.0",
        "DefaultIpGateway":"1.1.88.1",
        "DNS":["8.8.8.8","8.8.4.4"]
    }
]
```
*2. Retrieve information about specific interface by UUID*

```cs
new NetworkConnectionManager().GetAdapterInfo("{C103ED5E-E2C4-48F9-B3BD-DFFD4A9B7589}"); 
```
Example:

```
    {
        "UUID":"{C103ED5E-E2C4-48F9-B3BD-DFFD4A9B7589}"
        ,"IpAddress":"192.168.88.224",
        "IsDhcpEnabled":true,"NetMask":"255.255.255.0",
        "DefaultIpGateway":"192.168.88.1",
        "DNS":["8.8.8.8","8.8.4.4"]
        }
```

*3. Change DNS Servers*

```cs
var dnsArray = new string[] { "8.8.8.8", "8.8.8.4" };
new NetworkConnectionManager().SetDns(dnsArray, "{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}"); 
```


*4. Switch from dhcp to static mode*

```cs
new NetworkConnectionManager().SwitchDhcpToStaticMode("{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}"); 
```


*5. Switch from static to dhcp mode*

```cs
new NetworkConnectionManager().SwitchStaticToDhcpMode("{0EB0E025-3D65-4BFE-8F69-01A4B88F1A73}"); 
```
    
    
*6. Get DefaultGateway and Metric*

```cs
new DefaultGatewayManager().GetDefaultGateway();
```
    Example:
```
    ["192.168.88.1;0.0.0.0;25;"]
```

*7. Change DefaultGateway*
    You should specify an input parameters: IPAddress, Mask, Metri, Destination 

```cs
string ipAddress = "192.168.8.8";
string mask = "0.0.0.0";
string metric = 12;

new DefaultGatewayManager().SetDefaultGateway(ipAddress, mask, mertic, interfaceIndex);
```
    
    
*8. Remove DefaultGateway* 

```cs
string gateway = "192.168.8.8";
string mask = "0.0.0.0";
string mertic = 12;

new DefaultGatewayManager().RemoveDefaultGateway(gateway, mask, mertic);
```


## NuGet Packages
We have released a NuGet package. The library published by Plexteq is called Plexteq.Win32Managed.NetworkSettingsAdapter

Open Tools -> NuGet Package Manager -> Package Manager Console

```
    Install-Package Plexteq.Win32Managed.NetworkSettingsAdapter -Version 1.0.0
```
