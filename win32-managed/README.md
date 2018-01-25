# Network Settings Manager client library for .NET



## Description
Library allows to manage Windows network adapter settings.



## Developer Documentation


*1. Get information about available network interfaces (NI)*

   ```
   new NetworkConnectionManager().GetAdaptersInfo();  
   ```
    
**Response example:**
  ```
  [
    {   "UUID":"{C1.....5E-E2C4-48F9-B3BD-DFF......7589}",
        "IpAddress":"1.1.1.1",
        "IsDhcpEnabled":true,
        "NetMask":"255.255.255.0",
        "DefaultIpGateway":"1.1.88.1",
        "DNS":["8.8.8.8","8.8.4.4"]
    }
]
  ```
*2. Retrieve information about specific interface by UUID*

```
    new NetworkConnectionManager().GetAdapterInfo(UUID); 
```


*3. Change DNS Servers*

```
        var dnsArray = new string[] { "8.8.8.8", "8.8.8.4" };
        new NetworkConnectionManager().SetDns(dnsArray, UUID); 
```


*4. Switch from dhcp to static mode*

 ```
     new NetworkConnectionManager().SwitchDhcpToStaticMode(UUID); 
 ```


*5. Switch from static to dhcp mode*

```
    new NetworkConnectionManager().SwitchStaticToDhcpMode(UUID); 
```
    
    
*6. Get DefaultGateway and Metric*

```
    new DefaultGatewayManager().GetDefaultGateway();
```


*7. Change DefaultGateway*
    You should specify an input parameters: IPAddress, Mask, Metri, Destination 

```
    new DefaultGatewayManager().SetDefaultGateway(ipAddress, mask, mertic, interfaceIndex);
```
    
    
*8. Remove DefaultGateway* 

```
    new DefaultGatewayManager().RemoveDefaultGateway(gateway, mask, mertic);
```
