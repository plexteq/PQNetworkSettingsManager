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
