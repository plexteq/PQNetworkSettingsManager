#Network Settings Manager client library for .NET



##Description
Library allows to manage Windows network adapter settings.



###Developer Documentation
1. Get information about available network interfaces (NI)

    ```
    new NetworkConnectionManager().GetAdaptersInfo(); // all available NI
    ```
    
Response example:
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
