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
using System.Text;
using System.Threading.Tasks;

namespace NetworkSettingsAdapter
{
    internal class AppConfiguration
    {
        public const string NET_CONFIG = "Win32_NetworkAdapterConfiguration";
        public const string SETTING_ID_KEY = "SettingID";
        public const string IP_ENABLED_KEY = "IPEnabled";
        public const string SET_SEARCH_ORDER_KEY = "SetDNSServerSearchOrder";
        public const string SEARCH_ORDER_KEY = "DNSServerSearchOrder";
        public const string ENABLE_STATIC_KEY = "EnableStatic";
        public const string SET_GATEWAYS_KEY = "SetGateways";
        public const string DEFAULT_IP_GATEWAY_KEY = "DefaultIPGateway";
        public const string GATEWAY_COST_METRIC_KEY = "GatewayCostMetric";
        public const string IP_ADDRESS_KEY = "IPAddress";
        public const string SUBNET_MASK_KEY = "SubnetMask";
        public const string ENABLE_DHCP_KEY = "EnableDHCP";
        public const string CAPTION_KEY = "Caption";
        public const string INTERFACE_INDEX_KEY = "InterfaceIndex";
        public const string NEXT_HOP_KEY = "NextHop";
        public const string ZERO_IP_KEY = "0.0.0.0";
        public const string MERTIC1_KEY = "Metric1";
        public const string MASK_KEY = "Mask";
        public const int INDEX_VALUE = -12;
    }
}
