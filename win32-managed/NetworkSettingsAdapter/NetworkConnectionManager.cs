/**
 * Copyright (c) 2014, Plexteq OÜ
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Management;
using System.Text;
using System.Threading.Tasks;

namespace NetworkSettingsAdapter
{
    public class NetworkConnectionManager
    {
        /// <summary>
        /// Changes dns records
        /// </summary>
        /// <param name="dns"></param>
        /// <param name="uuid"></param>
        public void SetDns(string[] dns, string uuid)
        {
            ManagementClass manageClass = new ManagementClass(AppConfiguration.NET_CONFIG);
            ManagementObjectCollection objCollection = manageClass.GetInstances();
            foreach (ManagementObject item in objCollection)
            {
                if ((string)item[AppConfiguration.SETTING_ID_KEY] == uuid && (bool)item[AppConfiguration.IP_ENABLED_KEY])
                {
                    ManagementBaseObject baseObj = item.GetMethodParameters(AppConfiguration.SET_SEARCH_ORDER_KEY);
                    baseObj[AppConfiguration.SEARCH_ORDER_KEY] = dns;
                    ManagementBaseObject objNewDNS = item.InvokeMethod(AppConfiguration.SET_SEARCH_ORDER_KEY, baseObj, null);
                    break;
                }
            }
        }

        /// <summary>
        /// Switch from dhcp to static mode
        /// </summary>
        /// <param name="uuid"></param>
        public void SwitchDhcpToStaticMode(string uuid)
        {
            ManagementClass manageClass = new ManagementClass(AppConfiguration.NET_CONFIG);
            ManagementObjectCollection objCollection = manageClass.GetInstances();
            foreach (ManagementObject item in objCollection)
            {
                if ((bool)item[AppConfiguration.IP_ENABLED_KEY] && (string)item[AppConfiguration.SETTING_ID_KEY] == uuid)
                {
                    AdapterEntity adapter = new AdapterEntity();
                    adapter = GetAdapterInfo(uuid);

                    ManagementBaseObject objNewIP = item.GetMethodParameters(AppConfiguration.ENABLE_STATIC_KEY);
                    ManagementBaseObject objNewGate = item.GetMethodParameters(AppConfiguration.SET_GATEWAYS_KEY);

                    //Set DefaultGateway
                    objNewGate[AppConfiguration.DEFAULT_IP_GATEWAY_KEY] = new string[] { adapter.DefaultIpGateway };
                    objNewGate[AppConfiguration.GATEWAY_COST_METRIC_KEY] = new int[] { 1 };

                    //Set IPAddress and Subnet Mask
                    objNewIP[AppConfiguration.IP_ADDRESS_KEY] = new string[] { adapter.IpAddress };
                    objNewIP[AppConfiguration.SUBNET_MASK_KEY] = new string[] { adapter.NetMask };

                    ManagementBaseObject objSetIP = item.InvokeMethod(AppConfiguration.ENABLE_STATIC_KEY, objNewIP, null);
                    objNewGate = item.InvokeMethod(AppConfiguration.SET_GATEWAYS_KEY, objNewGate, null);
                }
            }
        }

        /// <summary>
        /// Switch from static to dhcp mode
        /// </summary>
        /// <param name="uuid"></param>
        public void SwitchStaticToDhcpMode(string uuid)
        {
            ManagementClass objMC = new ManagementClass(AppConfiguration.NET_CONFIG);
            ManagementObjectCollection objMOC = objMC.GetInstances();
            foreach (ManagementObject objMO in objMOC)
            {
                if ((bool)objMO[AppConfiguration.IP_ENABLED_KEY] && (string)objMO[AppConfiguration.SETTING_ID_KEY] == uuid)
                    objMO.InvokeMethod(AppConfiguration.ENABLE_DHCP_KEY, new object[] { });
            }
        }


        /// <summary>
        /// Geting information about all available interfaces
        /// </summary>
        /// <returns></returns>
        public List<AdapterEntity> GetAdaptersInfo()
        {
            //Retrieves a collection of management objects based on a specified query.
            ManagementObjectSearcher networkAdapterSearcher = new ManagementObjectSearcher("root\\cimv2",
                "select * from Win32_NetworkAdapterConfiguration");
            //Enumerator on the collection.
            ManagementObjectCollection objectCollection = networkAdapterSearcher.Get();

            List<AdapterEntity> adaptersList = new List<AdapterEntity>();

            foreach (ManagementObject networkAdapter in objectCollection)
            {
                //Getting a set of network adapter properties
                PropertyDataCollection networkAdapterProperties = networkAdapter.Properties;

                //Checking whether IP is enabled
                if (IsIpEnabled(networkAdapterProperties))
                    adaptersList.Add(SwitchInterfaceProperties(networkAdapterProperties));
            }

            return adaptersList;
        }

        /// <summary>
        /// Retrieving about specific interface
        /// </summary>
        /// <param name="UUID"></param>
        /// <returns></returns>
        public AdapterEntity GetAdapterInfo(string UUID)
        {
            //Retrieves a collection of management objects based on a specified query.
            ManagementObjectSearcher networkAdapterSearcher = new ManagementObjectSearcher("root\\cimv2",
                "select * from Win32_NetworkAdapterConfiguration");
            //Enumerator on the collection.
            ManagementObjectCollection objectCollection = networkAdapterSearcher.Get();

            //Getting exact interface 
            foreach (ManagementObject networkAdapter in objectCollection)
            {
                //Getting a set of network adapter properties
                PropertyDataCollection networkAdapterProperties = networkAdapter.Properties;
                //Checking whether IP is enabled
                if (IsIpEnabled(networkAdapterProperties))
                    if (UUID.Equals(SwitchInterfaceProperties(networkAdapterProperties).UUID))
                        return SwitchInterfaceProperties(networkAdapterProperties);
            }
            throw new Exception("Invalid UUID");
        }



        private bool IsIpEnabled(PropertyDataCollection networkAdapterProperties)
        {
            foreach (PropertyData networkAdapterProperty in networkAdapterProperties)
            {
                if (networkAdapterProperty.Name.Equals(AppConfiguration.IP_ENABLED_KEY) && (bool)networkAdapterProperty.Value)
                    return true;
            }
            return false;
        }

        private AdapterEntity SwitchInterfaceProperties(PropertyDataCollection networkAdapterProperties)
        {
            AdapterEntity adapterEntity = new AdapterEntity();
            foreach (PropertyData networkAdapterProperty in networkAdapterProperties)
            {
                switch (networkAdapterProperty.Name)
                {
                    case "IPAddress":
                        foreach (string ipAddress in (string[])networkAdapterProperty.Value)
                        {
                            adapterEntity.IpAddress = ipAddress;
                            break;
                        }
                        break;
                    case "IPSubnet":
                        foreach (string netMask in (string[])networkAdapterProperty.Value)
                        {
                            adapterEntity.NetMask = netMask;
                            break;
                        }
                        break;
                    case "DefaultIPGateway":
                        if ((string[])networkAdapterProperty.Value != null)
                            foreach (string defaultIPGateway in (string[])networkAdapterProperty.Value)
                            {
                                adapterEntity.DefaultIpGateway = defaultIPGateway;
                                break;
                            }
                        else
                            adapterEntity.DefaultIpGateway = "";
                        break;
                    case "SettingID":
                        adapterEntity.UUID = (string)networkAdapterProperty.Value;
                        break;
                    case "DHCPEnabled":
                        if (networkAdapterProperty.Value != null)
                            adapterEntity.IsDhcpEnabled = (bool)networkAdapterProperty.Value;
                        else
                            adapterEntity.IsDhcpEnabled = false;
                        break;
                    case "DNSServerSearchOrder":
                        if ((string[])networkAdapterProperty.Value != null)
                            adapterEntity.DNS = (string[])networkAdapterProperty.Value;
                        else
                            adapterEntity.DNS = new string[] { "", "" };
                        break;
                    default:
                        break;
                }
            }
            return adapterEntity;
        }
    }
}
