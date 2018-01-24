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
using System.Net;
using System.Net.NetworkInformation;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace NetworkSettingsAdapter
{
    public class DefaultGatewayManager
    {

        /// <summary>
        /// Gets DefaultGateway and Metric 
        /// </summary>
        /// <returns></returns>
        public string[] GetDefaultGateway()
        {
            ManagementObjectSearcher networkAdapterSearcher = new ManagementObjectSearcher("root\\cimv2",
                "select * from Win32_NetworkAdapterConfiguration");
            ManagementObjectCollection objectCollection = networkAdapterSearcher.Get();

            var indexList = new List<string>();
            var nameList = new List<string>();
            var tableList = new List<string>();

            foreach (ManagementObject networkAdapter in objectCollection)
            {
                PropertyDataCollection networkAdapterProperties = networkAdapter.Properties;
                foreach (PropertyData networkAdapterProperty in networkAdapterProperties)
                {
                    if (networkAdapterProperty.Name.Equals(AppConfiguration.CAPTION_KEY) && string.IsNullOrEmpty(networkAdapterProperty.Name))
                        nameList.Add((string)networkAdapterProperty.Value);
                    if (networkAdapterProperty.Name.Equals(AppConfiguration.INTERFACE_INDEX_KEY) && string.IsNullOrEmpty(networkAdapterProperty.Name))
                        indexList.Add(networkAdapterProperty.Value.ToString());
                }
            }

            ManagementObjectSearcher searcher = new ManagementObjectSearcher(
                "select * from Win32_IP4RouteTable");
            

            foreach (ManagementObject adapterObject in searcher.Get())
            {
                if (!adapterObject[AppConfiguration.NEXT_HOP_KEY].Equals(AppConfiguration.ZERO_IP_KEY))
                {
                    string tableResult = string.Format("{0};{1};{2};", adapterObject[AppConfiguration.NEXT_HOP_KEY], adapterObject[AppConfiguration.MASK_KEY], 
                        adapterObject[AppConfiguration.MERTIC1_KEY].ToString(), (string)adapterObject[AppConfiguration.NEXT_HOP_KEY].ToString());
                    for (int i = 0; i < indexList.Count; i++)
                        if ((string)adapterObject[AppConfiguration.NEXT_HOP_KEY].ToString() == indexList[i])
                            tableResult += ";" + nameList[i];
                    tableList.Add(tableResult);
                }

            }
            return tableList.ToArray();
        }


        /// <summary>
        /// Removes DefaultGateway
        /// </summary>
        /// <param name="gateway"></param>
        /// <param name="mask"></param>
        /// <param name="metric"></param>
        public void RemoveDefaultGateway(string gateway, string mask, int metric)
        {
            ManagementObjectSearcher searcher = new ManagementObjectSearcher(
                "select * from Win32_IP4RouteTable");

            foreach (ManagementObject adapterObject in searcher.Get())
                if (adapterObject[AppConfiguration.MERTIC1_KEY].Equals(metric) && adapterObject[AppConfiguration.MASK_KEY].Equals(mask) && adapterObject[AppConfiguration.NEXT_HOP_KEY].Equals(gateway))
                    adapterObject.Delete();
        }

        /// <summary>
        /// Changes DefaultGateway with Destination 0.0.0.0
        /// </summary>
        /// <param name="IPAddress"></param>
        /// <param name="Mask"></param>
        /// <param name="Mertic"></param>
        /// <param name="interfaceIndex"></param>
        public void SetDefaultGateway(string IPAddress, string Mask, int Mertic, int interfaceIndex)
        {
            List<NetworkInterface> interfaces = new List<NetworkInterface>();
            foreach (var nic in NetworkInterface.GetAllNetworkInterfaces())
                if (nic.OperationalStatus == OperationalStatus.Up)
                    interfaces.Add(nic);

            NetworkInterface result = null;
            foreach (NetworkInterface nic in interfaces)
            {
                if (result == null)
                    result = nic;
                else
                    if (nic.GetIPProperties().GetIPv4Properties() != null)
                        if (nic.GetIPProperties().GetIPv4Properties().Index < result.GetIPProperties().GetIPv4Properties().Index)
                            result = nic;
            }

            if (interfaceIndex == AppConfiguration.INDEX_VALUE)
                interfaceIndex = result.GetIPProperties().GetIPv4Properties().Index;

            CreateIpForwardEntry(IPAddress, interfaceIndex, Mertic, Mask);
        }


        private int CreateIpForwardEntry(string nextHopIPAddress, int ifIndex, int matric, string Mask)
        {
            MIB_IPFORWARDROW mifr = new MIB_IPFORWARDROW();
            mifr.dwForwardDest = BitConverter.ToUInt32(IPAddress.Parse(AppConfiguration.ZERO_IP_KEY).GetAddressBytes(), 0);
            mifr.dwForwardMask = BitConverter.ToUInt32(IPAddress.Parse(Mask).GetAddressBytes(), 0);
            mifr.dwForwardPolicy = Convert.ToUInt32(0);
            mifr.dwForwardNextHop = BitConverter.ToUInt32(IPAddress.Parse(nextHopIPAddress).GetAddressBytes(), 0);
            mifr.dwForwardIfIndex = Convert.ToUInt32(ifIndex);
            mifr.dwForwardType = Convert.ToUInt32(4);
            mifr.dwForwardProto = Convert.ToUInt32(3);
            mifr.dwForwardAge = Convert.ToUInt32(0);
            mifr.dwForwardNextHopAS = Convert.ToUInt32(0);
            mifr.dwForwardMetric1 = Convert.ToUInt32(matric);
            mifr.dwForwardMetric2 = Convert.ToUInt32(matric);
            mifr.dwForwardMetric3 = Convert.ToUInt32(matric);
            mifr.dwForwardMetric4 = Convert.ToUInt32(matric);
            mifr.dwForwardMetric5 = Convert.ToUInt32(matric);
            return CreateIpForwardEntry(ref mifr);
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct MIB_IPFORWARDROW
        {
            public UInt32 dwForwardDest;
            public UInt32 dwForwardMask;
            public UInt32 dwForwardPolicy;
            public UInt32 dwForwardNextHop;
            public UInt32 dwForwardIfIndex;
            public UInt32 dwForwardType;
            public UInt32 dwForwardProto;
            public UInt32 dwForwardAge;
            public UInt32 dwForwardNextHopAS;
            public UInt32 dwForwardMetric1;
            public UInt32 dwForwardMetric2;
            public UInt32 dwForwardMetric3;
            public UInt32 dwForwardMetric4;
            public UInt32 dwForwardMetric5;
        }


        [DllImport("Iphlpapi.dll")]
        [return: MarshalAs(UnmanagedType.U4)]
        public static extern int CreateIpForwardEntry(ref MIB_IPFORWARDROW
        pRoute);
    }
}
